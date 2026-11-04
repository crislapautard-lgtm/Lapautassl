#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LaPautaSLAudioProcessorEditor::LaPautaSLAudioProcessorEditor (LaPautaSLAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (900, 500);
    startTimerHz(30);
}

LaPautaSLAudioProcessorEditor::~LaPautaSLAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void LaPautaSLAudioProcessorEditor::paint (juce::Graphics& g)
{
    // FONDO PRO - NEGRO CARBON SSL ORIGINAL
    g.fillAll(juce::Colour(0xff151515));
    
    auto bounds = getLocalBounds().toFloat();
    
    // Degradado sutil de arriba a abajo
    juce::ColourGradient grad(juce::Colour(0xff252525), 0, 0,
                              juce::Colour(0xff0a0a0a), 0, (float)getHeight(), false);
    g.setGradientFill(grad);
    g.fillRect(bounds);
    
    // BORDE EXTERIOR METALICO
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawRoundedRectangle(bounds.reduced(2), 6.0f, 2.0f);
    
    g.setColour(juce::Colour(0xff1a1a1a));
    g.drawRoundedRectangle(bounds.reduced(4), 4.0f, 1.0f);
    
    // HEADER
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRect(0, 0, getWidth(), 65);
    
    g.setColour(juce::Colour(0xff2a2a2a));
    g.drawLine(0, 65, (float)getWidth(), 65, 2.0f);
    
    // TITULO LA PAUTA - BLANCO SSL
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText("La Pauta", 25, 10, 200, 30, juce::Justification::left);
    
    g.setColour(juce::Colour(0xff00ccff));
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText("SSL", 150, 10, 100, 30, juce::Justification::left);
    
    // SUBTITULO
    g.setColour(juce::Colour(0xffaaaaaa));
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.drawText("BUS COMPRESSOR  |  HYBRID EDITION  |  CRIS LA PAUTA RD", 25, 38, 500, 15, juce::Justification::left);
    
    // COPYRIGHT ARREGLADO
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.setFont(juce::Font(10.0f));
    g.drawText(juce::String::fromUTF8("© 2026 Cris La Pauta Audio Solutions"),
               getWidth() - 280, 15, 260, 15, juce::Justification::right);
    
    // SECCION DE METERS FAKE BONITOS - EJEMPLO VISUAL
    // Area izquierda - meters negros con verde
    auto meterBounds = juce::Rectangle<int>(25, 90, 200, 15).toFloat();
    
    g.setColour(juce::Colour(0xff000000));
    g.fillRoundedRectangle(meterBounds, 2.0f);
    g.setColour(juce::Colour(0xff333333));
    g.drawRoundedRectangle(meterBounds, 2.0f, 1.0f);
    
    // Barra verde
    g.setColour(juce::Colour(0xff00ff66));
    g.fillRoundedRectangle(meterBounds.removeFromLeft(meterBounds.getWidth() * 0.65f), 2.0f);
    
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("GAIN REDUCTION", 25, 75, 150, 12, juce::Justification::left);
}

void LaPautaSLAudioProcessorEditor::resized()
{
}

void LaPautaSLAudioProcessorEditor::timerCallback()
{
    repaint();
}
