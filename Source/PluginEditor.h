
#pragma once

#include "PluginProcessor.h"

class LaPautaLookAndFeel : public juce::LookAndFeel_V4
{
public:
    LaPautaLookAndFeel();

    void drawRotarySlider(
        juce::Graphics& g,
        int x, int y, int width, int height,
        float sliderPos,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider& slider) override;
};

class LaPautaSLEditor : public juce::AudioProcessorEditor,
                        private juce::Timer
{
public:
    explicit LaPautaSLEditor(LaPautaSLAudioProcessor&);
    ~LaPautaSLEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    void setupKnob(juce::Slider&, const juce::String&);
    void drawScrew(juce::Graphics&, float, float);
    void drawMeter(juce::Graphics&, juce::Rectangle<float>);

    LaPautaSLAudioProcessor& audioProcessor;

    LaPautaLookAndFeel lookAndFeel;

    juce::Slider thresholdSlider;
    juce::Slider ratioSlider;
    juce::Slider attackSlider;
    juce::Slider makeupSlider;
    juce::Slider mixSlider;

    juce::AudioProcessorValueTreeState::SliderAttachment
        thresholdAttachment, ratioAttachment, attackAttachment,
        makeupAttachment, mixAttachment;

    juce::Rectangle<int> knobAreas[5];

    float displayedGR = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LaPautaSLEditor)
};
