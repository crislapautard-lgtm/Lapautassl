#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
struct ProLook : juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos, float start, float end, juce::Slider&) override
    {
        auto b = juce::Rectangle<float>(x, y, w, h).reduced(6);
        auto c = b.getCentre();
        float r = juce::jmin(b.getWidth(), b.getHeight()) / 2.0f;
        // Sombra
        g.setColour(juce::Colour(0x55000000));
        g.fillEllipse(b.translated(0, 3));
        // Marcas alrededor
        g.setColour(juce::Colour(0xff666666));
        for (int i = 0; i <= 10; ++i) {
            float a = start + (float)i / 10.0f * (end - start);
            auto p1 = c.getPointOnCircumference(r + 2, a);
            auto p2 = c.getPointOnCircumference(r - 4, a);
            g.drawLine(juce::Line<float>(p1, p2), 1.5f);
        }
        // Cuerpo negro premium
        juce::ColourGradient grad(juce::Colour(0xff3d3d3d), c.x, c.y - r, juce::Colour(0xff0a0a0a), c.x, c.y + r, false);
        g.setGradientFill(grad);
        g.fillEllipse(b);
        g.setColour(juce::Colour(0xff00e5ff));
        g.drawEllipse(b, 2.0f);
        // Brillo arriba
        g.setColour(juce::Colour(0x44ffffff));
        g.fillEllipse(b.reduced(r * 0.45f).translated(0, -r * 0.25f));
        // Aguja blanca con punta cyan
        float angle = start + pos * (end - start);
        auto tip = c.getPointOnCircumference(r - 12, angle);
        g.setColour(juce::Colours::white);
        g.drawLine(juce::Line<float>(c, tip), 4.0f);
        g.setColour(juce::Colour(0xff00e5ff));
        g.fillEllipse(tip.x - 4, tip.y - 4, 8, 8);
        // Centro
        g.setColour(juce::Colour(0xff111111));
        g.fillEllipse(c.x - 10, c.y - 10, 20, 20);
        g.setColour(juce::Colour(0xff444444));
        g.drawEllipse(c.x - 10, c.y - 10, 20, 20, 1.0f);
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
    ProLook lnf;
    juce::Slider thresh, ratio, attack, release, makeup, mix, sidechain;
    juce::Label lThresh, lRatio, lAttack, lRelease, lMakeup, lMix, lSidechain, lTitle, lSub, lFooter;
    juce::ToggleButton sidechainOn;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> a1,a2,a3,a4,a5,a6,a7;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> b1;
    float smoothIn = -60, smoothOut = -60, smoothGR = 0;
    void setup(juce::Slider& s, juce::Label& l, juce::String t, juce::String suffix);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LaPautaSLAudioProcessorEditor)
};
