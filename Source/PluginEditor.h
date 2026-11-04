#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class ProLNF : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h, float pos, float start, float end, juce::Slider& s) override
    {
        auto bounds = juce::Rectangle<float>(x, y, w, h).reduced(4);
        auto centre = bounds.getCentre();
        float radius = bounds.getWidth() * 0.5f;
        g.setColour(juce::Colours::black.withAlpha(0.6f));
        g.fillEllipse(bounds.translated(0, 3));
        juce::ColourGradient grad(juce::Colour(0xff4a4a4a), centre.x, centre.y - radius, juce::Colour(0xff151515), centre.x, centre.y + radius, false);
        g.setGradientFill(grad);
        g.fillEllipse(bounds);
        g.setColour(juce::Colour(0xff7a7a7a));
        g.drawEllipse(bounds, 1.5f);
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        for (int i = 0; i <= 10; ++i)
        {
            float a = start + (float)i / 10.0f * (end - start);
            auto p1 = centre.getPointOnCircumference(radius + 2, a);
            auto p2 = centre.getPointOnCircumference(radius + 7, a);
            g.drawLine(p1.x, p1.y, p2.x, p2.y, 1.2f);
        }
        auto angle = start + pos * (end - start);
        juce::Path p;
        p.addRectangle(-1.5f, -radius * 0.85f, 3.0f, radius * 0.65f);
        g.setColour(juce::Colours::white);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centre));
        g.fillPath(p);
        g.setColour(juce::Colour(0xff222222));
        g.fillEllipse(centre.x - 4, centre.y - 4, 8, 8);
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
    ProLNF proLNF;
    juce::Slider thresholdSlider, ratioSlider, attackSlider, makeupSlider, mixSlider;
    juce::Label thresholdLabel, ratioLabel, attackLabel, makeupLabel, mixLabel;
    juce::Label thresholdVal, ratioVal, attackVal, makeupVal, mixVal;
    float grLevel = 0.0f;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attThreshold, attRatio, attAttack, attMakeup, attMix;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LaPautaSLAudioProcessorEditor)
};
