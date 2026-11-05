
#pragma once

#include <JuceHeader.h>

class LaPautaSLCompresorAudioProcessor
    : public juce::AudioProcessor
{
public:
    LaPautaSLCompresorAudioProcessor();
    ~LaPautaSLCompresorAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(
        juce::AudioBuffer<float>& buffer,
        juce::MidiBuffer& midiMessages
    ) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;

    const juce::String getProgramName(int index) override;

    void changeProgramName(
        int index,
        const juce::String& newName
    ) override;

    void getStateInformation(juce::MemoryBlock& destData) override;

    void setStateInformation(
        const void* data,
        int sizeInBytes
    ) override;

    juce::AudioProcessorValueTreeState parameters;

    static juce::AudioProcessorValueTreeState::ParameterLayout
    createParameterLayout();

private:
    double currentSampleRate = 44100.0;

    float envelope = 0.0f;

    float previousInput[2] = { 0.0f, 0.0f };
    float previousHighPass[2] = { 0.0f, 0.0f };

    int currentProgram = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        LaPautaSLCompresorAudioProcessor
    )
};
