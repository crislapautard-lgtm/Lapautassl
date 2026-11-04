
#include "PluginEditor.h"

LaPautaSLAudioProcessorEditor::LaPautaSLAudioProcessorEditor(
    LaPautaSLAudioProcessor& processor)
    : AudioProcessorEditor(&processor),
      audioProcessor(processor)
{
    setLookAndFeel(&metalLook);
    setSize(900, 300);
    setResizable(false, false);

    auto setupControl = [this](
        juce::Slider& slider,
        juce::Label& value,
        juce::Label& name,
        const juce::String& label)
    {
        setupSlider(slider);
        setupLabel(value, "0.0");
        setupLabel(name, label);

        addAndMakeVisible(slider);
        addAndMakeVisible(value);
        addAndMakeVisible(name);
    };

    setupControl(thresholdSlider, thresholdValue, thresholdName, "THRESHOLD");
    setupControl(ratioSlider, ratioValue, ratioName, "RATIO");
    setupControl(attackSlider, attackValue, attackName, "ATTACK");
    setupControl(makeupSlider, makeupValue, makeupName, "MAKEUP");
    setupControl(mixSlider, mixValue, mixName, "MIX");

    auto& state = audioProcessor.parameters;

    thresholdAttachment = std::make_unique<SliderAttachment>(
        state, "THRESHOLD", thresholdSlider);
    ratioAttachment = std::make_unique<SliderAttachment>(
        state, "RATIO", ratioSlider);
    attackAttachment = std::make_unique<SliderAttachment>(
        state, "ATTACK", attackSlider);
    makeupAttachment = std::make_unique<SliderAttachment>(
        state, "MAKEUP", makeupSlider);
    mixAttachment = std::make_unique<SliderAttachment>(
        state, "MIX", mixSlider);

    auto updateValues = [this]()
    {
        thresholdValue.setText(
            juce::String(thresholdSlider.getValue(), 1) + " dB",
            juce::dontSendNotification);

        ratioValue.setText(
            juce::String(ratioSlider.getValue(), 1) + ":1",
            juce::dontSendNotification);

        attackValue.setText(
            juce::String(attackSlider.getValue(), 1) + " ms",
            juce::dontSendNotification);

        makeupValue.setText(
            "+" + juce::String(makeupSlider.getValue(), 1) + " dB",
            juce::dontSendNotification);

        mixValue.setText(
            juce::String(mixSlider.getValue(), 0) + "%",
            juce::dontSendNotification);
    };

    thresholdSlider.onValueChange = updateValues;
    ratioSlider.onValueChange = updateValues;
    attackSlider.onValueChange = updateValues;
    makeupSlider.onValueChange = updateValues;
    mixSlider.onValueChange = updateValues;

    updateValues();
    startTimerHz(30);
}

LaPautaSLAudioProcessorEditor::~LaPautaSLAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void LaPautaSLAudioProcessorEditor::setupSlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setLookAndFeel(&metalLook);
}

void LaPautaSLAudioProcessorEditor::setupLabel(
    juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId,
                    juce::Colour(0xffbdbdbd));
    label.setFont(juce::Font(11.0f, juce::Font::bold));
}

void LaPautaSLAudioProcessorEditor::MetalLookAndFeel::drawRotarySlider(
    juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPos,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider&)
{
    const float diameter = juce::jmin(
        (float)width, (float)height) - 18.0f;

    const float cx = x + width * 0.5f;
    const float cy = y + height * 0.5f;
    const float radius = diameter * 0.5f;

    const float angle = rotaryStartAngle +
        sliderPos * (rotaryEndAngle - rotaryStartAngle);

    g.setColour(juce::Colours::black.withAlpha(0.85f));
    g.fillEllipse(cx - radius + 3, cy - radius + 5,
                  diameter, diameter);

    juce::ColourGradient ring(
        juce::Colour(0xffb9b9b9), cx, cy - radius,
        juce::Colour(0xff353535), cx, cy + radius, false);

    g.setGradientFill(ring);
    g.fillEllipse(cx - radius, cy - radius, diameter, diameter);

    juce::ColourGradient metal(
        juce::Colour(0xff555555), cx, cy - radius * 0.75f,
        juce::Colour(0xff111111), cx, cy + radius, false);

    g.setGradientFill(metal);
    g.fillEllipse(cx - radius + 4, cy - radius + 4,
                  diameter - 8, diameter - 8);

    for (int i = 0; i <= 20; ++i)
    {
        const float a = rotaryStartAngle +
            (rotaryEndAngle - rotaryStartAngle) * i / 20.0f;

        const float inner = radius + 3.0f;
        const float outer = radius + 9.0f;

        g.setColour(juce::Colour(0xffdddddd));
        g.drawLine(
            cx + std::cos(a) * inner,
            cy + std::sin(a) * inner,
            cx + std::cos(a) * outer,
            cy + std::sin(a) * outer,
            1.2f);
    }

    g.setColour(juce::Colour(0xff222222));
    g.fillEllipse(cx - radius * 0.68f, cy - radius * 0.68f,
                  radius * 1.36f, radius * 1.36f);

    g.setColour(juce::Colour(0xff777777));
    g.drawEllipse(cx - radius * 0.68f, cy - radius * 0.68f,
                  radius * 1.36f, radius * 1.36f, 1.0f);

    const float indicatorLength = radius * 0.48f;
    const float ix = cx + std::cos(angle) * indicatorLength;
    const float iy = cy + std::sin(angle) * indicatorLength;

    g.setColour(juce::Colours::white);
    g.drawLine(cx, cy, ix, iy, 3.0f);
    g.fillEllipse(ix - 2.0f, iy - 2.0f, 4.0f, 4.0f);
}

