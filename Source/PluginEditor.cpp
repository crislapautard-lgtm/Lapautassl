#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LaPautaSLAudioProcessorEditor::LaPautaSLAudioProcessorEditor (LaPautaSLAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (800, 400);
}

LaPautaSLAudioProcessorEditor::~LaPautaSLAudioProcessorEditor()
{
}

//==============================================================================
void LaPautaSLAudioProcessorEditor::paint (juce::Graphics& g)
{
    // FONDO MODERNO LA PAUTA - Degradado azul oscuro a negro
    g.fillAll (juce::Colour(0xff0a0e1a));
    
    auto bounds = getLocalBounds().toFloat();
    juce::ColourGradient gradient(juce::Colour(0xff111a33), 0, 0,
                                  juce::Colour(0xff05070f), 0, (float)getHeight(), false);
    g.setGradientFill(gradient);
    g.fillRect(bounds);
    
    // BORDE AZUL ELECTRICO
    g.setColour(juce::Colour(0xff00aaff).withAlpha(0.3f));
    g.drawRoundedRectangle(bounds.reduced(1), 8.0f, 1.5f);
    
    // TITULO
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(22.0f, juce::Font::bold));
    g.drawText("LA PAUTA SSL COMPRESSOR", 20, 10, 400, 30, juce::Justification::left);
    
    g.setColour(juce::Colour(0xff00aaff));
    g.setFont(juce::Font(12.0f, juce::Font::bold));
    g.drawText("HYBRID EDITION", 20, 35, 200, 15, juce::Justification::left);
    
    // COPYRIGHT - ARREGLADO
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.setFont(juce::Font(10.0f));
    g.drawText("CRIS LA PAUTA RD  " + juce::String::fromUTF8("© 2026") + "  AUDIO SOLUTIONS",
               getWidth() - 300, 10, 280, 15, juce::Justification::right);
}

void LaPautaSLAudioProcessorEditor::resized()
{
}
