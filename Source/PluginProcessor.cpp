
#include "PluginProcessor.h"
#include "PluginEditor.h"

LaPautaSLAudioProcessor::LaPautaSLAudioProcessor()
    : AudioProcessor(
        BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
LaPautaSLAudioProcessor::createParameterLayout()
{
    using Float = juce::AudioParameterFloat;

    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<Float>(
        "THRESHOLD", "Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
        -12.0f));

    layout.add(std::make_unique<Float>(
        "RATIO", "Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f),
        4.0f));

    layout.add(std::make_unique<Float>(
        "ATTACK", "Attack",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f),
        10.0f));

    layout.add(std::make_unique<Float>(
        "MAKEUP", "Makeup",
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f),
        3.0f));

    layout.add(std::make_unique<Float>(
        "MIX", "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f));

    return layout;
}

void LaPautaSLAudioProcessor::prepareToPlay(
    double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);

    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;

    envelope = 0.0f;
    smoothedGainDb = 0.0f;
    gainReductionDb.store(0.0f);
}

void LaPautaSLAudioProcessor::releaseResources()
{
    envelope = 0.0f;
    smoothedGainDb = 0.0f;
    gainReductionDb.store(0.0f);
}

bool LaPautaSLAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    const auto output = layouts.getMainOutputChannelSet();

    if (input != output)
        return false;

    return input == juce::AudioChannelSet::mono()
        || input == juce::AudioChannelSet::stereo();
}

void LaPautaSLAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    const int channels = getTotalNumInputChannels();
    const int samples = buffer.getNumSamples();

    for (int ch = channels; ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear(ch, 0, samples);

    const float threshold =
        parameters.getRawParameterValue("THRESHOLD")->load();

    const float ratio =
        parameters.getRawParameterValue("RATIO")->load();

    const float attackMs =
        parameters.getRawParameterValue("ATTACK")->load();

    const float makeupDb =
        parameters.getRawParameterValue("MAKEUP")->load();

    const float mix =
        parameters.getRawParameterValue("MIX")->load() / 100.0f;

    constexpr float releaseMs = 200.0f;

    const float attackCoeff = std::exp(
        -1.0f / static_cast<float>(
            currentSampleRate * attackMs * 0.001));

    const float releaseCoeff = std::exp(
        -1.0f / static_cast<float>(
            currentSampleRate * releaseMs * 0.001));

    const float makeupLinear = juce::Decibels::decibelsToGain(makeupDb);

    float maximumGR = 0.0f;

    for (int i = 0; i < samples; ++i)
    {
        float left = buffer.getSample(0, i);
        float right = channels > 1 ? buffer.getSample(1, i) : left;

        const float dryLeft = left;
        const float dryRight = right;

        // Detector estéreo enlazado.
        const float detector = juce::jmax(
            std::abs(left), std::abs(right));

        const float detectorCoeff =
            detector > envelope ? attackCoeff : releaseCoeff;

        envelope = detectorCoeff * envelope
                 + (1.0f - detectorCoeff) * detector;

        const float envelopeDb = juce::Decibels::gainToDecibels(
            juce::jmax(envelope, 0.000001f));

        float targetGainDb = 0.0f;

        if (envelopeDb > threshold)
        {
            const float compressedDb =
                threshold + (envelopeDb - threshold) / ratio;

            targetGainDb = compressedDb - envelopeDb;
        }

        const float gainCoeff =
            targetGainDb < smoothedGainDb
                ? attackCoeff : releaseCoeff;

        smoothedGainDb =
            gainCoeff * smoothedGainDb
            + (1.0f - gainCoeff) * targetGainDb;

        const float gr = juce::jmax(0.0f, -smoothedGainDb);

        maximumGR = juce::jmax(maximumGR, gr);

        const float compressedGain =
            juce::Decibels::decibelsToGain(smoothedGainDb)
            * makeupLinear;

        const float wetLeft = dryLeft * compressedGain;
        const float wetRight = dryRight * compressedGain;

        buffer.setSample(0, i,
            dryLeft * (1.0f - mix) + wetLeft * mix);

        if (channels > 1)
        {
            buffer.setSample(1, i,
                dryRight * (1.0f - mix) + wetRight * mix);
        }
    }

    gainReductionDb.store(maximumGR);
}

void LaPautaSLAudioProcessor::getStateInformation(
    juce::MemoryBlock& destData)
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
        parameters.replaceState(
            juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor*
LaPautaSLAudioProcessor::createEditor()
{
    return new LaPautaSLEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LaPautaSLAudioProcessor();
}
