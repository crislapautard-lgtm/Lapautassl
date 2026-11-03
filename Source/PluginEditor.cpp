#include "PluginProcessor.h"
#include "PluginEditor.h"

LaPautaSLAudioProcessorEditor::LaPautaSLAudioProcessorEditor(LaPautaSLAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(600, 400);
    background = juce::ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);

    auto setupSlider = [&](juce::Slider& s, juce::Label& l, juce::String text) {
        s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(s);
        l.setText(text, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(l);
    };

    setupSlider(thresholdSlider, thresholdLabel, "Threshold");
    setupSlider(ratioSlider, ratioLabel, "Ratio");
    setupSlider(attackSlider, attackLabel, "Attack");
    setupSlider(releaseSlider, releaseLabel, "Release");
    setupSlider(makeupSlider, makeupLabel, "Makeup");
    setupSlider(mixSlider, mixLabel, "Mix");

    presetLabel.setText("PRESET", juce::dontSendNotification);
    presetLabel.setJustificationType(juce::Justification::centred);
    presetLabel.setColour(juce::Label::textColourId, juce::Colours::yellow);
    addAndMakeVisible(presetLabel);
    for (int i = 0; i < 8; ++i)
        presetBox.addItem(audioProcessor.getProgramName(i), i + 1);
    presetBox.setSelectedId(1, juce::dontSendNotification);
    presetBox.onChange = [this] {
        int idx = presetBox.getSelectedId() - 1;
        if (idx >= 0) audioProcessor.applyPreset(idx);
    };
    addAndMakeVisible(presetBox);

    auto& apvts = audioProcessor.apvts;
    thresholdAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "threshold", thresholdSlider);
    ratioAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ratio", ratioSlider);
    attackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "attack", attackSlider);
    releaseAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "release", releaseSlider);
    makeupAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "makeup", makeupSlider);
    mixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "mix", mixSlider);
}

LaPautaSLAudioProcessorEditor::~LaPautaSLAudioProcessorEditor() {}

void LaPautaSLAudioProcessorEditor::paint(juce::Graphics& g)
{
    if (background.isValid())
        g.drawImage(background, getLocalBounds().toFloat());
    else
        g.fillAll(juce::Colours::darkgrey);
}

void LaPautaSLAudioProcessorEditor::resized()
{
    presetLabel.setBounds(20, 15, 100, 20);
    presetBox.setBounds(130, 15, 250, 25);

    int y = 120;
    int w = 80, h = 90, gap = 10, x = 20;
    auto place = [&](juce::Slider& s, juce::Label& l) {
        s.setBounds(x, y, w, h);
        l.setBounds(x, y + h, w, 20);
        x += w + gap;
    };
    place(thresholdSlider, thresholdLabel);
    place(ratioSlider, ratioLabel);
    place(attackSlider, attackLabel);
    place(releaseSlider, releaseLabel);
    place(makeupSlider, makeupLabel);
    place(mixSlider, mixLabel);
}
