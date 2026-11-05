
#include "PluginEditor.h"

namespace
{
    const juce::Colour background(0xff141619);
    const juce::Colour panel(0xff22262a);
    const juce::Colour black(0xff080a0c);
    const juce::Colour gold(0xffffd18a);
    const juce::Colour turquoise(0xff42d6c5);
    const juce::Colour white(0xffeeeeee);
    const juce::Colour outline(0xff51575e);

    class LaPautaKnobLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawRotarySlider(
            juce::Graphics& g,
            int x,
            int y,
            int width,
            int height,
            float sliderPos,
            float startAngle,
            float endAngle,
            juce::Slider&) override
        {
            const float cx = x + width * 0.5f;
            const float cy = y + height * 0.43f;
            const float radius =
                juce::jmin(width, height) * 0.34f;

            const float angle =
                startAngle + sliderPos * (endAngle - startAngle);

            const float arcRadius = radius + 5.0f;

            // Cuerpo negro
            g.setColour(black);
            g.fillEllipse(
                cx - radius,
                cy - radius,
                radius * 2.0f,
                radius * 2.0f);

            // Borde metálico
            g.setColour(outline);
            g.drawEllipse(
                cx - radius,
                cy - radius,
                radius * 2.0f,
                radius * 2.0f,
                1.5f);

            // Anillo de fondo
            juce::Path backgroundArc;
            backgroundArc.addCentredArc(
                cx, cy,
                arcRadius, arcRadius,
                0.0f,
                startAngle,
                endAngle,
                true);

            g.setColour(juce::Colour(0xff454b50));
            g.strokePath(
                backgroundArc,
                juce::PathStrokeType(3.0f));

            // Anillo de posición turquesa
            juce::Path valueArc;
            valueArc.addCentredArc(
                cx, cy,
                arcRadius, arcRadius,
                0.0f,
                startAngle,
                angle,
                true);

            g.setColour(turquoise);
            g.strokePath(
                valueArc,
                juce::PathStrokeType(3.5f));

            // Centro de la perilla
            g.setColour(juce::Colour(0xff15191d));
            g.fillEllipse(
                cx - radius * 0.68f,
                cy - radius * 0.68f,
                radius * 1.36f,
                radius * 1.36f);

            // Indicador dorado
            juce::Path pointer;
            pointer.startNewSubPath(cx, cy);
            pointer.lineTo(
                cx + std::sin(angle) * radius * 0.73f,
                cy - std::cos(angle) * radius * 0.73f);

            g.setColour(gold);
            g.strokePath(
                pointer,
                juce::PathStrokeType(3.0f));

            // Punto central
            g.setColour(turquoise);
            g.fillEllipse(
                cx - 2.5f,
                cy - 2.5f,
                5.0f,
                5.0f);
        }
    };

    // Instancia permanente para mantener vivo el LookAndFeel.
    LaPautaKnobLookAndFeel knobLookAndFeel;
}

// ============================================================
// CONSTRUCTOR
// ============================================================

LaPautaSLEditor::LaPautaSLEditor(
    LaPautaSLCompresorAudioProcessor& processor)
    : AudioProcessorEditor(&processor),
      audioProcessor(processor)
{
    setSize(900, 460);

    configureSlider(
        thresholdSlider, thresholdLabel, "THRESHOLD");

    configureSlider(
        ratioSlider, ratioLabel, "RATIO");

    configureSlider(
        attackSlider, attackLabel, "ATTACK");

    configureSlider(
        releaseSlider, releaseLabel, "RELEASE");

    configureSlider(
        makeupSlider, makeupLabel, "MAKEUP GAIN");

    configureSlider(
        mixSlider, mixLabel, "DRY / WET");

    configureSlider(
        hpfSlider, hpfLabel, "SIDECHAIN HPF");

    // Conexiones de parámetros
    thresholdAttachment =
        std::make_unique<Attachment>(
            audioProcessor.parameters,
            "THRESHOLD",
            thresholdSlider);

    ratioAttachment =
        std::make_unique<Attachment>(
            audioProcessor.parameters,
            "RATIO",
            ratioSlider);

    attackAttachment =
        std::make_unique<Attachment>(
            audioProcessor.parameters,
            "ATTACK",
            attackSlider);

    releaseAttachment =
        std::make_unique<Attachment>(
            audioProcessor.parameters,
            "RELEASE",
            releaseSlider);

    makeupAttachment =
        std::make_unique<Attachment>(
            audioProcessor.parameters,
            "MAKEUP",
            makeupSlider);

    mixAttachment =
        std::make_unique<Attachment>(
            audioProcessor.parameters,
            "MIX",
            mixSlider);

    hpfAttachment =
        std::make_unique<Attachment>(
            audioProcessor.parameters,
            "HPF",
            hpfSlider);

    // Selector de presets
    presetLabel.setText(
        "SSL COMPRESSOR PRESETS",
        juce::dontSendNotification);

    presetLabel.setFont(
        juce::Font(12.0f, juce::Font::bold));

    presetLabel.setColour(
        juce::Label::textColourId,
        gold);

    presetLabel.setJustificationType(
        juce::Justification::centredRight);

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

    presetBox.setSelectedId(
        audioProcessor.getCurrentProgram() + 1,
        juce::dontSendNotification);

    presetBox.setColour(
        juce::ComboBox::backgroundColourId,
        panel);

    presetBox.setColour(
        juce::ComboBox::textColourId,
        white);

    presetBox.setColour(
        juce::ComboBox::outlineColourId,
        gold);

    presetBox.setColour(
        juce::ComboBox::arrowColourId,
        turquoise);

    presetBox.onChange = [this]
    {
        const int id = presetBox.getSelectedId();

        if (id >= 1 && id <= 9)
            audioProcessor.setCurrentProgram(id - 1);
    };

    addAndMakeVisible(presetBox);

    startTimerHz(10);
}

