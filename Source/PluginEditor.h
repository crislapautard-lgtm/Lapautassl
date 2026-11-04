#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

// LOOK AND FEEL PRO - PERILLAS NEGRAS SSL
class LaPautaLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto centre = bounds.getCentre();
        
        // Sombra
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.fillEllipse(centre.x - radius + 2, centre.y - radius + 2, radius*2, radius*2);
        
        // Perilla negra carbon
        g.setColour(juce::Colour(0xff1e1e1e));
        g.fillEllipse(centre.x - radius, centre.y - radius, radius*2, radius*2);
        
        // Borde metalico
        g.setColour(juce::Colour(0xff3a3a3a));
        g.drawEllipse(centre.x - radius, centre.y - radius, radius*2, radius*2, 2.0f);
        g.setColour(juce::Colour(0xff5a5a5a));
        g.drawEllipse(centre.x - radius + 1, centre.y - radius + 1, radius*2 - 2, radius*2 - 2, 1.0f);
        
        // Linea blanca indicadora
        juce::Path p;
        auto pointerLength = radius * 0.7f;
        auto pointerThickness = 3.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(toAngle).translated(centre.x, centre.y));
        g.setColour(juce::Colours::white);
        g.fillPath(p);
        
        // Punto central azul La Pauta
        g.setColour(juce::Colour(0xff00aaff));
        g.fillEllipse(centre.x - 3, centre.y - 3, 6, 6);
    }
};

class LaPautaSLAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    LaPautaSLAudioProcessorEditor (LaPautaSLAudioProcessor&);
    ~LaPautaSLAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
private:
    LaPautaSLAudioProcessor& audioProcessor;
    LaPautaLookAndFeel laPautaLNF;
    
    juce::Slider thresholdSlider, ratioSlider, attackSlider, releaseSlider, makeupSlider, mixSlider;
    juce::Label thresholdLabel, ratioLabel, attackLabel, releaseLabel, makeupLabel, mixLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LaPautaSLAudioProcessorEditor)
};
