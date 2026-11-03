#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class LaPautaSLEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    LaPautaSLEditor (LaPautaSLAudioProcessor&);
    ~LaPautaSLEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
private:
    LaPautaSLAudioProcessor& audioProcessor;
    juce::Slider thr, ratio, att, rel, makeup, mix, sat, punch, parallel, link, hpf, air;
    juce::Label lThr, lRatio, lAtt, lRel, lMakeup, lMix, lSat, lPunch, lParallel, lLink, lHpf, lAir;
    juce::TextButton modeClassic, modePunchy, modeSmooth, over2x, over4x, over8x, sidechainBtn;
    juce::ComboBox presetBox;
    juce::Label presetLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> aThr, aRatio, aAtt, aRel, aMakeup, aMix, aSat, aPunch, aParallel, aLink, aHpf, aAir;
    void setupKnob (juce::Slider& s, juce::Label& l, const juce::String& name, const juce::String& id, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& a);
    void drawVUMeter (juce::Graphics& g, int x, int y, int w, int h, float db, bool isGR);
    void drawLEDBar (juce::Graphics& g, int x, int y, int w, int h, float db);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LaPautaSLEditor)
};
