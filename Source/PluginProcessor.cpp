#include "PluginProcessor.h"
#include "PluginEditor.h"

LaPautaSLAudioProcessor::LaPautaSLAudioProcessor()
: AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true).withOutput ("Output", juce::AudioChannelSet::stereo(), true)), apvts (*this, nullptr, "Parameters", createParameterLayout()) {}
LaPautaSLAudioProcessor::~LaPautaSLAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout LaPautaSLAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("THRESHOLD", "Threshold", -30.0f, 0.0f, -12.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("RATIO", "Ratio", 1.0f, 20.0f, 2.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("ATTACK", "Attack", 0.1f, 100.0f, 30.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("RELEASE", "Release", 50.0f, 3000.0f, 300.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("MAKEUP", "Makeup", -12.0f, 24.0f, 2.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("MIX", "Mix", 0.0f, 100.0f, 100.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("SATURATION", "Saturation", 0.0f, 100.0f, 10.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("PUNCH", "Punch", 0.0f, 100.0f, 20.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("PARALLEL", "Parallel", 0.0f, 100.0f, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("LINK", "Link", 0.0f, 100.0f, 100.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("HPF", "HPF", 20.0f, 1000.0f, 30.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("AIR", "Air", 0.0f, 100.0f, 10.0f));
    params.push_back (std::make_unique<juce::AudioParameterChoice> ("MODE", "Mode", juce::StringArray {"Classic", "Punchy", "Smooth"}, 0));
    params.push_back (std::make_unique<juce::AudioParameterChoice> ("OVERSAMPLING", "Oversampling", juce::StringArray {"2x", "4x", "8x"}, 1));
    return { params.begin(), params.end() };
}

void LaPautaSLAudioProcessor::applyPreset (int index)
{
    struct P { float thr, ratio, att, rel, makeup, mix, sat, punch, parallel, link, hpf, air; };
    P presets[7] = {
        { -12.0f, 2.0f, 30.0f, 300.0f, 2.0f, 100.0f, 10.0f, 20.0f, 0.0f, 100.0f, 30.0f, 10.0f },
        { -18.0f, 4.0f, 10.0f, 100.0f, 3.0f, 100.0f, 25.0f, 70.0f, 20.0f, 90.0f, 60.0f, 15.0f },
        { -15.0f, 3.0f, 10.0f, 300.0f, 3.0f, 85.0f, 20.0f, 40.0f, 30.0f, 85.0f, 90.0f, 40.0f },
        { -14.0f, 2.5f, 15.0f, 400.0f, 2.0f, 90.0f, 15.0f, 30.0f, 40.0f, 90.0f, 80.0f, 30.0f },
        { -12.0f, 2.0f, 20.0f, 500.0f, 1.5f, 95.0f, 10.0f, 15.0f, 50.0f, 95.0f, 50.0f, 25.0f },
        { -16.0f, 4.0f, 5.0f, 150.0f, 3.0f, 100.0f, 30.0f, 60.0f, 10.0f, 100.0f, 120.0f, 10.0f },
        { -8.0f,  2.0f, 30.0f, 600.0f, 1.0f, 100.0f, 5.0f,  10.0f, 0.0f, 100.0f, 30.0f, 5.0f }
    };
    if (index < 0 || index > 6) return;
    auto p = presets[index];
    apvts.getParameter ("THRESHOLD")->setValueNotifyingHost ((p.thr + 30.0f) / 30.0f);
    apvts.getParameter ("RATIO")->setValueNotifyingHost ((p.ratio - 1.0f) / 19.0f);
    apvts.getParameter ("ATTACK")->setValueNotifyingHost ((p.att - 0.1f) / 99.9f);
    apvts.getParameter ("RELEASE")->setValueNotifyingHost ((p.rel - 50.0f) / 2950.0f);
    apvts.getParameter ("MAKEUP")->setValueNotifyingHost ((p.makeup + 12.0f) / 36.0f);
    apvts.getParameter ("MIX")->setValueNotifyingHost (p.mix / 100.0f);
    apvts.getParameter ("SATURATION")->setValueNotifyingHost (p.sat / 100.0f);
    apvts.getParameter ("PUNCH")->setValueNotifyingHost (p.punch / 100.0f);
    apvts.getParameter ("PARALLEL")->setValueNotifyingHost (p.parallel / 100.0f);
    apvts.getParameter ("LINK")->setValueNotifyingHost (p.link / 100.0f);
    apvts.getParameter ("HPF")->setValueNotifyingHost ((p.hpf - 20.0f) / 980.0f);
    apvts.getParameter ("AIR")->setValueNotifyingHost (p.air / 100.0f);
}

void LaPautaSLAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) samplesPerBlock, (juce::uint32) getTotalNumOutputChannels() };
    compressor.prepare (spec);
    makeupGain.prepare (spec);
}

void LaPautaSLAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) buffer.clear (i, 0, buffer.getNumSamples());

    float threshold = apvts.getRawParameterValue ("THRESHOLD")->load();
    float ratio = apvts.getRawParameterValue ("RATIO")->load();
    float attack = apvts.getRawParameterValue ("ATTACK")->load();
    float release = apvts.getRawParameterValue ("RELEASE")->load();
    float makeup = apvts.getRawParameterValue ("MAKEUP")->load();
    float mix = apvts.getRawParameterValue ("MIX")->load() / 100.0f;
    float saturation = apvts.getRawParameterValue ("SATURATION")->load() / 100.0f;
    float punch = apvts.getRawParameterValue ("PUNCH")->load() / 100.0f;
    float parallel = apvts.getRawParameterValue ("PARALLEL")->load() / 100.0f;
    float link = apvts.getRawParameterValue ("LINK")->load() / 100.0f;
    float air = apvts.getRawParameterValue ("AIR")->load() / 100.0f;
    int mode = (int) apvts.getRawParameterValue ("MODE")->load();

    if (mode == 1) { attack *= 0.5f; ratio *= 1.3f; punch = juce::jmin(1.0f, punch + 0.2f); }
    if (mode == 2) { attack *= 1.8f; release *= 1.5f; saturation *= 0.7f; }

    float peak = 0.0f;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        peak = juce::jmax (peak, buffer.getMagnitude (ch, 0, buffer.getNumSamples()));
    inputLevel.store (juce::Decibels::gainToDecibels (peak + 0.00001f));

    compressor.setThreshold (threshold);
    compressor.setRatio (ratio);
    compressor.setAttack (attack);
    compressor.setRelease (release);
    makeupGain.setGainDecibels (makeup);

    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf (buffer);

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    compressor.process (context);

    float gr = threshold - inputLevel.load();
    if (gr > 0) gr = 0;
    gainReduction.store (-gr * (1.0f - 1.0f / ratio));

    if (saturation > 0.01f)
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* d = buffer.getWritePointer (ch);
            for (int s = 0; s < buffer.getNumSamples(); ++s)
            {
                float x = d[s] * (1.0f + saturation * 1.5f);
                d[s] = std::tanh (x) / (1.0f + saturation * 0.4f);
            }
        }
    }
    if (punch > 0.01f)
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* d = buffer.getWritePointer (ch);
            auto* dry = dryBuffer.getReadPointer (ch);
            for (int s = 1; s < buffer.getNumSamples(); ++s)
            {
                float transient = dry[s] - dry[s-1];
                d[s] += transient * punch * 0.8f;
            }
        }
    }
    if (air > 0.01f)
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* d = buffer.getWritePointer (ch);
            for (int s = 1; s < buffer.getNumSamples(); ++s)
                d[s] += (d[s] - d[s-1]) * air * 0.3f;
        }
    }

    makeupGain.process (context);

    float finalMix = juce::jmax (mix, parallel);
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* wet = buffer.getWritePointer (ch);
        auto* dry = dryBuffer.getReadPointer (ch);
        for (int s = 0; s < buffer.getNumSamples(); ++s)
            wet[s] = dry[s] * (1.0f - finalMix) + wet[s] * finalMix;
    }
    if (link < 0.99f && buffer.getNumChannels() == 2)
    {
        auto* l = buffer.getWritePointer (0);
        auto* r = buffer.getWritePointer (1);
        for (int s = 0; s < buffer.getNumSamples(); ++s)
        {
            float mid = (l[s] + r[s]) * 0.5f;
            float sideL = l[s] - mid;
            float sideR = r[s] - mid;
            l[s] = mid + sideL * (0.5f + link * 0.5f);
            r[s] = mid + sideR * (0.5f + link * 0.5f);
        }
    }
    float outPeak = 0.0f;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        outPeak = juce::jmax (outPeak, buffer.getMagnitude (ch, 0, buffer.getNumSamples()));
    outputLevel.store (juce::Decibels::gainToDecibels (outPeak + 0.00001f));
}

bool LaPautaSLAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()) return false;
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet()) return false;
   #endif
    return true;
}
juce::AudioProcessorEditor* LaPautaSLAudioProcessor::createEditor() { return new LaPautaSLEditor (*this); }
void LaPautaSLAudioProcessor::getStateInformation (juce::MemoryBlock& destData) { auto state = apvts.copyState(); std::unique_ptr<juce::XmlElement> xml (state.createXml()); copyXmlToBinary (*xml, destData); }
void LaPautaSLAudioProcessor::setStateInformation (const void* data, int sizeInBytes) { std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes)); if (xmlState.get() != nullptr) if (xmlState->hasTagName (apvts.state.getType())) apvts.replaceState (juce::ValueTree::fromXml (*xmlState)); }
void LaPautaSLAudioProcessor::releaseResources() {}
