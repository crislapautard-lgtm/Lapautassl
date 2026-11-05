
#include "PluginEditor.h"

namespace
{
    const juce::Colour background(0xff17191c);
    const juce::Colour panel(0xff25292e);
    const juce::Colour gold(0xffffd18a);
    const juce::Colour turquoise(0xff42d6c5);
    const juce::Colour white(0xffeeeeee);
}

LaPautaSLEditor::LaPautaSLEditor(LaPautaSLCompresorAudioProcessor& processor)
    : AudioProcessorEditor(&processor), audioProcessor(processor)
{
    setSize(1100, 650);

    configureSlider(thresholdSlider, thresholdLabel, "THRESHOLD");
    configureSlider(ratioSlider, ratioLabel, "RATIO");
    configureSlider(attackSlider, attackLabel, "ATTACK");
    configureSlider(releaseSlider, releaseLabel, "RELEASE");
    configureSlider(makeupSlider, makeupLabel, "MAKEUP GAIN");
    configureSlider(mixSlider, mixLabel, "DRY / WET");
    configureSlider(hpfSlider, hpfLabel, "SIDECHAIN HPF");

    thresholdAttachment = std::make_unique<Attachment>(audioProcessor.parameters, "THRESHOLD", thresholdSlider);
    ratioAttachment = std::make_unique<Attachment>(audioProcessor.parameters, "RATIO", ratioSlider);
    attackAttachment = std::make_unique<Attachment>(audioProcessor.parameters, "ATTACK", attackSlider);
    releaseAttachment = std::make_unique<Attachment>(audioProcessor.parameters, "RELEASE", releaseSlider);
    makeupAttachment = std::make_unique<Attachment>(audioProcessor.parameters, "MAKEUP", makeupSlider);
    mixAttachment = std::make_unique<Attachment>(audioProcessor.parameters, "MIX", mixSlider);
    hpfAttachment = std::make_unique<Attachment>(audioProcessor.parameters, "HPF", hpfSlider);

    presetLabel.setText("SSL COMPRESSOR PRESETS", juce::dontSendNotification);
    presetLabel.setFont(juce::Font(13.0f, juce::Font::bold));
    presetLabel.setColour(juce::Label::textColourId, gold);
    presetLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(presetLabel);

    const char* names[] =
    {
        "SSL 4000 Vocal",
        "SSL Vocal Tight",
        "SSL Drum Punch",
        "SSL Drum Bus",
        "SSL Bass Control",
        "SSL Mix Bus",
        "SSL Glue",
        "SSL Fast Attack",
        "SSL Slow Attack"
    };

    for (int i = 0; i < 9; ++i)
        presetBox.addItem(names[i], i + 1);

    presetBox.setSelectedId(1, juce::dontSendNotification);
    presetBox.setColour(juce::ComboBox::backgroundColourId, panel);
    presetBox.setColour(juce::ComboBox::textColourId, white);
    presetBox.setColour(juce::ComboBox::outlineColourId, gold);
    presetBox.setColour(juce::ComboBox::arrowColourId, turquoise);

    presetBox.onChange = [this]
    {
        const int id = presetBox.getSelectedId();
        if (id >= 1 && id <= 9)
            audioProcessor.setCurrentProgram(id - 1);
    };

    addAndMakeVisible(presetBox);
    startTimerHz(10);
}

LaPautaSLEditor::~LaPautaSLEditor()
{
    stopTimer();
}

void LaPautaSLEditor::configureSlider(
    juce::Slider& slider,
    juce::Label& label,
    const juce::String& name)
{
    label.setText(name, juce::dontSendNotification);
    label.setFont(juce::Font(13.0f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, gold);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);

    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 23);

    slider.setColour(juce::Slider::rotarySliderFillColourId, turquoise);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff555b62));
    slider.setColour(juce::Slider::thumbColourId, gold);
    slider.setColour(juce::Slider::textBoxTextColourId, white);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff111315));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff555b62));

    addAndMakeVisible(slider);
}

void LaPautaSLEditor::paint(juce::Graphics& g)
{
    g.fillAll(background);

    g.setColour(juce::Colour(0xff101214));
    g.fillRoundedRectangle(20.0f, 20.0f, 1060.0f, 105.0f, 10.0f);

    g.setColour(gold);
    g.setFont(juce::Font(29.0f, juce::Font::bold));
    g.drawText("LA PAUTA", 40, 29, 300, 38, juce::Justification::centredLeft);

    g.setColour(white);
    g.setFont(juce::Font(19.0f, juce::Font::bold));
    g.drawText("SL COMPRESSOR", 40, 68, 350, 27, juce::Justification::centredLeft);

    g.setColour(panel);
    g.fillRoundedRectangle(25.0f, 145.0f, 1050.0f, 435.0f, 12.0f);

    g.setColour(juce::Colour(0xff555b62));
    g.drawRoundedRectangle(25.0f, 145.0f, 1050.0f, 435.0f, 12.0f, 1.0f);

    g.setColour(turquoise);
    g.fillRoundedRectangle(40.0f, 594.0f, 1020.0f, 3.0f, 1.5f);

    g.setColour(white.withAlpha(0.7f));
    g.setFont(juce::Font(11.0f));
    g.drawText("LA PAUTA AUDIO  |  PROFESSIONAL DYNAMICS",
        40, 603, 1020, 20, juce::Justification::centred);
}

void LaPautaSLEditor::resized()
{
    presetLabel.setBounds(590, 39, 210, 25);
    presetBox.setBounds(810, 39, 235, 30);

    juce::Slider* sliders[] =
    {
        &thresholdSlider, &ratioSlider, &attackSlider,
        &releaseSlider, &makeupSlider, &mixSlider, &hpfSlider
    };

    juce::Label* labels[] =
    {
        &thresholdLabel, &ratioLabel, &attackLabel,
        &releaseLabel, &makeupLabel, &mixLabel, &hpfLabel
    };

    const int positionsX[] = { 55, 315, 575, 835 };
    const int positionsY[] = { 170, 375 };

    for (int i = 0; i < 7; ++i)
    {
        const int column = i % 4;
        const int row = i / 4;

        const int x = positionsX[column];
        const int y = positionsY[row];

        labels[i]->setBounds(x, y, 205, 27);
        sliders[i]->setBounds(x + 15, y + 27, 175, 165);
    }
}

void LaPautaSLEditor::timerCallback()
{
    const int program = audioProcessor.getCurrentProgram();

    if (program >= 0 && program < 9 &&
        presetBox.getSelectedId() != program + 1)
    {
        presetBox.setSelectedId(program + 1, juce::dontSendNotification);
    }
}
