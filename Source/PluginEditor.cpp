
#include "PluginEditor.h"

LaPautaSLLookAndFeel::LaPautaSLLookAndFeel()
{
    setColour(juce::Slider::rotarySliderFillColourId,
              juce::Colour(0xffd2a85a));
    setColour(juce::Slider::rotarySliderOutlineColourId,
              juce::Colour(0xff292b2c));
    setColour(juce::Slider::thumbColourId,
              juce::Colour(0xfff0d59a));
}

void LaPautaSLLookAndFeel::drawRotarySlider(
    juce::Graphics& g,
    int x,
    int y,
    int width,
    int height,
    float sliderPos,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider& slider)
{
    juce::ignoreUnused(slider);

    auto bounds = juce::Rectangle<float>(
        (float)x, (float)y, (float)width, (float)height
    ).reduced(8.0f);

    auto diameter = juce::jmin(bounds.getWidth(), bounds.getHeight());
    auto radius = diameter * 0.5f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();

    auto angle = rotaryStartAngle +
        sliderPos * (rotaryEndAngle - rotaryStartAngle);

    g.setColour(juce::Colour(0xff17191a));
    g.fillEllipse(
        centreX - radius,
        centreY - radius,
        diameter,
        diameter
    );

    g.setColour(juce::Colour(0xff55585a));
    g.drawEllipse(
        centreX - radius,
        centreY - radius,
        diameter,
        diameter,
        2.0f
    );

    auto innerRadius = radius * 0.78f;

    g.setGradientFill(
        juce::ColourGradient(
            juce::Colour(0xff55585a),
            centreX,
            centreY - innerRadius,
            juce::Colour(0xff111314),
            centreX,
            centreY + innerRadius,
            false
        )
    );

    g.fillEllipse(
        centreX - innerRadius,
        centreY - innerRadius,
        innerRadius * 2.0f,
        innerRadius * 2.0f
    );

    g.setColour(juce::Colour(0xffd0d0cc));
    g.drawEllipse(
        centreX - innerRadius,
        centreY - innerRadius,
        innerRadius * 2.0f,
        innerRadius * 2.0f,
        1.0f
    );

    juce::Path pointer;
    auto pointerLength = radius * 0.62f;
    auto pointerThickness = 3.0f;

    pointer.addRoundedRectangle(
        -pointerThickness * 0.5f,
        -pointerLength,
        pointerThickness,
        pointerLength * 0.34f,
        1.5f
    );

    g.setColour(juce::Colour(0xffe3c17d));
    g.fillPath(
        pointer,
        juce::AffineTransform::rotation(angle)
            .translated(centreX, centreY)
    );

    auto arcRadius = radius + 3.0f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(
        centreX, centreY,
        arcRadius, arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true
    );

    g.setColour(juce::Colour(0xff393c3e));
    g.strokePath(
        backgroundArc,
        juce::PathStrokeType(3.0f)
    );

    juce::Path valueArc;
    valueArc.addCentredArc(
        centreX, centreY,
        arcRadius, arcRadius,
        0.0f,
        rotaryStartAngle,
        angle,
        true
    );

    g.setColour(juce::Colour(0xffd2a85a));
    g.strokePath(
        valueArc,
        juce::PathStrokeType(3.0f)
    );
}

LaPautaSLEditor::LaPautaSLEditor(
    LaPautaSLCompresorAudioProcessor& processor)
    : AudioProcessorEditor(&processor),
      audioProcessor(processor)
{
    setLookAndFeel(&lookAndFeel);

    configureSlider(thresholdSlider, thresholdLabel, "THRESHOLD");
    configureSlider(ratioSlider, ratioLabel, "RATIO");
    configureSlider(releaseSlider, releaseLabel, "RELEASE");
    configureSlider(makeupSlider, makeupLabel, "MAKE UP");
    configureSlider(mixSlider, mixLabel, "MIX");
    configureSlider(hpfSlider, hpfLabel, "SIDECHAIN HPF");

    thresholdAttachment =
        std::make_unique<SliderAttachment>(
            processor.parameters, "THRESHOLD", thresholdSlider);

    ratioAttachment =
        std::make_unique<SliderAttachment>(
            processor.parameters, "RATIO", ratioSlider);

    releaseAttachment =
        std::make_unique<SliderAttachment>(
            processor.parameters, "RELEASE", releaseSlider);

    makeupAttachment =
        std::make_unique<SliderAttachment>(
            processor.parameters, "MAKEUP", makeupSlider);

    mixAttachment =
        std::make_unique<SliderAttachment>(
            processor.parameters, "MIX", mixSlider);

    hpfAttachment =
        std::make_unique<SliderAttachment>(
            processor.parameters, "HPF", hpfSlider);

    setSize(1200, 760);
    startTimerHz(20);
}

