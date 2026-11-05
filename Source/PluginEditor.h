
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class LaPautaSLEditor : public juce::AudioProcessorEditor,
                        private juce::Timer
{
public:
    explicit LaPautaSLEditor(LaPautaSLCompresorAudioProcessor& processor);
    ~LaPautaSLEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    LaPautaSLCompresorAudioProcessor& audioProcessor;

    juce::Slider thresholdSlider, ratioSlider, attackSlider,
        releaseSlider, makeupSlider, mixSlider, hpfSlider;

    juce::Label thresholdLabel, ratioLabel, attackLabel,
        releaseLabel, makeupLabel, mixLabel, hpfLabel;

    juce::ComboBox presetBox;
    juce::Label presetLabel;

    std::unique_ptr<Attachment> thresholdAttachment;
    std::unique_ptr<Attachment> ratioAttachment;
    std::unique_ptr<Attachment> attackAttachment;
    std::unique_ptr<Attachment> releaseAttachment;
    std::unique_ptr<Attachment> makeupAttachment;
    std::unique_ptr<Attachment> mixAttachment;
    std::unique_ptr<Attachment> hpfAttachment;

    void configureSlider(juce::Slider&, juce::Label&, const juce::String&);
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LaPautaSLEditor)
};
::SliderAttachment;
