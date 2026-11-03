#include "PluginProcessor.h"
#include "PluginEditor.h"

LaPautaSLAudioProcessorEditor::LaPautaSLAudioProcessorEditor (LaPautaSLAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (600, 380);

    presetLabel.setText ("Preset", juce::dontSendNotification);
    addAndMakeVisible (presetLabel);
    addAndMakeVisible (presetBox);
    presetBox.addItem ("Limpio", 1);
    presetBox.addItem ("Vocal Presente", 2);
    presetBox.addItem ("Dembow Fuerte", 3);
    presetBox.addItem ("Radio", 4);
    presetBox.setSelectedId (1);
    presetBox.onChange = [this]
    {
        auto id = presetBox.getSelectedId();
        auto& apvts = audioProcessor.apvts;
        if (id == 1) {
            apvts.getParameter ("threshold")->setValueNotifyingHost (0.5f);
            apvts.getParameter ("ratio")->setValueNotifyingHost (0.2f);
            apvts.getParameter ("mix")->setValueNotifyingHost (0.5f);
        } else if (id == 2) {
            apvts.getParameter ("threshold")->setValueNotifyingHost (0.35f);
            apvts.getParameter ("ratio")->setValueNotifyingHost (0.45f);
            apvts.getParameter ("mix")->setValueNotifyingHost (0.7f);
        } else if (id == 3) {
            apvts.getParameter ("threshold")->setValueNotifyingHost (0.25f);
            apvts.getParameter ("ratio")->setValueNotifyingHost (0.65f);
            apvts.getParameter ("mix")->setValueNotifyingHost (0.8f);
        } else if (id == 4) {
            apvts.getParameter ("threshold")->setValueNotifyingHost (0.4f);
            apvts.getParameter ("ratio")->setValueNotifyingHost (0.55f);
            apvts.getParameter ("mix")->setValueNotifyingHost (0.6f);
        }
    };

    auto setupSlider = [this] (juce::Slider& s, juce::Label& l, juce::String text)
    {
        s.setSliderStyle (juce::Slider::RotaryVerticalDrag);
        s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 20);
        addAndMakeVisible (s);
        l.setText (text, juce::dontSendNotification);
        l.setJustificationType (juce::Justification::centred);
        addAndMakeVisible (l);
    };

    setupSlider (thresholdSlider, thresholdLabel, "Threshold");
    setupSlider (ratioSlider, ratioLabel, "Ratio");
    setupSlider (attackSlider, attackLabel, "Attack");
    setupSlider (releaseSlider, releaseLabel, "Release");
    setupSlider (makeupSlider, makeupLabel, "Makeup");
    setupSlider (mixSlider, mixLabel, "Mix");

    auto& apvts = audioProcessor.apvts;
    thresholdAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "threshold", thresholdSlider);
    ratioAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "ratio", ratioSlider);
    attackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "attack", attackSlider);
    releaseAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "release", releaseSlider);
    makeupAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "makeup", makeupSlider);
    mixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "mix", mixSlider);
}

LaPautaSLAudioProcessorEditor::~LaPautaSLAudioProcessorEditor()
{
}

void LaPautaSLAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff111111));
    g.setColour (juce::Colours::white);
    g.setFont (22.0f);
    g.drawText ("LA PAUTA SL", getLocalBounds().removeFromTop (40), juce::Justification::centred, true);
}

void LaPautaSLAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop (40);
    auto top = area.removeFromTop (40);
    presetLabel.setBounds (top.removeFromLeft (80));
    presetBox.setBounds (top.removeFromLeft (200));

    auto row = area.removeFromTop (280);
    auto w = row.getWidth() / 6;
    thresholdSlider.setBounds (row.removeFromLeft (w).reduced (5));
    ratioSlider.setBounds (row.removeFromLeft (w).reduced (5));
    attackSlider.setBounds (row.removeFromLeft (w).reduced (5));
    releaseSlider.setBounds (row.removeFromLeft (w).reduced (5));
    makeupSlider.setBounds (row.removeFromLeft (w).reduced (5));
    mixSlider.setBounds (row.removeFromLeft (w).reduced (5));
}
