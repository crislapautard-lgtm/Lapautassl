#include "PluginProcessor.h"
#include "PluginEditor.h"

LaPautaSLAudioProcessorEditor::LaPautaSLAudioProcessorEditor (LaPautaSLAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (900, 500);
    
    // Configurar todas las perillas PRO
    auto setupKnob = [&](juce::Slider& s, juce::Label& l, const juce::String& name){
        s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s.setLookAndFeel(&laPautaLNF);
        addAndMakeVisible(s);
        l.setText(name, juce::DontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colours::white);
        l.setFont(juce::Font(11.0f, juce::Font::bold));
        addAndMakeVisible(l);
    };
    
    setupKnob(thresholdSlider, thresholdLabel, "THRESHOLD");
    setupKnob(ratioSlider, ratioLabel, "RATIO");
    setupKnob(attackSlider, attackLabel, "ATTACK");
    setupKnob(releaseSlider, releaseLabel, "RELEASE");
    setupKnob(makeupSlider, makeupLabel, "MAKEUP");
    setupKnob(mixSlider, mixLabel, "MIX");
    
    startTimerHz(30);
}

LaPautaSLAudioProcessorEditor::~LaPautaSLAudioProcessorEditor()
{
    stopTimer();
    for(auto* s : {&thresholdSlider, &ratioSlider, &attackSlider, &releaseSlider, &makeupSlider, &mixSlider})
        s->setLookAndFeel(nullptr);
}

void LaPautaSLAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff0f0f0f));
    
    // Header
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRect(0, 0, getWidth(), 65);
    g.setColour(juce::Colour(0xff333333));
    g.drawLine(0, 65, (float)getWidth(), 65, 2.0f);
    
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(26.0f, juce::Font::bold));
    g.drawText("LA PAUTA SSL COMPRESSOR", 20, 12, 400, 25, juce::Justification::left);
    
    g.setColour(juce::Colour(0xff00aaff));
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.drawText("HYBRID EDITION", 20, 38, 200, 15, juce::Justification::left);
    
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.setFont(juce::Font(10.0f));
    g.drawText("CRIS LA PAUTA RD  ©  2026  AUDIO SOLUTIONS", getWidth()-300, 15, 280, 15, juce::Justification::right);
    
    // Panel central con borde
    g.setColour(juce::Colour(0xff222222));
    g.fillRoundedRectangle(15, 85, getWidth()-30, getHeight()-100, 6.0f);
    g.setColour(juce::Colour(0xff2f2f2f));
    g.drawRoundedRectangle(15, 85, getWidth()-30, getHeight()-100, 6.0f, 1.5f);
}

void LaPautaSLAudioProcessorEditor::resized()
{
    int knobSize = 110;
    int startX = 50;
    int y = 140;
    int gap = 25;
    
    thresholdSlider.setBounds(startX, y, knobSize, knobSize);
    thresholdLabel.setBounds(startX, y+knobSize+5, knobSize, 15);
    
    ratioSlider.setBounds(startX + (knobSize+gap), y, knobSize, knobSize);
    ratioLabel.setBounds(startX + (knobSize+gap), y+knobSize+5, knobSize, 15);
    
    attackSlider.setBounds(startX + (knobSize+gap)*2, y, knobSize, knobSize);
    attackLabel.setBounds(startX + (knobSize+gap)*2, y+knobSize+5, knobSize, 15);
    
    releaseSlider.setBounds(startX + (knobSize+gap)*3, y, knobSize, knobSize);
    releaseLabel.setBounds(startX + (knobSize+gap)*3, y+knobSize+5, knobSize, 15);
    
    makeupSlider.setBounds(startX + (knobSize+gap)*4, y, knobSize, knobSize);
    makeupLabel.setBounds(startX + (knobSize+gap)*4, y+knobSize+5, knobSize, 15);
    
    mixSlider.setBounds(startX + (knobSize+gap)*5, y, knobSize, knobSize);
    mixLabel.setBounds(startX + (knobSize+gap)*5, y+knobSize+5, knobSize, 15);
}

void LaPautaSLAudioProcessorEditor::timerCallback()
{
    repaint();
}
