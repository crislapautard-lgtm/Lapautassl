
#include "PluginProcessor.h"
#include "PluginEditor.h"

LaPautaSLCompresorAudioProcessor::LaPautaSLCompresorAudioProcessor()
    : AudioProcessor(
        BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    ),
    parameters(
        *this,
        nullptr,
        "PARAMETERS",
        createParameterLayout()
    )
{
}

LaPautaSLCompresorAudioProcessor::~LaPautaSLCompresorAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
LaPautaSLCompresorAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "THRESHOLD",
        "Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
        -18.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "RATIO",
        "Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f),
        4.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "RELEASE",
        "Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f),
        120.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "MAKEUP",
        "Make Up",
        juce::NormalisableRange<float>(-12.0f, 24.0f, 0.1f),
        0.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "MIX",
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "HPF",
        "Sidechain HPF",
        juce::NormalisableRange<float>(20.0f, 500.0f, 1.0f),
        80.0f
    ));

    return layout;
}

const juce::String LaPautaSLCompresorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LaPautaSLCompresorAudioProcessor::acceptsMidi() const
{
    return false;
}

bool LaPautaSLCompresorAudioProcessor::producesMidi() const
{
    return false;
}

bool LaPautaSLCompresorAudioProcessor::isMidiEffect() const
{
    return false;
}

double LaPautaSLCompresorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LaPautaSLCompresorAudioProcessor::getNumPrograms()
{
    return 5;
}

int LaPautaSLCompresorAudioProcessor::getCurrentProgram()
{
    return currentProgram;
}

void LaPautaSLCompresorAudioProcessor::setCurrentProgram(int index)
{
    currentProgram = juce::jlimit(0, 4, index);

    switch (currentProgram)
    {
        case 0:
            parameters.getParameter("THRESHOLD")->setValueNotifyingHost(
                parameters.getParameterRange("THRESHOLD")
                    .convertTo0to1(-18.0f));
            parameters.getParameter("RATIO")->setValueNotifyingHost(
                parameters.getParameterRange("RATIO")
                    .convertTo0to1(3.0f));
            parameters.getParameter("RELEASE")->setValueNotifyingHost(
                parameters.getParameterRange("RELEASE")
                    .convertTo0to1(100.0f));
            parameters.getParameter("MAKEUP")->setValueNotifyingHost(
                parameters.getParameterRange("MAKEUP")
                    .convertTo0to1(2.0f));
            break;

        case 1:
            parameters.getParameter("THRESHOLD")->setValueNotifyingHost(
                parameters.getParameterRange("THRESHOLD")
                    .convertTo0to1(-24.0f));
            parameters.getParameter("RATIO")->setValueNotifyingHost(
                parameters.getParameterRange("RATIO")
                    .convertTo0to1(4.0f));
            parameters.getParameter("RELEASE")->setValueNotifyingHost(
                parameters.getParameterRange("RELEASE")
                    .convertTo0to1(150.0f));
            parameters.getParameter("MAKEUP")->setValueNotifyingHost(
                parameters.getParameterRange("MAKEUP")
                    .convertTo0to1(3.0f));
            break;

        case 2:
            parameters.getParameter("THRESHOLD")->setValueNotifyingHost(
                parameters.getParameterRange("THRESHOLD")
                    .convertTo0to1(-20.0f));
            parameters.getParameter("RATIO")->setValueNotifyingHost(
                parameters.getParameterRange("RATIO")
                    .convertTo0to1(5.0f));
            parameters.getParameter("RELEASE")->setValueNotifyingHost(
                parameters.getParameterRange("RELEASE")
                    .convertTo0to1(90.0f));
            parameters.getParameter("MAKEUP")->setValueNotifyingHost(
                parameters.getParameterRange("MAKEUP")
                    .convertTo0to1(1.0f));
            break;

        case 3:
            parameters.getParameter("THRESHOLD")->setValueNotifyingHost(
                parameters.getParameterRange("THRESHOLD")
                    .convertTo0to1(-15.0f));
            parameters.getParameter("RATIO")->setValueNotifyingHost(
                parameters.getParameterRange("RATIO")
                    .convertTo0to1(2.0f));
            parameters.getParameter("RELEASE")->setValueNotifyingHost(
                parameters.getParameterRange("RELEASE")
                    .convertTo0to1(250.0f));
            parameters.getParameter("MAKEUP")->setValueNotifyingHost(
                parameters.getParameterRange("MAKEUP")
                    .convertTo0to1(1.0f));
            break;

        case 4:
            parameters.getParameter("THRESHOLD")->setValueNotifyingHost(
                parameters.getParameterRange("THRESHOLD")
                    .convertTo0to1(-12.0f));
            parameters.getParameter("RATIO")->setValueNotifyingHost(
                parameters.getParameterRange("RATIO")
                    .convertTo0to1(2.0f));
            parameters.getParameter("RELEASE")->setValueNotifyingHost(
                parameters.getParameterRange("RELEASE")
                    .convertTo0to1(300.0f));
            parameters.getParameter("MAKEUP")->setValueNotifyingHost(
                parameters.getParameterRange("MAKEUP")
                    .convertTo0to1(0.0f));
            break;
    }
}

