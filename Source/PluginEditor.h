
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class LaPautaSLLookAndFeel : public juce::LookAndFeel_V4
{
public:
    LaPautaSLLookAndFeel();

    void drawRotarySlider(
        juce::Graphics& g,
        int x,
        int y,
        int width,
        int height,
        float sliderPos,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider& slider
    ) override;
};

class LaPautaSLEditor : public juce::AudioProcessorEditor,
                        private juce::Timer
{
public:
    explicit LaPautaSLEditor(
        LaPautaSLCompresorAudioProcessor& processor
    );

    ~LaPautaSLEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    LaPautaSLCompresorAudioProcessor& audioProcessor;

    LaPautaSLLookAndFeel lookAndFeel;

    juce::Slider thresholdSlider;
    juce::Slider ratioSlider;
    juce::Slider releaseSlider;
    juce::Slider makeupSlider;
    juce::Slider mixSlider;
    juce::Slider hpfSlider;

    juce::Label thresholdLabel;
    juce::Label ratioLabel;
    juce::Label releaseLabel;
    juce::Label makeupLabel;
    juce::Label mixLabel;
    juce::Label hpfLabel;

    using SliderAttachment =
        juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<SliderAttachment> thresholdAttachment;
    std::unique_ptr<SliderAttachment> ratioAttachment;
    std::unique_ptr<SliderAttachment> releaseAttachment;
    std::unique_ptr<SliderAttachment> makeupAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;
    std::unique_ptr<SliderAttachment> hpfAttachment;

    void configureSlider(
        juce::Slider& slider,
        juce::Label& label,
        const juce::String& labelText
    );

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LaPautaSLEditor)
};
