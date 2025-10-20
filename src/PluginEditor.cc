#include "PluginEditor.hh"
#include "WaveThumbnail.hh"
#include "juce_core/juce_core.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <algorithm>
#include <memory>

PluginEditor::PluginEditor(PluginProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p), waveThumbnail(p) {
  juce::ignoreUnused(processorRef);

  setSize(300, 300);

  addAndMakeVisible(waveThumbnail);
  addAndMakeVisible(filePicker);
  filePicker.onClick = [this] { loadWav(); };
}

PluginEditor::~PluginEditor() {
}

void PluginEditor::paint(juce::Graphics &g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized() {
  // layout the positions of your child components here
  filePicker.setBounds(getWidth()-40,5,20,10);
  waveThumbnail.setBounds(0, 20, getWidth(), getHeight()-150);
}

void PluginEditor::loadWav() {
    wavChooser = std::make_unique<juce::FileChooser> ("Please select the file you want to load...", juce::File::getSpecialLocation(juce::File::userHomeDirectory), "*wav");
    auto chooserFlags = juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::openMode;
    wavChooser->launchAsync(chooserFlags, [this] (const juce::FileChooser& chooser)
        {
            auto name = chooser.getResult().getFullPathName();
            processorRef.loadFile(juce::String(name));
            waveThumbnail.repaint();
        });
}
