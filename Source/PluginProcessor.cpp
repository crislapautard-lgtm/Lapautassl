
#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    struct Preset
    {
        const char* name;
        float threshold, ratio, attack, release, makeup, mix, hpf;
    };

    const Preset presets[] =
    {
        { "SSL 4000 Vocal",       -20.0f, 3.0f,  8.0f, 100.0f, 2.0f, 100.0f, 100.0f },
        { "SSL Vocal Tight",     -24.0f, 4.0f,  3.0f,  80.0f, 3.0f, 100.0f, 120.0f },
        { "SSL Drum Punch",      -18.0f, 4.0f, 10.0f, 100.0f, 2.0f, 100.0f,  80.0f },
        { "SSL Drum Bus",        -16.0f, 4.0f, 30.0f, 150.0f, 2.0f, 100.0f, 100.0f },
        { "SSL Bass Control",    -22.0f, 4.0f, 20.0f, 140.0f, 2.0f, 100.0f,  60.0f },
        { "SSL Mix Bus",         -12.0f, 2.0f, 30.0f, 200.0f, 1.0f, 100.0f,  90.0f },
        { "SSL Glue",            -10.0f, 2.0f, 30.0f, 300.0f, 1.0f, 100.0f,  90.0f },
        { "SSL Fast Attack",     -20.0f, 6.0f,  0.5f,  70.0f, 3.0f, 100.0f, 150.0f },
        { "SSL Slow Attack",     -18.0f, 3.0f, 30.0f, 250.0f, 2.0f, 100.0f,  80.0f }
    };

    constexpr int presetCount = sizeof(presets) / sizeof(presets[0]);
}

LaPautaSLCompresorAudioProcessor::LaPautaSLCompresorAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

LaPautaSLCompresorAudioProcessor::~LaPautaSLCompresorAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout
LaPautaSLCompresorAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    juce::NormalisableRange<float> hpfRange(20.0f, 20000.0f, 0.1f);
    hpfRange.setSkewForCentre(1000.0f);

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "THRESHOLD", 1 }, "Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -18.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "RATIO", 1 }, "Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f), 4.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ATTACK", 1 }, "Attack",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f), 10.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "RELEASE", 1 }, "Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f), 100.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "MAKEUP", 1 }, "Makeup",
        juce::NormalisableRange<float>(-12.0f, 24.0f, 0.1f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "MIX", 1 }, "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "HPF", 1 }, "Sidechain HPF",
        hpfRange, 100.0f));

    return layout;
}

void LaPautaSLCompresorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);
    currentSampleRate = sampleRate;
    envelope = 0.0f;

    for (int i = 0; i < 2; ++i)
    {
        previousInput[i] = 0.0f;
        previousHighPass[i] = 0.0f;
    }
}

void LaPautaSLCompresorAudioProcessor::releaseResources()
{
    envelope = 0.0f;
}

bool LaPautaSLCompresorAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    auto input = layouts.getMainInputChannelSet();
    auto output = layouts.getMainOutputChannelSet();

    return input == output &&
        (output == juce::AudioChannelSet::mono() ||
         output == juce::AudioChannelSet::stereo());
}

void LaPautaSLCompresorAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const int channels = juce::jmin(2, buffer.getNumChannels());

    const float threshold = parameters.getRawParameterValue("THRESHOLD")->load();
    const float ratio = juce::jmax(1.0f, parameters.getRawParameterValue("RATIO")->load());
    const float attack = parameters.getRawParameterValue("ATTACK")->load();
    const float release = parameters.getRawParameterValue("RELEASE")->load();
    const float makeup = parameters.getRawParameterValue("MAKEUP")->load();
    const float mix = parameters.getRawParameterValue("MIX")->load() / 100.0f;
    const float hpf = parameters.getRawParameterValue("HPF")->load();

    const float sr = static_cast<float>(currentSampleRate);

    const float attackCoef = std::exp(-1.0f / (0.001f * attack * sr));
    const float releaseCoef = std::exp(-1.0f / (0.001f * release * sr));

    const float hpfCoef = std::exp(
        -juce::MathConstants<float>::twoPi * hpf / sr);

    const float makeupGain = juce::Decibels::decibelsToGain(makeup);

    for (int s = 0; s < buffer.getNumSamples(); ++s)
    {
        float detector = 0.0f;

        for (int ch = 0; ch < channels; ++ch)
        {
            const float input = buffer.getSample(ch, s);

            const float filtered = hpfCoef *
                (previousHighPass[ch] + input - previousInput[ch]);

            previousInput[ch] = input;
            previousHighPass[ch] = filtered;

            detector = juce::jmax(detector, std::abs(filtered));
        }

        const float coefficient = detector > envelope ? attackCoef : releaseCoef;
        envelope = coefficient * envelope + (1.0f - coefficient) * detector;

        const float detectorDb = juce::Decibels::gainToDecibels(envelope, -100.0f);

        float reductionDb = 0.0f;

        if (detectorDb > threshold)
        {
            const float outputDb = threshold + (detectorDb - threshold) / ratio;
            reductionDb = outputDb - detectorDb;
        }

        const float compressionGain = juce::Decibels::decibelsToGain(reductionDb);
        const float wetGain = compressionGain * makeupGain;

        for (int ch = 0; ch < channels; ++ch)
        {
            const float dry = buffer.getSample(ch, s);
            const float wet = dry * wetGain;

            buffer.setSample(ch, s, dry * (1.0f - mix) + wet * mix);
        }
    }
}

juce::AudioProcessorEditor* LaPautaSLCompresorAudioProcessor::createEditor()
{
    return new LaPautaSLEditor(*this);
}

bool LaPautaSLCompresorAudioProcessor::hasEditor() const { return true; }
const juce::String LaPautaSLCompresorAudioProcessor::getName() const { return "La Pauta SL Compressor"; }
bool LaPautaSLCompresorAudioProcessor::acceptsMidi() const { return false; }
bool LaPautaSLCompresorAudioProcessor::producesMidi() const { return false; }
bool LaPautaSLCompresorAudioProcessor::isMidiEffect() const { return false; }
double LaPautaSLCompresorAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int LaPautaSLCompresorAudioProcessor::getNumPrograms() { return presetCount; }
int LaPautaSLCompresorAudioProcessor::getCurrentProgram() { return currentProgram; }

void LaPautaSLCompresorAudioProcessor::setCurrentProgram(int index)
{
    if (index < 0 || index >= presetCount)
        return;

    currentProgram = index;
    const auto& p = presets[index];

    auto setValue = [this](const juce::String& id, float value)
    {
        if (auto* parameter = parameters.getParameter(id))
            parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
    };

    setValue("THRESHOLD", p.threshold);
    setValue("RATIO", p.ratio);
    setValue("ATTACK", p.attack);
    setValue("RELEASE", p.release);
    setValue("MAKEUP", p.makeup);
    setValue("MIX", p.mix);
    setValue("HPF", p.hpf);
}

const juce::String LaPautaSLCompresorAudioProcessor::getProgramName(int index)
{
    if (index < 0 || index >= presetCount)
        return {};

    return presets[index].name;
}

void LaPautaSLCompresorAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void LaPautaSLCompresorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void LaPautaSLCompresorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LaPautaSLCompresorAudioProcessor();
}
