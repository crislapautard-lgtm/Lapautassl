#include "PluginProcessor.h"
#include "PluginEditor.h"

LaPautaSLAudioProcessor::LaPautaSLAudioProcessor()
    : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout()) {}
LaPautaSLAudioProcessor::~LaPautaSLAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout LaPautaSLAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("threshold", "Threshold", juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -12.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ratio", "Ratio", juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f), 4.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("attack", "Attack", juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f), 10.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("makeup", "Makeup", juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 3.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f));
    return { params.begin(), params.end() };
}

const juce::String LaPautaSLAudioProcessor::getName() const { return "LaPautaSL"; }
bool LaPautaSLAudioProcessor::acceptsMidi() const { return false; }
bool LaPautaSLAudioProcessor::producesMidi() const { return false; }
bool LaPautaSLAudioProcessor::isMidiEffect() const { return false; }
double LaPautaSLAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int LaPautaSLAudioProcessor::getNumPrograms() { return 1; }
int LaPautaSLAudioProcessor::getCurrentProgram() { return 0; }
void LaPautaSLAudioProcessor::setCurrentProgram (int) {}
const juce::String LaPautaSLAudioProcessor::getProgramName (int) { return {}; }
void LaPautaSLAudioProcessor::changeProgramName (int, const juce::String&) {}
void LaPautaSLAudioProcessor::prepareToPlay (double, int) { envelope = 0.0f; currentGR = 0.0f; }
void LaPautaSLAudioProcessor::releaseResources() {}
bool LaPautaSLAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* LaPautaSLAudioProcessor::createEditor() { return new LaPautaSLAudioProcessorEditor (*this); }
void LaPautaSLAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}
void LaPautaSLAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

void LaPautaSLAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float threshold = apvts.getRawParameterValue("threshold")->load();
    float ratio = apvts.getRawParameterValue("ratio")->load();
    float attack = apvts.getRawParameterValue("attack")->load();
    float makeup = apvts.getRawParameterValue("makeup")->load();
    float mix = apvts.getRawParameterValue("mix")->load() / 100.0f;
    float release = 200.0f; // fijo, como en la imagen no hay perilla de Release

    float sampleRate = (float)getSampleRate();
    float attackCoeff = std::exp(-1.0f / (attack * 0.001f * sampleRate));
    float releaseCoeff = std::exp(-1.0f / (release * 0.001f * sampleRate));
    float makeupGain = juce::Decibels::decibelsToGain(makeup);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* data = buffer.getWritePointer(channel);
        for (int s = 0; s < buffer.getNumSamples(); ++s)
        {
            float in = data[s];
            float absIn = std::abs(in);
            float coeff = absIn > envelope ? attackCoeff : releaseCoeff;
            envelope = coeff * envelope + (1.0f - coeff) * absIn;
            float envDb = juce::Decibels::gainToDecibels(envelope + 0.00001f);
            float grDb = 0.0f;
            if (envDb > threshold)
            {
                float over = envDb - threshold;
                grDb = -over * (1.0f - 1.0f / ratio);
            }
            currentGR = 0.9f * currentGR + 0.1f * grDb;
            float gain = juce::Decibels::decibelsToGain(grDb) * makeupGain;
            float compressed = in * gain;
            data[s] = in * (1.0f - mix) + compressed * mix;
        }
    }
    gainReductionDb.store(currentGR);
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new LaPautaSLAudioProcessor(); }