const juce::String LaPautaSLCompresorAudioProcessor::getProgramName(int index)
{
    const juce::String names[] = {
        "Vocal Control",
        "Drum Bus",
        "Bass Control",
        "Mix Bus",
        "Master Glue"
    };

    if (index < 0 || index >= 5)
        return {};

    return names[index];
}

void LaPautaSLCompresorAudioProcessor::changeProgramName(
    int index,
    const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void LaPautaSLCompresorAudioProcessor::prepareToPlay(
    double sampleRate,
    int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);

    currentSampleRate = sampleRate;

    envelope = 0.0f;
    previousInput[0] = 0.0f;
    previousInput[1] = 0.0f;
    previousHighPass[0] = 0.0f;
    previousHighPass[1] = 0.0f;
}

void LaPautaSLCompresorAudioProcessor::releaseResources()
{
}

bool LaPautaSLCompresorAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    const auto output = layouts.getMainOutputChannelSet();

    if (input != output)
        return false;

    return input == juce::AudioChannelSet::mono()
        || input == juce::AudioChannelSet::stereo();
}

void LaPautaSLCompresorAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numChannels == 0)
        return;

    const float threshold =
        parameters.getRawParameterValue("THRESHOLD")->load();

    const float ratio =
        parameters.getRawParameterValue("RATIO")->load();

    const float releaseMs =
        parameters.getRawParameterValue("RELEASE")->load();

    const float makeupDb =
        parameters.getRawParameterValue("MAKEUP")->load();

    const float mix =
        parameters.getRawParameterValue("MIX")->load() * 0.01f;

    const float hpfHz =
        parameters.getRawParameterValue("HPF")->load();

    const float attackMs = 10.0f;

    const float attackCoefficient =
        std::exp(-1.0f /
        (0.001f * attackMs * (float)currentSampleRate));

    const float releaseCoefficient =
        std::exp(-1.0f /
        (0.001f * releaseMs * (float)currentSampleRate));

    const float hpfCoefficient =
        std::exp(
            -juce::MathConstants<float>::twoPi *
            hpfHz / (float)currentSampleRate
        );

    const float makeupGain =
        juce::Decibels::decibelsToGain(makeupDb);

    const float dryGain = 1.0f - mix;
    const float wetGain = mix;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float detector = 0.0f;

        for (int channel = 0; channel < numChannels; ++channel)
        {
            const float input = buffer.getSample(channel, sample);

            const float highPassed =
                hpfCoefficient *
                (previousHighPass[channel] + input -
                 previousInput[channel]);

            previousInput[channel] = input;
            previousHighPass[channel] = highPassed;

            detector = juce::jmax(
                detector,
                std::abs(highPassed)
            );
        }

        const float coefficient =
            detector > envelope
                ? attackCoefficient
                : releaseCoefficient;

        envelope =
            coefficient * envelope +
            (1.0f - coefficient) * detector;

        const float detectorDb =
            juce::Decibels::gainToDecibels(
                juce::jmax(envelope, 0.000001f)
            );

        float gainReductionDb = 0.0f;

        if (detectorDb > threshold)
        {
            const float compressedDb =
                threshold +
                (detectorDb - threshold) / ratio;

            gainReductionDb = compressedDb - detectorDb;
        }

        const float compressorGain =
            juce::Decibels::decibelsToGain(gainReductionDb);

        for (int channel = 0; channel < numChannels; ++channel)
        {
            const float dry = buffer.getSample(channel, sample);

            const float wet =
                dry * compressorGain * makeupGain;

            const float output =
                dry * dryGain + wet * wetGain;

            buffer.setSample(channel, sample, output);
        }
    }
}

bool LaPautaSLCompresorAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor*
LaPautaSLCompresorAudioProcessor::createEditor()
{
    return new LaPautaSLEditor(*this);
}

void LaPautaSLCompresorAudioProcessor::getStateInformation(
    juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();

    std::unique_ptr<juce::XmlElement> xml(
        state.createXml()
    );

    copyXmlToBinary(*xml, destData);
}

void LaPautaSLCompresorAudioProcessor::setStateInformation(
    const void* data,
    int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(
        getXmlFromBinary(data, sizeInBytes)
    );

    if (xml != nullptr &&
        xml->hasTagName(parameters.state.getType()))
    {
        parameters.replaceState(
            juce::ValueTree::fromXml(*xml)
        );
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LaPautaSLCompresorAudioProcessor();
}
