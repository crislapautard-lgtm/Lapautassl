#include "PluginProcessor.h"
#include "PluginEditor.h"

LaPautaSLAudioProcessor::LaPautaSLAudioProcessor()
: AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo())
                                   .withOutput ("Output", juce::AudioChannelSet::stereo())),
  apvts (*this, nullptr, "Params", {
      std::make_unique<juce::AudioParameterFloat>("threshold", "Threshold", -24.0f, 0.0f, -6.0f),
      std::make_unique<juce::AudioParameterFloat>("release", "Release", 10.0f, 500.0f, 100.0f)
  })
{
}

LaPautaSLAudioProcessor::~LaPautaSLAudioProcessor() {}

void LaPautaSLAudioProcessor::prepareToPlay (double, int) {}
void LaPautaSLAudioProcessor::releaseResources() {}

void LaPautaSLAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto threshold = apvts.getRawParameterValue("threshold")->load();
    float threshLin = juce::Decibels::decibelsToGain(threshold);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float x = data[i];
            if (std::abs(x) > threshLin)
                x = (x > 0 ? threshLin : -threshLin);
            data[i] = x;
        }
    }
}

juce::AudioProcessorEditor* LaPautaSLAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LaPautaSLAudioProcessor();
}
