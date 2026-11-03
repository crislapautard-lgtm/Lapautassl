#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
class LaPautaSLAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    LaPautaSLAudioProcessorEditor(LaPautaSLAudioProcessor&);
    ~LaPautaSLAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    LaPautaSLAudioProcessor& audioProcessor;
    juce::Slider compSlider, makeupSlider, shineSlider;
    juce::Label compLabel, makeupLabel, shineLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compAttach, makeupAttach, shineAttach;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LaPautaSLAudioProcessorEditor)
};
