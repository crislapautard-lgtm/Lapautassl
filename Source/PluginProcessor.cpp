
#include "PluginProcessor.h"
#include "PluginEditor.h"

LaPautaSLAudioProcessor::LaPautaSLAudioProcessor()
    : AudioProcessor(
        BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    thresholdParam = parameters.getRawParameterValue("THRESHOLD");
    ratioParam = parameters.getRawParameterValue("RATIO");
    attackParam = parameters.getRawParameterValue("ATTACK");
    makeupParam = parameters.getRawParameterValue("MAKEUP");
    mixParam = parameters.getRawParameterValue("MIX");
}

juce::AudioProcessorValueTreeState::ParameterLayout
LaPautaSLAudioProcessor::createParameterLayout()
{
    using Parameter = juce::AudioParameterFloat;

    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<Parameter>(
        juce::ParameterID { "THRESHOLD", 1 },
        "Threshold", juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -12.0f));

    layout.add(std::make_unique<Parameter>(
        juce::ParameterID { "RATIO", 1 },
        "Ratio", juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f), 4.0f));

    layout.add(std::make_unique<Parameter>(
        juce::ParameterID { "ATTACK", 1 },
        "Attack", juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f), 10.0f));

    layout.add(std::make_unique<Parameter>(
        juce::ParameterID { "MAKEUP", 1 },
        "Makeup", juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 3.0f));

    layout.add(std::make_unique<Parameter>(
        juce::ParameterID { "MIX", 1 },
        "Mix", juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f));

    return layout;
}

void LaPautaSLAudioProcessor::prepareToPlay(double sampleRate, int)
{
    currentSampleRate = sampleRate;
    envelope = 0.0f;
    gainReductionDb.store(0.0f);
}

void LaPautaSLAudioProcessor::releaseResources()
{
    envelope = 0.0f;
    gainReductionDb.store(0.0f);
}

bool LaPautaSLAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    const auto output = layouts.getMainOutputChannelSet();

    return input == output &&
        (input == juce::AudioChannelSet::mono() ||
         input == juce::AudioChannelSet::stereo());
}

void LaPautaSLAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    midiMessages.clear();

    const int samples = buffer.getNumSamples();
    const int channels = getTotalNumInputChannels();

    for (int ch = channels; ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear(ch, 0, samples);

    if (samples == 0 || channels == 0)
        return;

    const float threshold = thresholdParam->load();
    const float ratio = juce::jmax(1.0f, ratioParam->load());
    const float attackMs = attackParam->load();
    const float makeupDb = makeupParam->load();
    const float mix = mixParam->load() * 0.01f;

    const float attackCoefficient = std::exp(
        -1.0f / (0.001f * attackMs * static_cast<float>(currentSampleRate)));

    const float releaseCoefficient = std::exp(
        -1.0f / (0.200f * static_cast<float>(currentSampleRate)));

    const float makeupLinear = juce::Decibels::decibelsToGain(makeupDb);

    const float dryGain = 1.0f - mix;
    const float wetGain = mix;

    float currentGR = 0.0f;

    for (int sample = 0; sample < samples; ++sample)
    {
        float detector = 0.0f;

        for (int ch = 0; ch < channels; ++ch)
            detector = juce::jmax(detector, std::abs(buffer.getSample(ch, sample)));

        const float coefficient =
            detector > envelope ? attackCoefficient : releaseCoefficient;

        envelope = coefficient * envelope + (1.0f - coefficient) * detector;

        const float envelopeDb = juce::Decibels::gainToDecibels(
            juce::jmax(envelope, 0.000001f));

        float targetGR = 0.0f;

        if (envelopeDb > threshold)
        {
            const float compressedDb =
                threshold + (envelopeDb - threshold) / ratio;

            targetGR = compressedDb - envelopeDb;
        }

        const float compressionGain =
            juce::Decibels::decibelsToGain(targetGR);

        currentGR = targetGR;

        for (int ch = 0; ch < channels; ++ch)
        {
            const float dry = buffer.getSample(ch, sample);
            const float wet = dry * compressionGain * makeupLinear;

            buffer.setSample(ch, sample,
                dry * dryGain + wet * wetGain);
        }
    }

    gainReductionDb.store(currentGR, std::memory_order_relaxed);
}

void LaPautaSLAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void LaPautaSLAudioProcessor::setStateInformation(
    const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(
        getXmlFromBinary(data, sizeInBytes));

    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* LaPautaSLAudioProcessor::createEditor()
{
    return new LaPautaSLAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LaPautaSLAudioProcessor();
}
