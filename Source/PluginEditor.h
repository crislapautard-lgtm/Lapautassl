
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class LaPautaSLAudioProcessorEditor
    : public juce::AudioProcessorEditor,
      private juce::Timer
{
public:
    explicit LaPautaSLAudioProcessorEditor(LaPautaSLAudioProcessor& processor);
    ~LaPautaSLAudioProcessorEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    LaPautaSLAudioProcessor& audioProcessor;

    class MetalLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawRotarySlider(
            juce::Graphics& g,
            int x, int y, int width, int height,
            float sliderPos,
            float rotaryStartAngle,
            float rotaryEndAngle,
            juce::Slider& slider) override;
    };

    MetalLookAndFeel metalLook;

    juce::Slider thresholdSlider, ratioSlider, attackSlider,
                 makeupSlider, mixSlider;

    juce::Label thresholdValue, ratioValue, attackValue,
                makeupValue, mixValue;

    juce::Label thresholdName, ratioName, attackName,
                makeupName, mixName;

    using SliderAttachment =
        juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<SliderAttachment> thresholdAttachment;
    std::unique_ptr<SliderAttachment> ratioAttachment;
    std::unique_ptr<SliderAttachment> attackAttachment;
    std::unique_ptr<SliderAttachment> makeupAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;

    void setupSlider(juce::Slider& slider);
    void setupLabel(juce::Label& label, const juce::String& text);
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        LaPautaSLAudioProcessorEditor)
};