void LaPautaSLAudioProcessorEditor::paint(juce::Graphics& g)
{
    juce::ColourGradient background(
        juce::Colour(0xff3a3a3a), 0.0f, 0.0f,
        juce::Colour(0xff1a1a1a), 0.0f, 300.0f, false);

    g.setGradientFill(background);
    g.fillAll();

    g.setColour(juce::Colours::white.withAlpha(0.025f));

    for (int y = 0; y < 300; y += 3)
        g.drawHorizontalLine(y, 0.0f, 900.0f);

    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText("LA PAUTA - SSL BUS COMPRESSOR",
               28, 12, 520, 25, juce::Justification::left);

    g.setColour(juce::Colour(0xff999999));
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.drawText("HYBRID EDITION",
               29, 38, 250, 16, juce::Justification::left);

    auto drawScrew = [&g](float x, float y)
    {
        g.setColour(juce::Colour(0xff111111));
        g.fillEllipse(x, y, 15.0f, 15.0f);

        g.setColour(juce::Colour(0xff888888));
        g.drawEllipse(x, y, 15.0f, 15.0f, 1.0f);

        g.setColour(juce::Colour(0xffcccccc));
        g.drawLine(x + 4.0f, y + 11.0f,
                   x + 11.0f, y + 4.0f, 1.2f);
    };

    drawScrew(8, 8);
    drawScrew(877, 8);
    drawScrew(8, 277);
    drawScrew(877, 277);

    juce::Label* values[] = {
        &thresholdValue, &ratioValue, &attackValue,
        &makeupValue, &mixValue
    };

    for (auto* label : values)
    {
        auto r = label->getBounds().toFloat();

        g.setColour(juce::Colour(0xff080808));
        g.fillRoundedRectangle(r, 3.0f);

        g.setColour(juce::Colour(0xff555555));
        g.drawRoundedRectangle(r, 3.0f, 1.0f);

        label->setColour(
            juce::Label::textColourId,
            juce::Colour(0xffffbd55));
    }

    g.setColour(juce::Colour(0xffaaaaaa));
    g.setFont(juce::Font(12.0f, juce::Font::bold));
    g.drawText("GR", 790, 65, 65, 20,
               juce::Justification::centred);

    const float reduction = juce::jlimit(
        0.0f, 14.0f, -audioProcessor.getGainReduction());

    const int activeSegments = juce::jlimit(
        0, 14, (int)std::round(reduction));

    const float meterX = 802.0f;
    const float meterY = 92.0f;
    const float segmentHeight = 10.0f;
    const float gap = 2.0f;

    for (int i = 0; i < 14; ++i)
    {
        const float yy = meterY +
            (13 - i) * (segmentHeight + gap);

        juce::Colour colour;

        if (i < 7)
            colour = juce::Colour(0xff36a65b);
        else if (i < 11)
            colour = juce::Colour(0xffd2a52c);
        else
            colour = juce::Colour(0xffb83c32);

        if (i >= activeSegments)
            colour = juce::Colour(0xff292929);

        g.setColour(colour);
        g.fillRoundedRectangle(meterX, yy, 22.0f, segmentHeight, 2.0f);
        g.fillRoundedRectangle(meterX + 25.0f, yy, 22.0f,
                               segmentHeight, 2.0f);
    }

    g.setColour(juce::Colour(0xffffbd55));
    g.setFont(juce::Font(14.0f, juce::Font::bold));

    g.drawText(
        juce::String(audioProcessor.getGainReduction(), 1) + " dB",
        785, 252, 90, 22, juce::Justification::centred);
}

void LaPautaSLAudioProcessorEditor::resized()
{
    const int knobY = 105;
    const int knobSize = 105;
    const int valueY = 75;
    const int valueW = 90;
    const int valueH = 22;

    const int centers[] = { 105, 245, 385, 525, 665 };

    juce::Slider* sliders[] = {
        &thresholdSlider, &ratioSlider, &attackSlider,
        &makeupSlider, &mixSlider
    };

    juce::Label* values[] = {
        &thresholdValue, &ratioValue, &attackValue,
        &makeupValue, &mixValue
    };

    juce::Label* names[] = {
        &thresholdName, &ratioName, &attackName,
        &makeupName, &mixName
    };

    for (int i = 0; i < 5; ++i)
    {
        sliders[i]->setBounds(
            centers[i] - knobSize / 2,
            knobY, knobSize, knobSize);

        values[i]->setBounds(
            centers[i] - valueW / 2,
            valueY, valueW, valueH);

        names[i]->setBounds(
            centers[i] - 55, 222, 110, 20);
    }
}

void LaPautaSLAudioProcessorEditor::timerCallback()
{
    repaint();
}
