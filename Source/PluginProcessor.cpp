#include "PluginProcessor.h"
#include "PluginEditor.h"

LaPautaSLAudioProcessor::LaPautaSLAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

LaPautaSLAudioProcessor::~LaPautaSLAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout LaPautaSLAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("threshold", "Threshold", juce::NormalisableRange<float>(-30.0f, 0.0f), -10.0f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("ratio", "Ratio", juce::StringArray{"2", "4", "10", "20"}, 1));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("attack", "Attack", juce::StringArray{"0.1 ms", "0.3 ms", "1 ms", "3 ms", "10 ms", "30 ms"}, 4));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("release", "Release", juce::StringArray{"0.1 s", "0.3 s", "0.6 s", "1.2 s", "Auto"}, 1));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("makeup", "Makeup", juce::NormalisableRange<float>(0.0f, 20.0f), 2.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", juce::NormalisableRange<float>(0.0f, 100.0f), 100.0f));
    return { params.begin(), params.end() };
}

void LaPautaSLAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec{ sampleRate, (juce::uint32)samplesPerBlock, 2 };
    compressor.prepare(spec);
    makeupGain.prepare(spec);
    dryWet.prepare(spec);
    dryWet.setMixingRule(juce::dsp::DryWetMixingRule::balanced);
}

void LaPautaSLAudioProcessor::releaseResources() {}

void LaPautaSLAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    float thresh = apvts.getRawParameterValue("threshold")->load();
    int ratioIdx = (int)apvts.getRawParameterValue("ratio")->load();
    int attIdx = (int)apvts.getRawParameterValue("attack")->load();
    int relIdx = (int)apvts.getRawParameterValue("release")->load();
    float makeup = apvts.getRawParameterValue("makeup")->load();
    float mix = apvts.getRawParameterValue("mix")->load();

    float ratios[] = { 2.0f, 4.0f, 10.0f, 20.0f };
    float attacks[] = { 0.0001f, 0.0003f, 0.001f, 0.003f, 0.01f, 0.03f };
    float releases[] = { 0.1f, 0.3f, 0.6f, 1.2f, 0.4f };

    compressor.setThreshold(thresh);
    compressor.setRatio(ratios[ratioIdx]);
    compressor.setAttack(attacks[attIdx]);
    compressor.setRelease(releases[relIdx]);
    makeupGain.setGainDecibels(makeup);
    dryWet.setWetMixProportion(mix / 100.0f);

    juce::dsp::AudioBlock<float> block(buffer);
    dryWet.pushDrySamples(block);
    juce::dsp::ProcessContextReplacing<float> ctx(block);
    compressor.process(ctx);
    makeupGain.process(ctx);
    dryWet.mixWetSamples(block);
}

// ===== PRESETS DEL SSL NORMAL =====
void LaPautaSLAudioProcessor::setCurrentProgram(int index)
{
    applyPreset(index);
    updateHostDisplay();
}

const juce::String LaPautaSLAudioProcessor::getProgramName(int index)
{
    juce::StringArray names{ "2-Mix Glue", "Drum Bus Punch", "Gentle Mix", "Vocal Glue", "Mastering Transparent", "Punchy Drums", "Acoustic Guitar", "Brickwall" };
    if (index < 0 || index >= names.size()) return {};
    return names[index];
}

void LaPautaSLAudioProcessor::applyPreset(int index)
{
    struct P { float t; int r; int a; int rel; float m; float mix; };
    P presets[8] = {
        { -10.0f, 1, 4, 1, 2.0f, 100.0f }, // 2-Mix Glue
        { -15.0f, 1, 3, 0, 3.0f, 100.0f }, // Drum Bus Punch
        { -5.0f,  0, 5, 2, 1.0f, 100.0f }, // Gentle Mix
        { -8.0f,  0, 4, 4, 2.0f, 100.0f }, // Vocal Glue
        { -3.0f,  0, 5, 4, 0.5f, 100.0f }, // Mastering Transparent
        { -12.0f, 2, 2, 0, 4.0f, 100.0f }, // Punchy Drums
        { -12.0f, 1, 4, 1, 2.0f, 100.0f }, // Acoustic Guitar
        { -10.0f, 3, 1, 0, 5.0f, 100.0f }, // Brickwall
    };
    if (index < 0 || index >= 8) return;
    auto p = presets[index];
    auto* th = apvts.getParameter("threshold");
    auto* mk = apvts.getParameter("makeup");
    auto* mx = apvts.getParameter("mix");
    th->setValueNotifyingHost(th->convertTo0to1(p.t));
    apvts.getParameter("ratio")->setValueNotifyingHost((float)p.r / 3.0f);
    apvts.getParameter("attack")->setValueNotifyingHost((float)p.a / 5.0f);
    apvts.getParameter("release")->setValueNotifyingHost((float)p.rel / 4.0f);
    mk->setValueNotifyingHost(mk->convertTo0to1(p.m));
    mx->setValueNotifyingHost(mx->convertTo0to1(p.mix));
    currentPreset = index;
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
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorEditor* LaPautaSLAudioProcessor::createEditor()
{
    return new LaPautaSLAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LaPautaSLAudioProcessor();
}
