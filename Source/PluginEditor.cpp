#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LapautasslAudioProcessorEditor::LapautasslAudioProcessorEditor (LapautasslAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (400, 300);
}

LapautasslAudioProcessorEditor::~LapautasslAudioProcessorEditor()
{
}

//==============================================================================
void LapautasslAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("LAPAUTA SSL - LISTO", getLocalBounds(), juce::Justification::centred, 1);
}

void LapautasslAudioProcessorEditor::resized()
{
}
