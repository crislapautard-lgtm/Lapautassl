#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class LaPautaLNF : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h, float pos, float start, float end, juce::Slider& s) override
    {
        auto bounds = juce::Rectangle<float>(x, y, w, h).reduced(8);
        g.setColour(juce::Colour(0xff1a1a1a));
        g.fillEllipse(bounds);
        g.setColour(juce::Colour(0xff3a3a3a));
        g.drawEllipse(bounds, 2.5f);
        
        auto angle = start + pos * (end - start);
        auto centre = bounds.getCentre();
        auto r = bounds.getWidth() * 0.42f;
        juce::Path p;
        p.addRectangle(-1.5f, -r, 3.0f, r * 0.65f);
        g.setColour(juce::Colours::white);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centre));
        g.fillPath(p);
        
        auto dotR = bounds.getWidth() * 0.07f;
        g.setColour(juce::Colour(0xff00aaff));
        g.fillEllipse(centre.x - dotR, centre.y - bounds.getHeight()*0.38f - dotR, dotR*2, dotR*2);
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
    LaPautaLNF laPautaLNF;

    juce::Slider thresholdSlider, ratioSlider, attackSlider, releaseSlider, makeupSlider, mixSlider;
    juce::Label thresholdLabel, ratioLabel, attackLabel, releaseLabel, makeupLabel, mixLabel;
    juce::Label thresholdVal, ratioVal, attackVal, releaseVal, makeupVal, mixVal;
    
    // VU
    float grLevel = 0.0f;
    float outLevel = 0.0f;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attThreshold, attRatio, attAttack, attRelease, attMakeup, attMix;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LaPautaSLAudioProcessorEditor)
};
