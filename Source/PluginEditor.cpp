#include "PluginProcessor.h"
#include "PluginEditor.h"
LaPautaSLAudioProcessorEditor::LaPautaSLAudioProcessorEditor (LaPautaSLAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (1100, 620);
    setLookAndFeel(&lnf);
    setup(thresh, lThresh, "THRESHOLD", " dB");
    setup(ratio, lRatio, "RATIO", " : 1");
    setup(attack, lAttack, "ATTACK", " ms");
    setup(release, lRelease, "RELEASE", " ms");
    setup(makeup, lMakeup, "MAKE UP", " dB");
    setup(mix, lMix, "MIX", " %");
    setup(sidechain, lSidechain, "HPF", " Hz");
    a1 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "threshold", thresh);
    a2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "ratio", ratio);
    a3 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "attack", attack);
    a4 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "release", release);
    a5 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "makeup", makeup);
    a6 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "mix", mix);
    a7 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "sidechain", sidechain);
    b1 = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "sidechainOn", sidechainOn);
    lTitle.setText("La Pauta   SSL COMPRESSOR   HYBRID EDITION", juce::dontSendNotification);
    lTitle.setFont(juce::Font(34.0f, juce::Font::bold));
    lTitle.setColour(juce::Label::textColourId, juce::Colours::white);
    lTitle.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lTitle);
    lSub.setText("CRIS LA PAUTA RD  •  AUDIO SOLUTIONS", juce::dontSendNotification);
    lSub.setFont(juce::Font(13.0f, juce::Font::bold));
    lSub.setColour(juce::Label::textColourId, juce::Colour(0xffffc400));
    lSub.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lSub);
    lFooter.setText("EL SONIDO QUE MARCA LA DIFERENCIA", juce::dontSendNotification);
    lFooter.setFont(juce::Font(11.0f, juce::Font::bold));
    lFooter.setColour(juce::Label::textColourId, juce::Colour(0xff888888));
    lFooter.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lFooter);
    sidechainOn.setButtonText("SIDECHAIN  ON");
    sidechainOn.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    sidechainOn.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xff00e5ff));
    addAndMakeVisible(sidechainOn);
    startTimerHz(60);
}
LaPautaSLAudioProcessorEditor::~LaPautaSLAudioProcessorEditor() { setLookAndFeel(nullptr); }
void LaPautaSLAudioProcessorEditor::setup(juce::Slider& s, juce::Label& l, juce::String t, juce::String suffix)
{
    s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 24);
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff0a0a0a));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff333333));
    s.setTextValueSuffix(suffix);
    addAndMakeVisible(s);
    l.setText(t, juce::dontSendNotification);
    l.setFont(juce::Font(14.0f, juce::Font::bold));
    l.setColour(juce::Label::textColourId, juce::Colour(0xffcccccc));
    l.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(l);
}
void LaPautaSLAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fondo premium degradado
    juce::ColourGradient bg(juce::Colour(0xff2b2b2b), 0, 0, juce::Colour(0xff0e0e0e), 0, (float)getHeight(), false);
    g.setGradientFill(bg);
    g.fillAll();
    // Header
    g.setColour(juce::Colour(0xff050514));
    g.fillRect(0, 0, getWidth(), 88);
    g.setColour(juce::Colour(0xff00e5ff));
    g.drawLine(0, 88, getWidth(), 88, 2.0f);
    g.setColour(juce::Colour(0xffffc400));
    g.drawLine(0, 90, getWidth(), 90, 1.0f);

    // --- VU METERS REALES ---
    auto drawVU = [&](int x, float db, bool isGR, juce::String title){
        auto area = juce::Rectangle<float>((float)x, 115, 340, 125);
        g.setColour(juce::Colour(0xfffff2cc));
        g.fillRoundedRectangle(area, 8);
        g.setColour(juce::Colours::black);
        g.setFont(juce::Font(12.0f, juce::Font::bold));
        g.drawText(title, area.reduced(10, 5).removeFromBottom(22), juce::Justification::centred);
        g.setFont(10.0f);
        if (!isGR) g.drawText("-20   -10   -7   -5   -3   -1   0   +3", area.getX()+20, area.getY()+8, area.getWidth()-40, 16, juce::Justification::centred);
        else g.drawText("0    1    3    6    9    12    15", area.getX()+20, area.getY()+8, area.getWidth()-40, 16, juce::Justification::centred);
        // Aguja REAL que se mueve
        float norm = isGR ? juce::jmap(juce::jlimit(0.0f, 15.0f, db), 0.0f, 15.0f, -0.9f, 0.9f)
                          : juce::jmap(juce::jlimit(-30.0f, 3.0f, db), -30.0f, 3.0f, -0.95f, 0.95f);
        auto centre = juce::Point<float>(area.getCentreX(), area.getBottom() - 18);
        auto tip = centre.getPointOnCircumference(78, -1.5708f + norm);
        g.setColour(juce::Colours::red);
        g.drawLine(juce::Line<float>(centre, tip), 2.5f);
        g.setColour(juce::Colours::black);
        g.fillEllipse(centre.x - 4, centre.y - 4, 8, 8);
        g.setFont(11.0f);
        g.drawText(juce::String(db, 1) + " dB", area.getX(), area.getY()+28, area.getWidth(), 16, juce::Justification::centred);
    };
    drawVU(205, smoothIn, false, "INPUT LEVEL");
    drawVU(560, smoothGR, true, "GAIN REDUCTION");

    // --- MEDIDORES VERTICALES REALES ---
    auto drawBar = [&](int x, float db, juce::String t){
        g.setColour(juce::Colour(0xff0a0a0a));
        g.fillRoundedRectangle((float)x, 115, 72, 125, 6);
        g.setColour(juce::Colours::white);
        g.setFont(11.0f);
        g.drawText(t, (float)x, 115, 72, 18, juce::Justification::centred);
        float h = juce::jmap(juce::jlimit(-30.0f, 3.0f, db), -30.0f, 3.0f, 0.0f, 78.0f);
        auto bar = juce::Rectangle<float>((float)x+10, 240 - h, 52, h);
        juce::ColourGradient grad = juce::ColourGradient(juce::Colour(0xff00ff00), bar.getTopLeft(), juce::Colour(0xff00e5ff), bar.getBottomLeft(), false);
        g.setGradientFill(grad);
        g.fillRoundedRectangle(bar, 3);
        g.setColour(juce::Colour(0xff00e5ff));
        g.setFont(10.0f);
        g.drawText(juce::String(db, 1), (float)x, 240, 72, 16, juce::Justification::centred);
    };
    drawBar(45, smoothIn, "INPUT");
    drawBar(983, smoothOut, "OUTPUT");

    // Caja Sidechain
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRoundedRectangle(983, 285, 92, 155, 8);
    g.setColour(juce::Colour(0xff333333));
    g.drawRoundedRectangle(983, 285, 92, 155, 8, 1.0f);

    // Linea footer
    g.setColour(juce::Colour(0xffff2222));
    g.drawLine(0, getHeight()-2, getWidth(), getHeight()-2, 2.0f);
}
void LaPautaSLAudioProcessorEditor::resized()
{
    lTitle.setBounds(0, 8, getWidth(), 42);
    lSub.setBounds(0, 50, getWidth(), 20);
    int y = 285, h = 140, w = 135, gap = 12, startX = 35;
    thresh.setBounds(startX, y, w, h); lThresh.setBounds(startX, y-22, w, 20);
    ratio.setBounds(startX + (w+gap)*1, y, w, h); lRatio.setBounds(startX + (w+gap)*1, y-22, w, 20);
    attack.setBounds(startX + (w+gap)*2, y, w, h); lAttack.setBounds(startX + (w+gap)*2, y-22, w, 20);
    release.setBounds(startX + (w+gap)*3, y, w, h); lRelease.setBounds(startX + (w+gap)*3, y-22, w, 20);
    makeup.setBounds(startX + (w+gap)*4, y, w, h); lMakeup.setBounds(startX + (w+gap)*4, y-22, w, 20);
    mix.setBounds(startX + (w+gap)*5, y, w, h); lMix.setBounds(startX + (w+gap)*5, y-22, w, 20);
    sidechainOn.setBounds(978, 292, 100, 24);
    sidechain.setBounds(978, 320, 100, 90);
    lSidechain.setBounds(978, 415, 100, 20);
    lFooter.setBounds(0, getHeight() - 28, getWidth(), 20);
}
void LaPautaSLAudioProcessorEditor::timerCallback()
{
    float targetIn = audioProcessor.getInputLevel();
    float targetOut = audioProcessor.getOutputLevel();
    float targetGR = audioProcessor.getGainReduction();
    smoothIn = smoothIn * 0.75f + targetIn * 0.25f;
    smoothOut = smoothOut * 0.75f + targetOut * 0.25f;
    smoothGR = smoothGR * 0.8f + targetGR * 0.2f;
    if (smoothIn < -60) smoothIn = -60;
    repaint();
}
