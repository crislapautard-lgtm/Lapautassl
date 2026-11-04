
#include "PluginEditor.h"

static const juce::Colour backgroundColour(0xff17191b);
static const juce::Colour metalColour(0xff34383b);
static const juce::Colour silverColour(0xffc5c9ca);
static const juce::Colour amberColour(0xffffb94e);
static const juce::Colour cyanColour(0xff50c8dc);

LaPautaLookAndFeel::LaPautaLookAndFeel()
{
    setColour(juce::Slider::thumbColourId, silverColour);
    setColour(juce::Slider::rotarySliderFillColourId, amberColour);
    setColour(juce::Slider::rotarySliderOutlineColourId,
              juce::Colours::transparentBlack);
}

void LaPautaLookAndFeel::drawRotarySlider(
    juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPos,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider& slider)
{
    juce::ignoreUnused(slider);

    const float size = juce::jmin(
        static_cast<float>(width),
        static_cast<float>(height)) * 0.88f;

    const float cx = x + width * 0.5f;
    const float cy = y + height * 0.5f;

    const float radius = size * 0.5f;
    const float angle = rotaryStartAngle
        + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    const float knobRadius = radius * 0.78f;

    // Sombra exterior
    g.setColour(juce::Colours::black.withAlpha(0.75f));
    g.fillEllipse(cx - knobRadius + 3.0f,
                  cy - knobRadius + 5.0f,
                  knobRadius * 2.0f,
                  knobRadius * 2.0f);

    // Anillo metálico
    g.setColour(juce::Colour(0xff777e82));
    g.fillEllipse(cx - knobRadius - 3.0f,
                  cy - knobRadius - 3.0f,
                  knobRadius * 2.0f + 6.0f,
                  knobRadius * 2.0f + 6.0f);

    juce::ColourGradient body(
        juce::Colour(0xff62696d), cx - knobRadius, cy - knobRadius,
        juce::Colour(0xff171a1c), cx + knobRadius, cy + knobRadius,
        false);

    g.setGradientFill(body);
    g.fillEllipse(cx - knobRadius,
                  cy - knobRadius,
                  knobRadius * 2.0f,
                  knobRadius * 2.0f);

    // Cara interior
    juce::ColourGradient face(
        juce::Colour(0xff454b4f), cx - knobRadius * 0.5f,
        cy - knobRadius,
        juce::Colour(0xff202326), cx + knobRadius * 0.5f,
        cy + knobRadius, false);

    g.setGradientFill(face);
    g.fillEllipse(cx - knobRadius * 0.78f,
                  cy - knobRadius * 0.78f,
                  knobRadius * 1.56f,
                  knobRadius * 1.56f);

    // Reflejo superior: corregido para JUCE
    juce::Path reflectionArc;

    reflectionArc.addArc(
        cx - knobRadius * 0.72f,
        cy - knobRadius * 0.72f,
        knobRadius * 1.44f,
        knobRadius * 1.44f,
        rotaryStartAngle + 0.3f,
        rotaryStartAngle + 1.8f,
        true);

    g.setColour(juce::Colours::white.withAlpha(0.12f));
    g.strokePath(
        reflectionArc,
        juce::PathStrokeType(2.0f));

    // Marcas de escala
    for (int i = 0; i <= 20; ++i)
    {
        const float p = i / 20.0f;
        const float a = rotaryStartAngle
            + p * (rotaryEndAngle - rotaryStartAngle);

        const float r1 = radius * 0.91f;
        const float r2 = radius * (i % 5 == 0 ? 0.78f : 0.84f);

        juce::Point<float> p1(
            cx + std::cos(a) * r1,
            cy + std::sin(a) * r1);

        juce::Point<float> p2(
            cx + std::cos(a) * r2,
            cy + std::sin(a) * r2);

        g.setColour(i % 5 == 0
            ? juce::Colours::white.withAlpha(0.9f)
            : juce::Colours::white.withAlpha(0.35f));

        g.drawLine(p1.x, p1.y, p2.x, p2.y,
                   i % 5 == 0 ? 2.0f : 1.0f);
    }

    // Indicador blanco
    const float pointerRadius = knobRadius * 0.66f;

    juce::Point<float> pointer(
        cx + std::cos(angle) * pointerRadius,
        cy + std::sin(angle) * pointerRadius);

    g.setColour(juce::Colours::black.withAlpha(0.7f));

    g.drawLine(
        cx + std::cos(angle) * (pointerRadius - 1.0f),
        cy + std::sin(angle) * (pointerRadius - 1.0f),
        pointer.x + 1.0f, pointer.y + 2.0f, 6.0f);

    g.setColour(juce::Colours::white);

    g.drawLine(
        cx + std::cos(angle) * (pointerRadius - 1.0f),
        cy + std::sin(angle) * (pointerRadius - 1.0f),
        pointer.x, pointer.y, 4.0f);
}

