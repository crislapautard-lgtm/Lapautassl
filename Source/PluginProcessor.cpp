#include "PluginProcessor.h"
#include "PluginEditor.h"
LaPautaSLAudioProcessor::LaPautaSLAudioProcessor() : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)), apvts(*this, nullptr, "Params", createParams()) { }
LaPautaSLAudioProcessor::~LaPautaSLAudioProcessor() {}
juce::AudioProcessorValueTreeState::ParameterLayout LaPautaSLAudioProcessor::createParams() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back(std::make_unique<juce::AudioParameterFloat>("COMP", "Compresion", 0.0f, 100.0f, 50.0f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("MAKEUP", "Makeup", -12.0f, 12.0f, 0.0f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("SHINE", "Brillo", 0.0f, 100.0f, 25.0f));
    return { p.begin(), p.end() };
}
void LaPautaSLAudioProcessor::prepareToPlay(double sr, int) { sampleRate = sr; envelope = 0.0f; }
void LaPautaSLAudioProcessor::releaseResources() {}
bool LaPautaSLAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const { return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo() || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono(); }
void LaPautaSLAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;
    auto comp = apvts.getRawParameterValue("COMP")->load() / 100.0f;
    auto makeup = juce::Decibels::decibelsToGain(apvts.getRawParameterValue("MAKEUP")->load());
    auto shine = apvts.getRawParameterValue("SHINE")->load() / 100.0f;
    for (int
