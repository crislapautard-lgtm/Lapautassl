#pragma once
#include <JuceHeader.h>
class LaPautaSLAudioProcessor : public juce::AudioProcessor
{
public:
    LaPautaSLAudioProcessor();
    ~LaPautaSLAudioProcessor() override;
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "LaPautaSL"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}
    juce::AudioProcessorValueTreeState apvts;
private:
    juce::dsp::Compressor<float> compressor;
    juce::dsp::Gain<float> makeupGain;
    juce::dsp::IIR::Filter<float> shineFilter;
    juce::dsp::ProcessSpec spec;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LaPautaSLAudioProcessor)
};
