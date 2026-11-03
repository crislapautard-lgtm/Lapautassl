#include "PluginProcessor.h"
#include "PluginEditor.h"
LaPautaSLAudioProcessor::LaPautaSLAudioProcessor()
    : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout()) {}
LaPautaSLAudioProcessor::~LaPautaSLAudioProcessor() {}
juce::AudioProcessorValueTreeState::ParameterLayout LaPautaSLAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("threshold", "Threshold", -30.0f, 0.0f, -18.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("ratio", "Ratio", 1.0f, 20.0f, 4.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("attack", "Attack", 0.1f, 100.0f, 10.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("release", "Release", 50.0f, 3000.0f, 300.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("makeup", "Makeup", -12.0f, 24.0f, 3.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("mix", "Mix", 0.0f, 100.0f, 70.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("sidechain", "Sidechain HPF", 20.0f, 1000.0f, 120.0f));
    params.push_back (std::make_unique<juce::AudioParameterBool> ("sidechainOn", "Sidechain On", true));
    return { params.begin(), params.end() };
}
void LaPautaSLAudioProcessor::prepareToPlay (double sr, int) { lastSampleRate = sr; envelope = 0.0f; hpfL.reset(); hpfR.reset(); }
void LaPautaSLAudioProcessor::releaseResources() {}
bool LaPautaSLAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* LaPautaSLAudioProcessor::createEditor() { return new LaPautaSLAudioProcessorEditor (*this); }
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
void LaPautaSLAudioProcessor::getStateInformation (juce::MemoryBlock& d) { auto s = apvts.copyState(); std::unique_ptr<juce::XmlElement> x (s.createXml()); copyXmlToBinary (*x, d); }
void LaPautaSLAudioProcessor::setStateInformation (const void* data, int size) { std::unique_ptr<juce::XmlElement> x (getXmlFromBinary (data, size)); if (x) apvts.replaceState (juce::ValueTree::fromXml (*x)); }
void LaPautaSLAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto inCh = getTotalNumInputChannels();
    auto outCh = getTotalNumOutputChannels();
    for (auto i = inCh; i < outCh; ++i) buffer.clear (i, 0, buffer.getNumSamples());
    float thresh = apvts.getRawParameterValue("threshold")->load();
    float ratio = apvts.getRawParameterValue("ratio")->load();
    float attack = apvts.getRawParameterValue("attack")->load();
    float release = apvts.getRawParameterValue("release")->load();
    float makeup = apvts.getRawParameterValue("makeup")->load();
    float mix = apvts.getRawParameterValue("mix")->load() / 100.0f;
    float hpfFreq = apvts.getRawParameterValue("sidechain")->load();
    bool hpfOn = apvts.getRawParameterValue("sidechainOn")->load() > 0.5f;
    float aCoeff = std::exp(-1.0f / (attack * 0.001f * (float)lastSampleRate * 0.3f + 1.0f));
    float rCoeff = std::exp(-1.0f / (release * 0.001f * (float)lastSampleRate * 0.3f + 1.0f));
    float makeupGain = juce::Decibels::decibelsToGain(makeup);
    // Actualizar HPF solo si esta ON
    if (hpfOn) {
        auto coeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate, hpfFreq, 0.7f);
        *hpfL.coefficients = *coeffs; *hpfR.coefficients = *coeffs;
        hpfL.prepare({lastSampleRate, (juce::uint32)buffer.getNumSamples(), 1});
        hpfR.prepare({lastSampleRate, (juce::uint32)buffer.getNumSamples(), 1});
    }
    float maxIn = 0.0f, maxOut = 0.0f;
    for (int ch = 0; ch < inCh; ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int s = 0; s < buffer.getNumSamples(); ++s)
        {
            float in = data[s];
            maxIn = juce::jmax(maxIn, std::abs(in));
            float detector = in;
            if (hpfOn) detector = (ch == 0 ? hpfL.processSample(in) : hpfR.processSample(in));
            float levelDb = juce::Decibels::gainToDecibels(std::abs(detector) + 0.00001f);
            float target = 0.0f;
            if (levelDb > thresh) target = (levelDb - thresh) * (1.0f - 1.0f / ratio);
            float coeff = target > envelope ? aCoeff : rCoeff;
            envelope = coeff * envelope + (1.0f - coeff) * target;
            float comp = juce::Decibels::decibelsToGain(-envelope);
            float wet = in * comp * makeupGain;
            data[s] = in * (1.0f - mix) + wet * mix;
            maxOut = juce::jmax(maxOut, std::abs(data[s]));
        }
    }
    float inDb = juce::Decibels::gainToDecibels(maxIn + 0.00001f);
    float outDb = juce::Decibels::gainToDecibels(maxOut + 0.00001f);
    // Suavizado para que no brinque feo
    inputLevel.store(inputLevel.load() * 0.7f + inDb * 0.3f);
    outputLevel.store(outputLevel.load() * 0.7f + outDb * 0.3f);
    gainReduction.store(envelope);
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LaPautaSLAudioProcessor();
}
