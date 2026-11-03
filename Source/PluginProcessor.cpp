#include <JuceHeader.h>

class LaPautaProcessor : public juce::AudioProcessor {
public:
    LaPautaProcessor() : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo()).withOutput("Output", juce::AudioChannelSet::stereo())) {
        addParameter(gain = new juce::AudioParameterFloat("gain", "Input", 0.0f, 2.0f, 1.0f));
        addParameter(punch = new juce::AudioParameterFloat("punch", "Pauta", 0.0f, 1.0f, 0.5f));
    }
    void prepareToPlay(double, int) override {}
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override { return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet(); }
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i) {
                float x = data[i] * gain->get();
                x = std::tanh(x * (1.0f + punch->get() * 3.0f));
                data[i] = x;
            }
        }
    }
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }
    const juce::String getName() const override { return "LaPautaSL"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}
private:
    juce::AudioParameterFloat* gain;
    juce::AudioParameterFloat* punch;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LaPautaProcessor)
};

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new LaPautaProcessor(); }
