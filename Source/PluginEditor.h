#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

struct LaPautaLook : juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos, float start, float end, juce::Slider&) override
    {
        auto b = juce::Rectangle<float>(x, y, w, h).reduced(5);
        auto c = b.getCentre();
        auto r = juce::jmin(b.getWidth(), b.getHeight()) / 2.0f;
        // Anillo azul neon como en la foto
        g.setColour(juce::Colour(0xff00aaff));
        g.drawEllipse(b, 4.0f);
        g.setColour(juce::Colour(0xff0033ff));
        g.drawEllipse(b.reduced(2), 1.0f);
        // Metal plateado
        juce::ColourGradient grad(juce::Colours::white, c.x, c.y - r, juce::Colour(0xff3a3a3a), c.x, c.y + r, false);
        g.setGradientFill(grad);
        g.fillEllipse(b.reduced(7));
        g.setColour(juce::Colours::black);
        g.drawEllipse(b.reduced(7), 1.0f);
        // Puntero
        auto angle = start + pos * (end - start);
        auto p1 = c.getPointOnCircumference(r - 14, angle);
        g.setColour(juce::Colour(0xff00ccff));
        g.drawLine(juce::Line<float>(c, p1), 4.0f);
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
    LaPautaLook lnf;
    juce::Slider thresh, ratio, attack, release, makeup, mix, sidechain;
    juce::Label lThresh, lRatio, lAttack, lRelease, lMakeup, lMix, lSidechain, lTitle, lSub;
    juce::TextButton sidechainOn { "ON" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> a1,a2,a3,a4,a5,a6;
    void setup(juce::Slider& s, juce::Label& l, juce::String t);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LaPautaSLAudioProcessorEditor)
};
