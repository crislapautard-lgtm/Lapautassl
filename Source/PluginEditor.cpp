
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
    ).reduced(7.0f);

    const float diameter = juce::jmin(
        bounds.getWidth(), bounds.getHeight()
    );

    const float radius = diameter * 0.5f;
    const float centreX = bounds.getCentreX();
    const float centreY = bounds.getCentreY();

    const float angle = rotaryStartAngle +
        sliderPos * (rotaryEndAngle - rotaryStartAngle);

    g.setColour(juce::Colour(0xff111314));
    g.fillEllipse(
        centreX - radius, centreY - radius,
        diameter, diameter
    );

    g.setColour(juce::Colour(0xff777775));
    g.drawEllipse(
        centreX - radius, centreY - radius,
        diameter, diameter, 2.0f
    );

    const float innerRadius = radius * 0.78f;

    g.setGradientFill(
        juce::ColourGradient(
            juce::Colour(0xff55585a),
            centreX, centreY - innerRadius,
            juce::Colour(0xff151718),
            centreX, centreY + innerRadius,
            false
        )
    );

    g.fillEllipse(
        centreX - innerRadius,
        centreY - innerRadius,
        innerRadius * 2.0f,
        innerRadius * 2.0f
    );

    g.setColour(juce::Colour(0xff99958b));
    g.drawEllipse(
        centreX - innerRadius,
        centreY - innerRadius,
        innerRadius * 2.0f,
        innerRadius * 2.0f,
        1.0f
    );

    juce::Path pointer;

    const float pointerLength = radius * 0.63f;

    pointer.addRoundedRectangle(
        -1.8f,
        -pointerLength,
        3.6f,
        pointerLength * 0.34f,
        1.5f
    );

    g.setColour(juce::Colour(0xffe3c17d));

    g.fillPath(
        pointer,
        juce::AffineTransform::rotation(angle)
            .translated(centreX, centreY)
    );

    const float arcRadius = radius + 3.0f;

    juce::Path backgroundArc;

    backgroundArc.addCentredArc(
        centreX, centreY,
        arcRadius, arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true
    );

    g.setColour(juce::Colour(0xff505253));
    g.strokePath(
        backgroundArc,
        juce::PathStrokeType(2.5f)
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
        juce::PathStrokeType(2.5f)
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

    setSize(1000, 650);
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

    slider.setSliderStyle(
        juce::Slider::RotaryHorizontalVerticalDrag
    );

    slider.setTextBoxStyle(
        juce::Slider::TextBoxBelow,
        false,
        78,
        22
    );

    slider.setColour(
        juce::Slider::textBoxTextColourId,
        juce::Colour(0xffffe4ad)
    );

    slider.setColour(
        juce::Slider::textBoxBackgroundColourId,
        juce::Colour(0xff17191a)
    );

    slider.setColour(
        juce::Slider::textBoxOutlineColourId,
        juce::Colour(0xff77736b)
    );

    label.setText(
        labelText,
        juce::dontSendNotification
    );

    label.setJustificationType(
        juce::Justification::centred
    );

    label.setColour(
        juce::Label::textColourId,
        juce::Colour(0xffffd68b)
    );

    label.setFont(
        juce::Font(14.0f, juce::Font::bold)
    );
}

void LaPautaSLEditor::paint(juce::Graphics& g)
{
    g.setGradientFill(
        juce::ColourGradient(
            juce::Colour(0xffd5d2ca),
            0.0f, 0.0f,
            juce::Colour(0xff92948f),
            0.0f, (float)getHeight(),
            false
        )
    );

    g.fillAll();

    g.setColour(juce::Colour(0xffe7e4dc));
    g.drawRect(getLocalBounds().reduced(7), 2);

    g.setColour(juce::Colour(0xff252829));
    g.fillRoundedRectangle(
        18.0f, 18.0f,
        964.0f, 95.0f,
        8.0f
    );

    g.setColour(juce::Colour(0xffe1c18a));
    g.setFont(juce::Font(34.0f, juce::Font::italic));

    g.drawText(
        "La Pauta",
        38, 25, 350, 45,
        juce::Justification::centredLeft
    );

    g.setColour(juce::Colour(0xffe7e5df));
    g.setFont(juce::Font(17.0f, juce::Font::bold));

    g.drawText(
        "SL COMPRESSOR",
        40, 72, 350, 23,
        juce::Justification::centredLeft
    );

    g.setColour(juce::Colour(0xffd2a85a));
    g.fillRoundedRectangle(
        830.0f, 48.0f,
        115.0f, 32.0f,
        5.0f
    );

    g.setColour(juce::Colour(0xff202324));
    g.setFont(juce::Font(13.0f, juce::Font::bold));

    g.drawText(
        "STEREO",
        830, 48, 115, 32,
        juce::Justification::centred
    );

    g.setColour(juce::Colour(0xff292d2e));
    g.fillRoundedRectangle(
        35.0f, 135.0f,
        930.0f, 465.0f,
        8.0f
    );

    g.setColour(juce::Colour(0xffb7b4ac));
    g.drawRoundedRectangle(
        35.0f, 135.0f,
        930.0f, 465.0f,
        8.0f, 1.5f
    );

    g.setColour(juce::Colour(0xffd2a85a));
    g.setFont(juce::Font(16.0f, juce::Font::bold));

    g.drawText(
        "COMPRESSOR SECTION",
        55, 150, 350, 25,
        juce::Justification::centredLeft
    );

    g.setColour(juce::Colour(0xff191c1d));
    g.fillRoundedRectangle(
        55.0f, 210.0f,
        890.0f, 330.0f,
        8.0f
    );

    g.setColour(juce::Colour(0xff77736b));
    g.drawRoundedRectangle(
        55.0f, 210.0f,
        890.0f, 330.0f,
        8.0f, 1.0f
    );

    g.setColour(juce::Colour(0xffd2a85a));
    g.setFont(juce::Font(12.0f, juce::Font::bold));

    g.drawText(
        "LA PAUTA AUDIO  |  STEREO DYNAMICS  |  VST3",
        55, 565, 890, 20,
        juce::Justification::centred
    );
}

void LaPautaSLEditor::resized()
{
    const int knobWidth = 130;
    const int knobHeight = 155;

    const int labelWidth = 140;
    const int labelHeight = 28;

    const int positions[] = {
        75, 225, 375, 525, 675, 825
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
        labels[i]->setBounds(
            positions[i],
            255,
            labelWidth,
            labelHeight
        );

        sliders[i]->setBounds(
            positions[i] + 5,
            295,
            knobWidth,
            knobHeight
        );
    }
}

void LaPautaSLEditor::timerCallback()
{
    repaint();
}
