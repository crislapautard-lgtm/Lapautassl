#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class LaPautaSLAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    LaPautaSLAudioProcessorEditor (LaPautaSLAudioProcessor&);
    ~LaPautaSLAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    LaPautaSLAudioProcessor& audioProcessor;

    juce::ComboBox presetBox;
    juce::Label presetLabel;

    juce::Slider thresholdSlider, ratioSlider, attackSlider, releaseSlider, makeupSlider, mixSlider;
    juce::Label thresholdLabel, ratioLabel, attackLabel, releaseLabel, makeupLabel, mixLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttach, ratioAttach, attackAttach, releaseAttach, makeupAttach, mixAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LaPautaSLAudioProcessorEditor)
};
