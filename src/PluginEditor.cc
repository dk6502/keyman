#include "PluginEditor.hh"

PluginEditor::PluginEditor(PluginProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p) {
  juce::ignoreUnused(processorRef);

  setSize(300, 300);
}

PluginEditor::~PluginEditor() {}

void PluginEditor::paint(juce::Graphics &g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  auto area = getLocalBounds();
  g.setColour(juce::Colours::white);
  g.setFont(16.0f);
  auto helloWorld = juce::String("Ojuce");
  g.drawText(helloWorld, area.removeFromTop(300), juce::Justification::centred,
             false);
}

void PluginEditor::resized() {
  // layout the positions of your child components here
  auto area = getLocalBounds();
  area.removeFromBottom(50);
}
