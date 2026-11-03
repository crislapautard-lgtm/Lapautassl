#pragma once
#include <JuceHeader.h>
class LaPautaSLAudioProcessor : public juce::AudioProcessor
{
public:
    LaPautaSLAudioProcessor();
    ~LaPautaSLAudioProcessor() override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int) override;
    const juce::String getProgramName (int) override;
    void changeProgramName (int, const juce::String&) override;
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;
    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    float getInputLevel() { return inputLevel.load(); }
    float getOutputLevel() { return outputLevel.load(); }
    float getGainReduction() { return gainReduction.load(); }
private:
    std::atomic<float> inputLevel { -60.0f };
    std::atomic<float> outputLevel { -60.0f };
    std::atomic<float> gainReduction { 0.0f };
    float envelope = 0.0f;
    juce::dsp::IIR::Filter<float> hpfL, hpfR;
    double lastSampleRate = 44100.0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LaPautaSLAudioProcessor)
};
