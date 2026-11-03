#include <JuceHeader.h>
// Editor dummy - La Pauta usa solo el sonido, sin ventana
class LaPautaEditor : public juce::AudioProcessorEditor {
public:
    LaPautaEditor(juce::AudioProcessor& p) : AudioProcessorEditor(p) { setSize(1, 1); }
    void paint(juce::Graphics&) override {}
    void resized() override {}
};
juce::AudioProcessorEditor* LaPautaProcessor::createEditor() { return new LaPautaEditor(*this); }
