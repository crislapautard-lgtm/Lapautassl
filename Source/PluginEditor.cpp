#include "PluginProcessor.h"
#include "PluginEditor.h"

LaPautaSLEditor::LaPautaSLEditor (LaPautaSLAudioProcessor& p) : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (1000, 620);
    setupKnob (thr, lThr, "THRESHOLD", "THRESHOLD", aThr);
    setupKnob (ratio, lRatio, "RATIO", "RATIO", aRatio);
    setupKnob (att, lAtt, "ATTACK", "ATTACK", aAtt);
    setupKnob (rel, lRel, "RELEASE", "RELEASE", aRel);
    setupKnob (makeup, lMakeup, "MAKE UP", "MAKEUP", aMakeup);
    setupKnob (mix, lMix, "MIX", "MIX", aMix);
    setupKnob (sat, lSat, "SATURATION", "SATURATION", aSat);
    setupKnob (punch, lPunch, "PUNCH", "PUNCH", aPunch);
    setupKnob (parallel, lParallel, "PARALLEL MIX", "PARALLEL", aParallel);
    setupKnob (link, lLink, "STEREO LINK", "LINK", aLink);
    setupKnob (hpf, lHpf, "SIDECHAIN HPF", "HPF", aHpf);
    setupKnob (air, lAir, "AIR", "AIR", aAir);
    addAndMakeVisible (sidechainBtn); sidechainBtn.setButtonText ("ON");
    addAndMakeVisible (modeClassic); modeClassic.setButtonText ("Classic");
    addAndMakeVisible (modePunchy); modePunchy.setButtonText ("Punchy");
    addAndMakeVisible (modeSmooth); modeSmooth.setButtonText ("Smooth");
    addAndMakeVisible (over2x); over2x.setButtonText ("2x");
    addAndMakeVisible (over4x); over4x.setButtonText ("4x");
    addAndMakeVisible (over8x); over8x.setButtonText ("8x");

    // PRESETS SSL NORMALES
    addAndMakeVisible (presetLabel);
    presetLabel.setText ("PRESET:", juce::dontSendNotification);
    presetLabel.setColour (juce::Label::textColourId, juce::Colour (0xffffcc00));
    presetLabel.setFont (juce::Font (14.0f, juce::Font::bold));
    presetLabel.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (presetBox);
    presetBox.addItem ("Mix Bus Glue", 1);
    presetBox.addItem ("Drum Bus Punch", 2);
    presetBox.addItem ("Vocal Leveler", 3);
    presetBox.addItem ("Acoustic Guitar", 4);
    presetBox.addItem ("Piano Smooth", 5);
    presetBox.addItem ("Bass Tight", 6);
    presetBox.addItem ("Master Gentle", 7);
    presetBox.setSelectedId (1);
    presetBox.onChange = [this] { audioProcessor.applyPreset (presetBox.getSelectedId() - 1); };
    startTimerHz (30);
}
LaPautaSLEditor::~LaPautaSLEditor() { stopTimer(); }
void LaPautaSLEditor::timerCallback() { repaint(); }

void LaPautaSLEditor::setupKnob (juce::Slider& s, juce::Label& l, const juce::String& name, const juce::String& id, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& a)
{
    s.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 85, 22);
    s.setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::transparentBlack);
    s.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colours::transparentBlack);
    s.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    s.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff00ccff));
    s.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff001122));
    s.setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0xff00ccff));
    addAndMakeVisible (s);
    l.setText (name, juce::dontSendNotification);
    l.setJustificationType (juce::Justification::centred);
    l.setColour (juce::Label::textColourId, juce::Colours::white);
    l.setFont (juce::Font (13.0f, juce::Font::bold));
    addAndMakeVisible (l);
    a = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, id, s);
}

void LaPautaSLEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff111111));
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff3d3d3d), 0, 0, juce::Colour (0xff0a0a0a), 0, (float) getHeight(), false));
    g.fillAll();
    g.setColour (juce::Colour (0xff00ccff));
    g.drawRect (getLocalBounds(), 2);

    // HEADER - TU IMAGEN: La Pauta + corona + SSL + Cris la Pauta RD + bandera
    g.setFont (juce::Font (36.0f, juce::Font::bold));
    g.setColour (juce::Colours::white);
    g.drawText ("La Pauta", 25, 8, 220, 38, juce::Justification::left, true);
    g.setFont (juce::Font (14.0f, juce::Font::bold));
    g.setColour (juce::Colour (0xffffd700));
    g.drawText ("👑", 95, 0, 30, 20, juce::Justification::centred, true);
    g.setFont (juce::Font (11.0f, juce::Font::bold));
    g.setColour (juce::Colours::white);
    g.drawText ("AUDIO SOLUTIONS", 40, 44, 180, 12, juce::Justification::left, true);
    g.setFont (juce::Font (44.0f, juce::Font::bold));
    g.setColour (juce::Colour (0xffd0d0d0));
    g.drawText ("SSL", 270, 5, 130, 50, juce::Justification::centred, true);
    g.setFont (juce::Font (27.0f, juce::Font::bold));
    g.setColour (juce::Colours::white);
    g.drawText ("COMPRESSOR", 400, 8, 280, 30, juce::Justification::left, true);
    g.setFont (juce::Font (17.0f, juce::Font::bold));
    g.setColour (juce::Colour (0xffffcc00));
    g.drawText ("HYBRID EDITION", 410, 36, 280, 20, juce::Justification::left, true);
    g.setFont (juce::Font (20.0f, juce::Font::bold));
    g.setColour (juce::Colours::white);
    g.drawText ("Cris la Pauta RD", 730, 12, 180, 25, juce::Justification::right, true);
    g.setColour (juce::Colours::white); g.fillRect (925, 5, 55, 36);
    g.setColour (juce::Colours::red); g.fillRect (925, 5, 27, 18); g.fillRect (953, 23, 27, 18);
    g.setColour (juce::Colours::blue); g.fillRect (953, 5, 27, 18); g.fillRect (925, 23, 27, 18);
    g.setColour (juce::Colour (0xffff0000)); g.drawLine (20, 62, 980, 62, 3.0f);
    g.setColour (juce::Colour (0xff00ccff)); g.drawLine (20, 66, 980, 66, 1.0f);

    float inDb = audioProcessor.inputLevel.load();
    float grDb = audioProcessor.gainReduction.load();
    drawVUMeter (g, 180, 78, 300, 115, inDb, false);
    drawVUMeter (g, 520, 78, 300, 115, grDb, true);
    g.setFont (juce::Font (11.0f, juce::Font::bold));
    g.setColour (juce::Colours::white);
    g.drawText ("INPUT LEVEL", 180, 195, 300, 15, juce::Justification::centred, true);
    g.drawText ("GAIN REDUCTION", 520, 195, 300, 15, juce::Justification::centred, true);

    g.setColour (juce::Colour (0xff1a2a3a));
    g.fillRoundedRectangle (20, 78, 140, 135, 8);
    g.fillRoundedRectangle (840, 78, 140, 135, 8);
    g.setColour (juce::Colours::white); g.setFont (juce::Font (13.0f, juce::Font::bold));
    g.drawText ("INPUT", 20, 82, 140, 20, juce::Justification::centred, true);
    g.drawText ("OUTPUT", 840, 82, 140, 20, juce::Justification::centred, true);
    drawLEDBar (g, 45, 105, 35, 80, inDb);
    drawLEDBar (g, 920, 105, 35, 80, audioProcessor.outputLevel.load());
    g.setFont (juce::Font (11.0f, juce::Font::bold)); g.setColour (juce::Colour (0xff00ccff));
    g.drawText ("0.0 dB", 20, 185, 140, 20, juce::Justification::centred, true);
    g.drawText ("0.0 dB", 840, 185, 140, 20, juce::Justification::centred, true);

    g.setFont (juce::Font (24.0f, juce::Font::bold));
    g.setColour (juce::Colours::white);
    g.drawText ("El Sonido que Marca la Diferencia", 0, 555, 1000, 30, juce::Justification::centred, true);
    g.setFont (juce::Font (12.0f, juce::Font::bold));
    g.setColour (juce::Colour (0xffffcc00));
    g.drawText ("SIDECHAIN", 840, 228, 140, 15, juce::Justification::centred, true);
    g.drawText ("MODE", 840, 470, 140, 15, juce::Justification::centred, true);
    g.setColour (juce::Colour (0xff00ccff));
    g.drawText ("OVERSAMPLING  4x ON", 840, 440, 140, 20, juce::Justification::centred, true);
}

