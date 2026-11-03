// trigger 37
//LaPautaSL - V2 ACTUALIZADO - Nov 2026
// Ya corregido: Ratio Float, DrawImage, Presets nuevos
#include "PluginProcessor.h"
#include "PluginEditor.h"

LaPautaSLAudioProcessor::LaPautaSLAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

LaPautaSLAudioProcessor::~LaPautaSLAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout LaPautaSLAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    // NUEVO: Rangos mas musicales, estilo SSL
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "threshold", "Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -18.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "ratio", "Ratio",
        juce::NormalisableRange<float>(1.0f, 10.0f, 0.1f), 4.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "attack", "Attack",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.5f), 10.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "release", "Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.5f), 150.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "makeup", "Makeup",
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "mix", "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 100.0f));
    return { params.begin(), params.end() };
}

void LaPautaSLAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec{ sampleRate, (juce::uint32)samplesPerBlock, 2 };
    compressor.prepare(spec);
    makeupGain.prepare(spec);
    dryWet.prepare(spec);
    dryWet.setWetMixProportion(1.0f);
}

void LaPautaSLAudioProcessor::releaseResources() {}

void LaPautaSLAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    float th = *apvts.getRawParameterValue("threshold");
    float ratioVal = *apvts.getRawParameterValue("ratio");
    float at = *apvts.getRawParameterValue("attack");
    float rel = *apvts.getRawParameterValue("release");
    float mk = *apvts.getRawParameterValue("makeup");
    float mx = *apvts.getRawParameterValue("mix");

    compressor.setThreshold(th);
    compressor.setRatio(ratioVal);
    compressor.setAttack(at);
    compressor.setRelease(rel);
    makeupGain.setGainDecibels(mk);
    dryWet.setWetMixProportion(mx / 100.0f);

    juce::dsp::AudioBlock<float> block(buffer);
    dryWet.pushDrySamples(block);
    compressor.process(juce::dsp::ProcessContextReplacing<float>(block));
    makeupGain.process(juce::dsp::ProcessContextReplacing<float>(block));
    dryWet.mixWetSamples(block);
}

juce::AudioProcessorEditor* LaPautaSLAudioProcessor::createEditor()
{
    return new LaPautaSLAudioProcessorEditor(*this);
}

// PRESETS NUEVOS ACTUALIZADOS
struct Preset { const char* name; float t; float r; float a; float rel; float m; float mix; };
static const Preset presets[8] = {
    {"1. Vocal Suave",    -18.0f, 2.0f, 10.0f, 150.0f, 2.0f, 100.0f},
    {"2. Vocal Fuerte",   -12.0f, 3.0f,  5.0f, 100.0f, 3.0f, 100.0f},
    {"3. Bateria Pegada", -10.0f, 4.0f,  2.0f,  80.0f, 4.0f, 100.0f},
    {"4. Bajo Controlado",-14.0f, 2.5f, 15.0f, 200.0f, 2.0f, 100.0f},
    {"5. Master Suave",   -6.0f,  1.5f, 20.0f, 300.0f, 1.0f, 100.0f},
    {"6. Master Fuerte",  -10.0f, 2.0f, 10.0f, 200.0f, 2.0f, 100.0f},
    {"7. Guitarra",       -16.0f, 2.0f,  8.0f, 180.0f, 2.0f, 100.0f},
    {"8. Paralelo NY",    -20.0f, 4.0f,  5.0f, 120.0f, 6.0f,  50.0f}
};

void LaPautaSLAudioProcessor::applyPreset(int index)
{
    if (index < 0 || index >= 8) return;
    currentPreset = index;
    auto p = presets[index];
    auto* th = apvts.getParameter("threshold");
    auto* ra = apvts.getParameter("ratio");
    auto* at = apvts.getParameter("attack");
    auto* rl = apvts.getParameter("release");
    auto* mk = apvts.getParameter("makeup");
    auto* mx = apvts.getParameter("mix");
    if (th) th->setValueNotifyingHost(th->convertTo0to1(p.t));
    if (ra) ra->setValueNotifyingHost(ra->convertTo0to1(p.r));
    if (at) at->setValueNotifyingHost(at->convertTo0to1(p.a));
    if (rl) rl->setValueNotifyingHost(rl->convertTo0to1(p.rel));
    if (mk) mk->setValueNotifyingHost(mk->convertTo0to1(p.m));
    if (mx) mx->setValueNotifyingHost(mx->convertTo0to1(p.mix));
}

void LaPautaSLAudioProcessor::setCurrentProgram(int index) { applyPreset(index); }

const juce::String LaPautaSLAudioProcessor::getProgramName(int index)
{
    if (index >= 0 && index < 8) return presets[index].name;
    return "Default";
}

void LaPautaSLAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void LaPautaSLAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LaPautaSLAudioProcessor();
}
