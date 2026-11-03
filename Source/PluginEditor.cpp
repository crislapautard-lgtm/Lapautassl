#include "PluginProcessor.h"
#include "PluginEditor.h"
LaPautaSLAudioProcessorEditor::LaPautaSLAudioProcessorEditor(LaPautaSLAudioProcessor& p) : AudioProcessorEditor(&p), audioProcessor(p) {
    setSize(500, 300);
    compLabel.setText("COMPRESION", juce::dontSendNotification); addAndMakeVisible(compLabel);
    makeupLabel.setText("MAKEUP", juce::dontSendNotification); addAndMakeVisible(makeupLabel);
    shineLabel.setText("BRILLO SL", juce::dontSendNotification); addAndMakeVisible(shineLabel);
    compSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag); compSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0); addAndMakeVisible(compSlider);
    makeupSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag); makeupSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0); addAndMakeVisible(makeupSlider);
    shineSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag); shineSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0); addAndMakeVisible(shineSlider);
    compAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "COMP", compSlider);
    makeupAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "MAKEUP", makeupSlider);
    shineAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SHINE", shineSlider);
}
LaPautaSLAudioProcessorEditor::~LaPautaSLAudioProcessorEditor() {}
void LaPautaSLAudioProcessorEditor::paint(juce::Graphics& g) { g.fillAll(juce::Colour(0xff111111)); g.setColour(juce::Colours::white); g.setFont(22.0f); g.drawFittedText("LA PAUTA SL - COMPRESSOR", getLocalBounds().removeFromTop(40), juce::Justification::centred, 1); }
void LaPautaSLAudioProcessorEditor::resized() {
    auto area = getLocalBounds(); area.removeFromTop(50);
    auto w = area.getWidth() / 3;
    compLabel.setBounds(w*0, area.getY(), w, 20); compLabel.setJustificationType(juce::Justification::centred);
    makeupLabel.setBounds(w*1, area.getY(), w, 20); makeupLabel.setJustificationType(juce::Justification::centred);
    shineLabel.setBounds(w*2, area.getY(), w, 20); shineLabel.setJustificationType(juce::Justification::centred);
    compSlider.setBounds(w*0, area.getY()+20, w, area.getHeight()-20);
    makeupSlider.setBounds(w*1, area.getY()+20, w, area.getHeight()-20);
    shineSlider.setBounds(w*2, area.getY()+20, w, area.getHeight()-20);
}
