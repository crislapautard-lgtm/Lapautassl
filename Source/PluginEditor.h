
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class LaPautaSLEditor : public juce::AudioProcessorEditor,
                       private juce::Timer
{
public:
    using Attachment =
        juce::AudioProcessorValueTreeState::SliderAttachment;

    explicit LaPautaSLEditor(
        LaPautaSLCompresorAudioProcessor& processor);

    ~LaPautaSLEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    LaPautaSLCompresorAudioProcessor& audioProcessor;

    juce::Slider thresholdSlider;
    juce::Slider ratioSlider;
    juce::Slider attackSlider;
    juce::Slider releaseSlider;
    juce::Slider makeupSlider;
    juce::Slider mixSlider;
    juce::Slider hpfSlider;

    juce::Label thresholdLabel;
    juce::Label ratioLabel;
    juce::Label attackLabel;
    juce::Label releaseLabel;
    juce::Label makeupLabel;
    juce::Label mixLabel;
    juce::Label hpfLabel;

    juce::ComboBox presetBox;
    juce::Label presetLabel;

    std::unique_ptr<Attachment> thresholdAttachment;
    std::unique_ptr<Attachment> ratioAttachment;
    std::unique_ptr<Attachment> attackAttachment;
    std::unique_ptr<Attachment> releaseAttachment;
    std::unique_ptr<Attachment> makeupAttachment;
    std::unique_ptr<Attachment> mixAttachment;
    std::unique_ptr<Attachment> hpfAttachment;

    void configureSlider(
        juce::Slider& slider,
        juce::Label& label,
        const juce::String& parameterName);

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        LaPautaSLEditor
    )
};