LaPautaSLEditor::LaPautaSLEditor(
    LaPautaSLAudioProcessor& processor)
    : AudioProcessorEditor(&processor),
      audioProcessor(processor),
      thresholdAttachment(processor.parameters, "THRESHOLD",
                          thresholdSlider),
      ratioAttachment(processor.parameters, "RATIO",
                      ratioSlider),
      attackAttachment(processor.parameters, "ATTACK",
                       attackSlider),
      makeupAttachment(processor.parameters, "MAKEUP",
                       makeupSlider),
      mixAttachment(processor.parameters, "MIX",
                    mixSlider)
{
    setLookAndFeel(&lookAndFeel);

    setupKnob(thresholdSlider, "THRESHOLD");
    setupKnob(ratioSlider, "RATIO");
    setupKnob(attackSlider, "ATTACK");
    setupKnob(makeupSlider, "MAKEUP");
    setupKnob(mixSlider, "MIX");

    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(ratioSlider);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(makeupSlider);
    addAndMakeVisible(mixSlider);

    setResizable(false, false);
    setSize(1000, 360);

    startTimerHz(30);
}

LaPautaSLEditor::~LaPautaSLEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void LaPautaSLEditor::setupKnob(
    juce::Slider& slider,
    const juce::String& name)
{
    slider.setName(name);
    slider.setSliderStyle(
        juce::Slider::RotaryHorizontalVerticalDrag);

    slider.setTextBoxStyle(
        juce::Slider::NoTextBox, false, 0, 0);

    slider.setLookAndFeel(&lookAndFeel);
    slider.setMouseDragSensitivity(180);
    slider.setPopupDisplayEnabled(true, false, this);
}

void LaPautaSLEditor::timerCallback()
{
    displayedGR = audioProcessor.getGainReduction();
    repaint();
}

void LaPautaSLEditor::drawScrew(
    juce::Graphics& g, float x, float y)
{
    g.setColour(juce::Colour(0xff777e82));
    g.fillEllipse(x - 6.0f, y - 6.0f, 12.0f, 12.0f);

    g.setColour(juce::Colour(0xff25292c));
    g.drawEllipse(x - 6.0f, y - 6.0f, 12.0f, 12.0f, 1.2f);

    g.drawLine(x - 3.0f, y + 3.0f,
               x + 3.0f, y - 3.0f, 1.5f);
}

void LaPautaSLEditor::drawMeter(
    juce::Graphics& g,
    juce::Rectangle<float> area)
{
    g.setColour(juce::Colour(0xff080a0b));
    g.fillRoundedRectangle(area, 5.0f);

    g.setColour(juce::Colour(0xff60676b));
    g.drawRoundedRectangle(area, 5.0f, 1.0f);

    g.setColour(juce::Colour(0xffb7bdc0));
    g.setFont(juce::Font(13.0f, juce::Font::bold));

    g.drawText("GAIN REDUCTION",
        area.getX(), area.getY() - 24.0f,
        area.getWidth(), 18.0f,
        juce::Justification::centred);

    const float value = juce::jlimit(
        0.0f, 14.0f, displayedGR);

    for (int i = 0; i < 14; ++i)
    {
        const float segmentHeight = 9.0f;
        const float gap = 4.0f;

        const float y = area.getBottom() - 12.0f
            - (i + 1) * (segmentHeight + gap);

        juce::Colour segmentColour;

        if (i < 7)
            segmentColour = juce::Colour(0xff39d878);
        else if (i < 11)
            segmentColour = juce::Colour(0xffffc444);
        else
            segmentColour = juce::Colour(0xffff4d42);

        const bool active = value >= (i + 1);

        g.setColour(active
            ? segmentColour
            : segmentColour.withAlpha(0.13f));

        const float columnWidth =
            (area.getWidth() - 24.0f) / 2.0f;

        for (int column = 0; column < 2; ++column)
        {
            const float x = area.getX() + 8.0f
                + column * (columnWidth + 8.0f);

            g.fillRoundedRectangle(
                x, y, columnWidth, segmentHeight, 2.0f);
        }
    }

    g.setColour(amberColour);
    g.setFont(juce::Font(18.0f, juce::Font::bold));

    g.drawText(
        "-" + juce::String(displayedGR, 1) + " dB",
        area.getX() - 12.0f,
        area.getBottom() + 9.0f,
        area.getWidth() + 24.0f,
        25.0f,
        juce::Justification::centred);
}

void LaPautaSLEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    juce::ColourGradient background(
        juce::Colour(0xff303538), 0.0f, 0.0f,
        backgroundColour, 0.0f, bounds.getBottom(),
        false);

    g.setGradientFill(background);
    g.fillAll();

    // Textura metálica
    for (int y = 0; y < getHeight(); y += 3)
    {
        g.setColour(
            juce::Colours::white.withAlpha(
                (y % 9 == 0) ? 0.025f : 0.009f));

        g.drawHorizontalLine(
            y, 0.0f, static_cast<float>(getWidth()));
    }

    // Marco
    g.setColour(juce::Colour(0xff858c90));
    g.drawRoundedRectangle(bounds.reduced(4.0f), 8.0f, 1.5f);

    g.setColour(juce::Colour(0xff080a0c));
    g.drawRoundedRectangle(bounds.reduced(9.0f), 6.0f, 2.0f);

    // Tornillos
    drawScrew(g, 19.0f, 19.0f);
    drawScrew(g, 981.0f, 19.0f);
    drawScrew(g, 19.0f, 341.0f);
    drawScrew(g, 981.0f, 341.0f);

    // Cabecera
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(25.0f, juce::Font::bold));

    g.drawText("LA PAUTA",
        38, 18, 180, 31,
        juce::Justification::left);

    g.setColour(amberColour);
    g.setFont(juce::Font(21.0f, juce::Font::bold));

    g.drawText("SSL BUS COMPRESSOR",
        215, 19, 370, 30,
        juce::Justification::left);

    g.setColour(juce::Colour(0xffa5aaad));
    g.setFont(juce::Font(11.0f, juce::Font::bold));

    g.drawText("HYBRID EDITION / STEREO ANALOG MODEL",
        40, 49, 420, 17,
        juce::Justification::left);

    g.setColour(cyanColour);
    g.fillRect(40, 72, 920, 1);

    // Controles
    juce::Slider* sliders[] = {
        &thresholdSlider, &ratioSlider, &attackSlider,
        &makeupSlider, &mixSlider
    };

    const juce::String names[] = {
        "THRESHOLD", "RATIO", "ATTACK", "MAKEUP", "MIX"
    };

    const juce::String suffix[] = {
        " dB", ":1", " ms", " dB", " %"
    };

    for (int i = 0; i < 5; ++i)
    {
        const auto area = knobAreas[i].toFloat();

        g.setColour(juce::Colour(0xff080a0c));
        g.fillRoundedRectangle(
            area.getCentreX() - 48.0f,
            91.0f, 96.0f, 25.0f, 4.0f);

        g.setColour(juce::Colour(0xff666d71));
        g.drawRoundedRectangle(
            area.getCentreX() - 48.0f,
            91.0f, 96.0f, 25.0f, 4.0f, 1.0f);

        const double value = sliders[i]->getValue();

        juce::String display;

        if (i == 0 || i == 1 || i == 2)
            display = juce::String(value, 1);
        else if (i == 3)
            display = "+" + juce::String(value, 1);
        else
            display = juce::String(value, 0);

        g.setColour(amberColour);
        g.setFont(juce::Font(15.0f, juce::Font::bold));

        g.drawText(display + suffix[i],
            area.getCentreX() - 45.0f,
            94.0f, 90.0f, 19.0f,
            juce::Justification::centred);

        g.setColour(juce::Colour(0xffd7dadd));
        g.setFont(juce::Font(12.0f, juce::Font::bold));

        g.drawText(names[i],
            area.getX() - 8.0f,
            267.0f, area.getWidth() + 16.0f, 22.0f,
            juce::Justification::centred);
    }

    // Panel del medidor
    g.setColour(juce::Colour(0xff090b0d));
    g.fillRoundedRectangle(
        835.0f, 87.0f, 125.0f, 230.0f, 7.0f);

    g.setColour(juce::Colour(0xff777e82));
    g.drawRoundedRectangle(
        835.0f, 87.0f, 125.0f, 230.0f, 7.0f, 1.0f);

    drawMeter(g, { 855.0f, 130.0f, 85.0f, 145.0f });

    // Identificación
    g.setColour(juce::Colour(0xff777e82));
    g.setFont(juce::Font(9.0f, juce::Font::bold));

    g.drawText("LA PAUTA AUDIO | 2026",
        38, 328, 300, 15,
        juce::Justification::left);
}

void LaPautaSLEditor::resized()
{
    const int knobWidth = 116;
    const int knobHeight = 130;

    const int startX = 57;
    const int spacing = 153;

    juce::Slider* sliders[] = {
        &thresholdSlider, &ratioSlider, &attackSlider,
        &makeupSlider, &mixSlider
    };

    for (int i = 0; i < 5; ++i)
    {
        const int x = startX + i * spacing;

        knobAreas[i] = {
            x, 123, knobWidth, knobHeight
        };

        sliders[i]->setBounds(knobAreas[i]);
    }
}
