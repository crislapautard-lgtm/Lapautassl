#include "PluginProcessor.h"
#include "PluginEditor.h"

LaPautaSLAudioProcessorEditor::LaPautaSLAudioProcessorEditor (LaPautaSLAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (900, 300);
    auto setup = [&](juce::Slider& s, juce::Label& lab, juce::Label& val, const juce::String& name){
        s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s.setLookAndFeel(&proLNF);
        addAndMakeVisible(s);
        lab.setText(name, juce::NotificationType::dontSendNotification);
        lab.setJustificationType(juce::Justification::centred);
        lab.setColour(juce::Label::textColourId, juce::Colour(0xffb8b8b8));
        lab.setFont(juce::Font(13.0f, juce::Font::plain));
        addAndMakeVisible(lab);
        val.setJustificationType(juce::Justification::centred);
        val.setColour(juce::Label::textColourId, juce::Colour(0xffffcc33));
        val.setFont(juce::Font(22.0f, juce::Font::bold));
        addAndMakeVisible(val);
    };
    setup(thresholdSlider, thresholdLabel, thresholdVal, "THRESHOLD");
    setup(ratioSlider, ratioLabel, ratioVal, "RATIO");
    setup(attackSlider, attackLabel, attackVal, "ATTACK");
    setup(makeupSlider, makeupLabel, makeupVal, "MAKEUP");
    setup(mixSlider, mixLabel, mixVal, "MIX");
    auto& apvts = audioProcessor.apvts;
    attThreshold.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, "threshold", thresholdSlider));
    attRatio.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, "ratio", ratioSlider));
    attAttack.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, "attack", attackSlider));
    attMakeup.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, "makeup", makeupSlider));
    attMix.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, "mix", mixSlider));
    startTimerHz(30);
}

LaPautaSLAudioProcessorEditor::~LaPautaSLAudioProcessorEditor()
{
    stopTimer();
    for(auto* s : {&thresholdSlider, &ratioSlider, &attackSlider, &makeupSlider, &mixSlider})
        s->setLookAndFeel(nullptr);
}

void LaPautaSLAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fondo metal cepillado oscuro - como la imagen
    juce::ColourGradient bg(juce::Colour(0xff3a3a3a), 0, 0, juce::Colour(0xff1a1a1a), 0, (float)getHeight(), false);
    g.setGradientFill(bg);
    g.fillAll();

    // Header
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(0, 0, getWidth(), 52);
    g.setColour(juce::Colour(0xff000000));
    g.drawLine(0, 52, (float)getWidth(), 52, 2.0f);
    g.setColour(juce::Colour(0xffe8e8e8));
    g.setFont(juce::Font(18.0f, juce::Font::bold));
    g.drawText("LA PAUTA - SSL BUS COMPRESSOR", 18, 8, 500, 20, juce::Justification::left);
    g.setColour(juce::Colour(0xffb8b8b8));
    g.setFont(juce::Font(13.0f, juce::Font::plain));
    g.drawText("HYBRID EDITION", 18, 28, 300, 16, juce::Justification::left);

    // Tornillos
    auto screw = [&](int x, int y){
        g.setColour(juce::Colour(0xff0a0a0a));
        g.fillEllipse(x, y, 16, 16);
        g.setColour(juce::Colour(0xff777777));
        g.drawEllipse(x, y, 16, 16, 1.2f);
        g.setColour(juce::Colours::white);
        g.drawLine(x+4, y+8, x+12, y+8, 1.5f);
    };
    screw(10, 65); screw(getWidth()-26, 65);
    screw(10, getHeight()-26); screw(getWidth()-26, getHeight()-26);

    // Cajas amarillas de valores encima de cada perilla - como en la imagen
    auto drawValueBox = [&](juce::Label& val, int x){
        g.setColour(juce::Colour(0xff2a1a00));
        g.fillRoundedRectangle(x, 78, 130, 38, 3.0f);
        g.setColour(juce::Colour(0xff000000));
        g.drawRoundedRectangle(x, 78, 130, 38, 3.0f, 1.0f);
        val.setBounds(x, 78, 130, 38);
    };
    drawValueBox(thresholdVal, 55);
    drawValueBox(ratioVal, 220);
    drawValueBox(attackVal, 385);
    drawValueBox(makeupVal, 550);
    drawValueBox(mixVal, 715);

    // ===== MEDIDOR GR AL LADO DEL MIX - COMO EN LA IMAGEN =====
    int meterX = 845;
    int meterY = 80;
    int meterW = 32;
    int meterH = 165;
    g.setColour(juce::Colour(0xffd4a24a));
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText("GR", meterX, meterY - 18, meterW, 14, juce::Justification::centred);
    g.setColour(juce::Colour(0xff0a0a0a));
    g.fillRect(meterX, meterY, meterW, meterH);
    g.setColour(juce::Colour(0xff333333));
    g.drawRect(meterX, meterY, meterW, meterH, 1);

    // Segmentos verde / amarillo / rojo - FUNCIONA DE VERDAD
    float gr = juce::jlimit(0.0f, 1.0f, grLevel);
    int totalSegs = 14;
    int litSegs = (int)(gr * totalSegs);
    for (int i = 0; i < totalSegs; ++i)
    {
        int segY = meterY + meterH - (i+1) * (meterH / totalSegs);
        int segH = (meterH / totalSegs) - 2;
        bool on = i < litSegs;
        juce::Colour c;
        if (i < 6) c = juce::Colour(0xff33cc44);
        else if (i < 10) c = juce::Colour(0xffffcc00);
        else c = juce::Colour(0xffff2222);
        g.setColour(on ? c : juce::Colour(0xff1a1a1a));
        // Doble columna como en la imagen
        g.fillRect(meterX+4, segY, 10, segH);
        g.fillRect(meterX+18, segY, 10, segH);
    }
    g.setColour(juce::Colour(0xffd4a24a));
    g.setFont(juce::Font(13.0f, juce::Font::bold));
    float grDb = grLevel * -20.0f;
    g.drawText(juce::String(grDb, 1) + " dB", meterX - 12, meterY + meterH + 5, meterW + 24, 14, juce::Justification::centred);
}

void LaPautaSLAudioProcessorEditor::resized()
{
    // Posiciones exactas como en la imagen
    int knob = 130;
    int y = 125;
    auto place = [&](juce::Slider& s, juce::Label& lab, int x){
        s.setBounds(x, y, knob, knob);
        lab.setBounds(x, y + knob + 8, knob, 18);
    };
    place(thresholdSlider, thresholdLabel, 55);
    place(ratioSlider, ratioLabel, 220);
    place(attackSlider, attackLabel, 385);
    place(makeupSlider, makeupLabel, 550);
    place(mixSlider, mixLabel, 715);
}

void LaPautaSLAudioProcessorEditor::timerCallback()
{
    thresholdVal.setText(juce::String(thresholdSlider.getValue(), 1) + " dB", juce::NotificationType::dontSendNotification);
    ratioVal.setText(juce::String(ratioSlider.getValue(), 1) + ":1", juce::NotificationType::dontSendNotification);
    attackVal.setText(juce::String(attackSlider.getValue(), 0) + " ms", juce::NotificationType::dontSendNotification);
    makeupVal.setText(juce::String(makeupSlider.getValue(), 1) + " dB", juce::NotificationType::dontSendNotification);
    mixVal.setText(juce::String((int)mixSlider.getValue()) + "%", juce::NotificationType::dontSendNotification);
    grLevel = -audioProcessor.getGainReduction() / 20.0f;
    repaint();
}
