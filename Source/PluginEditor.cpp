#include "PluginProcessor.h"
#include "PluginEditor.h"

LaPautaSLAudioProcessorEditor::LaPautaSLAudioProcessorEditor (LaPautaSLAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (980, 560);
    setLookAndFeel(&lnf);

    setup(thresh, lThresh, "THRESHOLD");
    setup(ratio, lRatio, "RATIO");
    setup(attack, lAttack, "ATTACK");
    setup(release, lRelease, "RELEASE");
    setup(makeup, lMakeup, "MAKE UP");
    setup(mix, lMix, "MIX");
    // Sidechain es solo visual, NO toca tu Processor, por eso no tiene Attachment
    setup(sidechain, lSidechain, "HPF");
    sidechain.setValue(120.0);
    sidechain.setRange(20.0, 1000.0);

    // Solo los 6 que YA tienes en tu Processor, nada nuevo
    a1 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "threshold", thresh);
    a2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "ratio", ratio);
    a3 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "attack", attack);
    a4 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "release", release);
    a5 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "makeup", makeup);
    a6 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "mix", mix);

    lTitle.setText("La Pauta  SSL COMPRESSOR  HYBRID EDITION", juce::dontSendNotification);
    lTitle.setFont(juce::Font(30.0f, juce::Font::bold));
    lTitle.setColour(juce::Label::textColourId, juce::Colours::white);
    lTitle.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lTitle);

    lSub.setText("Cris la Pauta RD  |  AUDIO SOLUTIONS", juce::dontSendNotification);
    lSub.setFont(juce::Font(13.0f, juce::Font::bold));
    lSub.setColour(juce::Label::textColourId, juce::Colour(0xffffcc00));
    lSub.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lSub);

    sidechainOn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff00aaff));
    addAndMakeVisible(sidechainOn);

    startTimerHz(30);
}

LaPautaSLAudioProcessorEditor::~LaPautaSLAudioProcessorEditor() { setLookAndFeel(nullptr); }

void LaPautaSLAudioProcessorEditor::setup(juce::Slider& s, juce::Label& l, juce::String t)
{
    s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 22);
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xff00ccff));
    s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff000000));
    addAndMakeVisible(s);
    l.setText(t, juce::dontSendNotification);
    l.setFont(juce::Font(14.0f, juce::Font::bold));
    l.setColour(juce::Label::textColourId, juce::Colours::white);
    l.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(l);
}

void LaPautaSLAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1e1e1e));
    // Header oscuro como la foto
    g.setColour(juce::Colour(0xff0a0a22));
    g.fillRect(0, 0, getWidth(), 75);
    g.setColour(juce::Colour(0xff00aaff));
    g.drawLine(0, 75, getWidth(), 75, 3.0f);

    // VU Izquierdo - INPUT LEVEL
    g.setColour(juce::Colour(0xffffe6b3));
    g.fillRoundedRectangle(175, 95, 280, 105, 4.0f);
    g.setColour(juce::Colours::black);
    g.setFont(juce::Font(13.0f, juce::Font::bold));
    g.drawText("INPUT LEVEL", 175, 170, 280, 20, juce::Justification::centred);
    g.drawText("dB", 295, 145, 40, 20, juce::Justification::centred);
    g.drawText("-20  -10  -7  -5  -3  -1  0  +3", 195, 110, 240, 20, juce::Justification::centred);
    auto c1 = juce::Point<float>(315, 185);
    auto p1 = c1.getPointOnCircumference(75, -1.5708f + 0.3f);
    g.drawLine(juce::Line<float>(c1, p1), 2.5f);

    // VU Derecho - GAIN REDUCTION
    g.setColour(juce::Colour(0xffffe6b3));
    g.fillRoundedRectangle(525, 95, 280, 105, 4.0f);
    g.setColour(juce::Colours::black);
    g.drawText("GAIN REDUCTION", 525, 170, 280, 20, juce::Justification::centred);
    g.drawText("dB", 645, 145, 40, 20, juce::Justification::centred);
    g.drawText("0  1  3  6  9  12  15", 545, 110, 240, 20, juce::Justification::centred);
    auto c2 = juce::Point<float>(665, 185);
    auto p2 = c2.getPointOnCircumference(75, -1.5708f + 0.6f);
    g.drawLine(juce::Line<float>(c2, p2), 2.5f);

    // Medidores laterales INPUT / OUTPUT - solo visual
    g.setColour(juce::Colours::black);
    g.fillRect(25, 95, 70, 105);
    g.setColour(juce::Colours::white);
    g.setFont(11.0f);
    g.drawText("INPUT", 25, 95, 70, 16, juce::Justification::centred);
    g.setColour(juce::Colour(0xff00ff00));
    g.fillRect(33, 130, 54, 55);

    g.setColour(juce::Colours::black);
    g.fillRect(885, 95, 70, 105);
    g.setColour(juce::Colours::white);
    g.drawText("OUTPUT", 885, 95, 70, 16, juce::Justification::centred);
    g.setColour(juce::Colour(0xff00ff00));
    g.fillRect(893, 130, 54, 55);

    g.setColour(juce::Colour(0xffff2222));
    g.drawLine(0, getHeight()-2, getWidth(), getHeight()-2, 3.0f);
}

void LaPautaSLAudioProcessorEditor::resized()
{
    lTitle.setBounds(0, 5, getWidth(), 35);
    lSub.setBounds(0, 38, getWidth(), 20);
    int y = 250, h = 130, w = 125, gap = 10, startX = 25;
    thresh.setBounds(startX, y, w, h); lThresh.setBounds(startX, y-22, w, 20);
    ratio.setBounds(startX + (w+gap)*1, y, w, h); lRatio.setBounds(startX + (w+gap)*1, y-22, w, 20);
    attack.setBounds(startX + (w+gap)*2, y, w, h); lAttack.setBounds(startX + (w+gap)*2, y-22, w, 20);
    release.setBounds(startX + (w+gap)*3, y, w, h); lRelease.setBounds(startX + (w+gap)*3, y-22, w, 20);
    makeup.setBounds(startX + (w+gap)*4, y, w, h); lMakeup.setBounds(startX + (w+gap)*4, y-22, w, 20);
    mix.setBounds(startX + (w+gap)*5, y, w, h); lMix.setBounds(startX + (w+gap)*5, y-22, w, 20);
    // Sidechain visual a la derecha
    sidechainOn.setBounds(865, 250, 90, 30);
    sidechain.setBounds(870, 295, 80, 80);
    lSidechain.setBounds(845, 230, 130, 20);
    lSidechain.setText("SIDECHAIN", juce::dontSendNotification);
}

void LaPautaSLAudioProcessorEditor::timerCallback() { repaint(); }