void LaPautaSLEditor::drawVUMeter (juce::Graphics& g, int x, int y, int w, int h, float db, bool isGR)
{
    g.setColour (juce::Colour (0xff000000)); g.fillRoundedRectangle (x, y, w, h, 6);
    g.setColour (juce::Colour (0xffffe8b0)); g.fillRoundedRectangle (x+8, y+8, w-16, h-16, 4);
    g.setColour (juce::Colours::black); g.setFont (juce::Font (11.0f, juce::Font::bold));
    g.drawText ("dB", x + w/2 - 10, y + h - 35, 20, 20, juce::Justification::centred, true);
    float norm = 0.5f;
    if (!isGR) norm = juce::jmap (juce::jlimit (-20.0f, 3.0f, db), -20.0f, 3.0f, 0.0f, 1.0f);
    else norm = juce::jmap (juce::jlimit (0.0f, 15.0f, db), 0.0f, 15.0f, 0.0f, 1.0f);
    float angle = juce::jmap (norm, 0.0f, 1.0f, -0.7f, 0.7f);
    auto centre = juce::Point<float> (x + w/2, y + h - 10);
    auto end = centre.getPointOnCircumference (h - 30, angle - juce::MathConstants<float>::pi * 0.5f);
    g.setColour (juce::Colours::black); g.drawLine (juce::Line<float> (centre, end), 2.5f);
    g.setColour (juce::Colours::red);
    if (norm > 0.85f) g.fillEllipse (x + w - 30, y + 10, 12, 12);
}
void LaPautaSLEditor::drawLEDBar (juce::Graphics& g, int x, int y, int w, int h, float db)
{
    int lit = (int) juce::jmap (juce::jlimit (-18.0f, 18.0f, db), -18.0f, 18.0f, 0.0f, 10.0f);
    for (int i = 0; i < 10; ++i)
    {
        juce::Colour c = i < 4 ? juce::Colours::green : (i < 7 ? juce::Colours::yellow : juce::Colours::red);
        if (i >= lit) c = juce::Colour (0xff222222);
        g.setColour (c); g.fillRect (x, y + h - (i+1)*8, w, 6);
    }
}
void LaPautaSLEditor::resized()
{
    int kw = 105, kh = 105;
    int y1 = 225;
    thr.setBounds (25, y1, kw, kh); lThr.setBounds (25, y1-20, kw, 20);
    ratio.setBounds (155, y1, kw, kh); lRatio.setBounds (155, y1-20, kw, 20);
    att.setBounds (285, y1, kw, kh); lAtt.setBounds (285, y1-20, kw, 20);
    rel.setBounds (415, y1, kw, kh); lRel.setBounds (415, y1-20, kw, 20);
    makeup.setBounds (545, y1, kw, kh); lMakeup.setBounds (545, y1-20, kw, 20);
    mix.setBounds (675, y1, kw, kh); lMix.setBounds (675, y1-20, kw, 20);
    hpf.setBounds (840, y1+30, 140, 90); lHpf.setBounds (840, y1-20, 140, 20);
    sidechainBtn.setBounds (875, 250, 70, 25);

    int y2 = 415;
    sat.setBounds (30, y2, kw, kh); lSat.setBounds (30, y2-20, kw, 20);
    punch.setBounds (195, y2, kw, kh); lPunch.setBounds (195, y2-20, kw, 20);
    parallel.setBounds (360, y2, kw, kh); lParallel.setBounds (360, y2-20, kw, 20);
    link.setBounds (525, y2, kw, kh); lLink.setBounds (525, y2-20, kw, 20);
    air.setBounds (675, y2, kw, kh); lAir.setBounds (675, y2-20, kw, 20);
    over2x.setBounds (675, 425, 40, 25); over4x.setBounds (720, 425, 40, 25); over8x.setBounds (765, 425, 40, 25);
    modeClassic.setBounds (850, 490, 120, 25); modePunchy.setBounds (850, 520, 120, 25); modeSmooth.setBounds (850, 550, 120, 25);
    presetLabel.setBounds (20, 585, 70, 25);
    presetBox.setBounds (95, 585, 220, 25);
}