// ============================================================
// DESTRUCTOR
// ============================================================

LaPautaSLEditor::~LaPautaSLEditor()
{
    stopTimer();

    thresholdSlider.setLookAndFeel(nullptr);
    ratioSlider.setLookAndFeel(nullptr);
    attackSlider.setLookAndFeel(nullptr);
    releaseSlider.setLookAndFeel(nullptr);
    makeupSlider.setLookAndFeel(nullptr);
    mixSlider.setLookAndFeel(nullptr);
    hpfSlider.setLookAndFeel(nullptr);
}

// ============================================================
// CONFIGURACIÓN DE PERILLAS
// ============================================================

void LaPautaSLEditor::configureSlider(
    juce::Slider& slider,
    juce::Label& label,
    const juce::String& name)
{
    label.setText(
        name,
        juce::dontSendNotification);

    label.setFont(
        juce::Font(11.5f, juce::Font::bold));

    label.setColour(
        juce::Label::textColourId,
        gold);

    label.setJustificationType(
        juce::Justification::centred);

    addAndMakeVisible(label);

    slider.setSliderStyle(
        juce::Slider::RotaryHorizontalVerticalDrag);

    slider.setTextBoxStyle(
        juce::Slider::TextBoxBelow,
        false,
        78,
        20);

    slider.setLookAndFeel(&knobLookAndFeel);

    slider.setColour(
        juce::Slider::rotarySliderFillColourId,
        turquoise);

    slider.setColour(
        juce::Slider::rotarySliderOutlineColourId,
        outline);

    slider.setColour(
        juce::Slider::thumbColourId,
        gold);

    slider.setColour(
        juce::Slider::textBoxTextColourId,
        white);

    slider.setColour(
        juce::Slider::textBoxBackgroundColourId,
        black);

    slider.setColour(
        juce::Slider::textBoxOutlineColourId,
        outline);

    addAndMakeVisible(slider);
}

// ============================================================
// DISEÑO GRÁFICO
// ============================================================

void LaPautaSLEditor::paint(juce::Graphics& g)
{
    g.fillAll(background);

    // Cabecera
    g.setColour(juce::Colour(0xff0c0e10));
    g.fillRoundedRectangle(
        15.0f, 12.0f,
        870.0f, 67.0f,
        9.0f);

    g.setColour(gold);
    g.setFont(juce::Font(23.0f, juce::Font::bold));

    g.drawText(
        "LA PAUTA",
        30, 20, 230, 28,
        juce::Justification::centredLeft);

    g.setColour(white);
    g.setFont(juce::Font(14.0f, juce::Font::bold));

    g.drawText(
        "SL COMPRESSOR",
        31, 48, 230, 20,
        juce::Justification::centredLeft);

    // Panel principal
    g.setColour(panel);

    g.fillRoundedRectangle(
        15.0f, 90.0f,
        870.0f, 326.0f,
        10.0f);

    g.setColour(outline);

    g.drawRoundedRectangle(
        15.0f, 90.0f,
        870.0f, 326.0f,
        10.0f, 1.0f);

    // Separador entre filas
    g.setColour(white.withAlpha(0.08f));

    g.drawLine(
        35.0f, 252.0f,
        865.0f, 252.0f,
        1.0f);

    // Línea inferior
    g.setColour(turquoise);

    g.fillRoundedRectangle(
        30.0f, 426.0f,
        840.0f, 2.5f,
        1.0f);

    g.setColour(white.withAlpha(0.65f));
    g.setFont(juce::Font(9.5f));

    g.drawText(
        "LA PAUTA AUDIO  |  PROFESSIONAL DYNAMICS",
        20, 432, 860, 18,
        juce::Justification::centred);
}

// ============================================================
// DISTRIBUCIÓN COMPACTA
// ============================================================

void LaPautaSLEditor::resized()
{
    presetLabel.setBounds(
        440, 32, 190, 22);

    presetBox.setBounds(
        640, 27, 225, 30);

    juce::Slider* sliders[] =
    {
        &thresholdSlider,
        &ratioSlider,
        &attackSlider,
        &releaseSlider,
        &makeupSlider,
        &mixSlider,
        &hpfSlider
    };

    juce::Label* labels[] =
    {
        &thresholdLabel,
        &ratioLabel,
        &attackLabel,
        &releaseLabel,
        &makeupLabel,
        &mixLabel,
        &hpfLabel
    };

    // Cuatro controles arriba
    const int topX[] =
    {
        45, 255, 465, 675
    };

    // Tres controles abajo
    const int bottomX[] =
    {
        155, 365, 575
    };

    for (int i = 0; i < 7; ++i)
    {
        int x = 0;
        int y = 0;

        if (i < 4)
        {
            x = topX[i];
            y = 105;
        }
        else
        {
            x = bottomX[i - 4];
            y = 263;
        }

        labels[i]->setBounds(
            x, y,
            170, 20);

        sliders[i]->setBounds(
            x + 20, y + 18,
            130, 116);
    }
}

// ============================================================
// ACTUALIZACIÓN DE PRESETS
// ============================================================

void LaPautaSLEditor::timerCallback()
{
    const int program =
        audioProcessor.getCurrentProgram();

    if (program >= 0 && program < 9 &&
        presetBox.getSelectedId() != program + 1)
    {
        presetBox.setSelectedId(
            program + 1,
            juce::dontSendNotification);
    }
}
