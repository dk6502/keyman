#include "PluginEditor.hh"
#include "WaveThumbnail.hh"

PluginEditor::PluginEditor(PluginProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p), waveThumbnail(p) {
  juce::ignoreUnused(processorRef);

  setSize(300, 300);

  addAndMakeVisible(waveThumbnail);
}

PluginEditor::~PluginEditor() {}

void PluginEditor::paint(juce::Graphics &g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized() {
  // layout the positions of your child components here
  waveThumbnail.setBounds(0, 20, getWidth(), getHeight()-150);
}
