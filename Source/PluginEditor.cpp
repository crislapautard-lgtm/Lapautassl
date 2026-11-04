#include "PluginProcessor.h"
#include "PluginEditor.h"

LaPautaSLAudioProcessorEditor::LaPautaSLAudioProcessorEditor (LaPautaSLAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (900, 540);
    
    auto setup = [&](juce::Slider& s, juce::Label& lab, juce::Label& val, const juce::String& name){
        s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s.setLookAndFeel(&laPautaLNF);
        addAndMakeVisible(s);
        lab.setText(name, juce::NotificationType::dontSendNotification);
        lab.setJustificationType(juce::Justification::centred);
        lab.setColour(juce::Label::textColourId, juce::Colours::white);
        lab.setFont(juce::Font(11.0f, juce::Font::bold));
        addAndMakeVisible(lab);
        val.setJustificationType(juce::Justification::centred);
        val.setColour(juce::Label::textColourId, juce::Colour(0xff00aaff));
        val.setFont(juce::Font(11.0f, juce::Font::bold));
        val.setText("-12 dB", juce::NotificationType::dontSendNotification);
        addAndMakeVisible(val);
    };
    
    setup(thresholdSlider, thresholdLabel, thresholdVal, "THRESHOLD");
    setup(ratioSlider, ratioLabel, ratioVal, "RATIO");
    setup(attackSlider, attackLabel, attackVal, "ATTACK");
    setup(releaseSlider, releaseLabel, releaseVal, "RELEASE");
    setup(makeupSlider, makeupLabel, makeupVal, "MAKEUP");
    setup(mixSlider, mixLabel, mixVal, "MIX");

    // Attachments a tu APVTS
    auto& apvts = audioProcessor.apvts;
    attThreshold.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, "threshold", thresholdSlider));
    attRatio.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, "ratio", ratioSlider));
    attAttack.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, "attack", attackSlider));
    attRelease.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, "release", releaseSlider));
    attMakeup.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, "makeup", makeupSlider));
    attMix.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, "mix", mixSlider));

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
    g.drawText("CRIS LA PAUTA RD 2026 AUDIO SOLUTIONS", getWidth()-300, 15, 280, 15, juce::Justification::right);
    
    g.setColour(juce::Colour(0xff222222));
    g.fillRoundedRectangle(15, 85, getWidth()-30, getHeight()-100, 6.0f);

    // VU Backgrounds
    g.setColour(juce::Colours::black);
    g.fillRect(40, 400, getWidth()-250, 18);
    g.fillRect(40, 425, getWidth()-250, 18);
    
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("GAIN REDUCTION", 40, 402, 100, 14, juce::Justification::left);
    g.drawText("OUTPUT", 40, 427, 100, 14, juce::Justification::left);

    // VU Bars
    float grW = juce::jlimit(0.0f, 1.0f, grLevel) * (getWidth()-370);
    g.setColour(juce::Colour(0xffff3333));
    g.fillRect(150, 402, (int)grW, 14);
    
    float outW = juce::jlimit(0.0f, 1.0f, outLevel) * (getWidth()-370);
    g.setColour(juce::Colour(0xff00ff88));
    g.fillRect(150, 427, (int)outW, 14);
}

void LaPautaSLAudioProcessorEditor::resized()
{
    int knob = 100; int gap = 30; int x = 45; int y = 120;
    auto place = [&](juce::Slider& s, juce::Label& lab, juce::Label& val, int i){
        int px = x + i*(knob+gap);
        val.setBounds(px, y-5, knob, 15);
        s.setBounds(px, y+12, knob, knob);
        lab.setBounds(px, y+12+knob+5, knob, 15);
    };
    place(thresholdSlider, thresholdLabel, thresholdVal, 0);
    place(ratioSlider, ratioLabel, ratioVal, 1);
    place(attackSlider, attackLabel, attackVal, 2);
    place(releaseSlider, releaseLabel, releaseVal, 3);
    place(makeupSlider, makeupLabel, makeupVal, 4);
    place(mixSlider, mixLabel, mixVal, 5);
}

void LaPautaSLAudioProcessorEditor::timerCallback()
{
    thresholdVal.setText(juce::String(thresholdSlider.getValue(), 1) + " dB", juce::NotificationType::dontSendNotification);
    ratioVal.setText(juce::String(ratioSlider.getValue(), 1) + ":1", juce::NotificationType::dontSendNotification);
    attackVal.setText(juce::String(attackSlider.getValue(), 1) + " ms", juce::NotificationType::dontSendNotification);
    releaseVal.setText(juce::String(releaseSlider.getValue(), 1) + " ms", juce::NotificationType::dontSendNotification);
    makeupVal.setText(juce::String(makeupSlider.getValue(), 1) + " dB", juce::NotificationType::dontSendNotification);
    mixVal.setText(juce::String((int)mixSlider.getValue()) + " %", juce::NotificationType::dontSendNotification);
    
    // Intenta leer niveles reales si tu processor los tiene
    grLevel = audioProcessor.getGainReduction() / -40.0f;
    outLevel = audioProcessor.getOutputLevel();
    repaint();
}