LaPautaSLEditor::~LaPautaSLEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void LaPautaSLEditor::configureSlider(
    juce::Slider& slider,
    juce::Label& label,
    const juce::String& labelText)
{
    addAndMakeVisible(slider);
    addAndMakeVisible(label);

    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(
        juce::Slider::TextBoxBelow,
        false,
        90,
        24
    );

    slider.setColour(
        juce::Slider::textBoxTextColourId,
        juce::Colour(0xffe4e0d6)
    );

    slider.setColour(
        juce::Slider::textBoxBackgroundColourId,
        juce::Colour(0xff202324)
    );

    slider.setColour(
        juce::Slider::textBoxOutlineColourId,
        juce::Colour(0xff55585a)
    );

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(
        juce::Label::textColourId,
        juce::Colour(0xff292a29)
    );
    label.setFont(juce::Font(15.0f, juce::Font::bold));
}

void LaPautaSLEditor::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat();

    g.setGradientFill(
        juce::ColourGradient(
            juce::Colour(0xffd5d2ca),
            0.0f, 0.0f,
            juce::Colour(0xff8e918e),
            0.0f, (float)getHeight(),
            false
        )
    );
    g.fillAll();

    g.setColour(juce::Colour(0xffe7e4dc));
    g.drawRect(area.reduced(8.0f), 2.0f);

    g.setColour(juce::Colour(0xff252829));
    g.fillRoundedRectangle(
        22.0f, 20.0f,
        1156.0f, 105.0f,
        8.0f
    );

    g.setColour(juce::Colour(0xffe1c18a));
    g.setFont(juce::Font(38.0f, juce::Font::italic));
    g.drawText(
        "La Pauta",
        42, 27, 350, 55,
        juce::Justification::centredLeft
    );

    g.setColour(juce::Colour(0xffe7e5df));
    g.setFont(juce::Font(19.0f, juce::Font::bold));
    g.drawText(
        "SL COMPRESSOR",
        45, 78, 350, 25,
        juce::Justification::centredLeft
    );

    g.setColour(juce::Colour(0xffd2a85a));
    g.fillRoundedRectangle(
        1010.0f, 48.0f,
        125.0f, 34.0f,
        5.0f
    );

    g.setColour(juce::Colour(0xff202324));
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText(
        "STEREO",
        1010, 48, 125, 34,
        juce::Justification::centred
    );

    g.setColour(juce::Colour(0xff343738));
    g.fillRoundedRectangle(
        22.0f, 150.0f,
        1156.0f, 550.0f,
        8.0f
    );

    g.setColour(juce::Colour(0xffc5c2b9));
    g.drawRoundedRectangle(
        22.0f, 150.0f,
        1156.0f, 550.0f,
        8.0f,
        2.0f
    );

    g.setColour(juce::Colour(0xffd2a85a));
    g.setFont(juce::Font(18.0f, juce::Font::bold));
    g.drawText(
        "COMPRESSOR SECTION",
        50, 165, 400, 30,
        juce::Justification::centredLeft
    );

    g.setColour(juce::Colour(0xff252829));
    g.fillRoundedRectangle(
        50.0f, 225.0f,
        1100.0f, 365.0f,
        8.0f
    );

    g.setColour(juce::Colour(0xffb7b4ac));
    g.drawRoundedRectangle(
        50.0f, 225.0f,
        1100.0f, 365.0f,
        8.0f,
        1.5f
    );

    g.setColour(juce::Colour(0xffd2a85a));
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText(
        "ANALOG STYLE STEREO DYNAMICS",
        60, 610, 600, 28,
        juce::Justification::centredLeft
    );

    g.setColour(juce::Colour(0xff202324));
    g.setFont(juce::Font(12.0f));
    g.drawText(
        "LA PAUTA AUDIO  |  VST3  |  WINDOWS 64-BIT",
        650, 610, 480, 28,
        juce::Justification::centredRight
    );

    g.setColour(juce::Colour(0xffd2a85a));
    g.fillRect(50, 650, 1100, 2);
}

void LaPautaSLEditor::resized()
{
    const int knobWidth = 150;
    const int knobHeight = 170;
    const int labelHeight = 30;
    const int y = 300;

    const int positions[] = {
        90, 270, 450, 630, 810, 990
    };

    juce::Slider* sliders[] = {
        &thresholdSlider,
        &ratioSlider,
        &releaseSlider,
        &makeupSlider,
        &mixSlider,
        &hpfSlider
    };

    juce::Label* labels[] = {
        &thresholdLabel,
        &ratioLabel,
        &releaseLabel,
        &makeupLabel,
        &mixLabel,
        &hpfLabel
    };

    for (int i = 0; i < 6; ++i)
    {
        sliders[i]->setBounds(
            positions[i],
            y,
            knobWidth,
            knobHeight
        );

        labels[i]->setBounds(
            positions[i],
            y - labelHeight - 8,
            knobWidth,
            labelHeight
        );
    }
}

void LaPautaSLEditor::timerCallback()
{
    repaint();
}
