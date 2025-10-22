#include "PluginEditor.hh"
#include "WaveThumbnail.hh"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_core/juce_core.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <algorithm>
#include <memory>

PluginEditor::PluginEditor(PluginProcessor &p, juce::AudioProcessorValueTreeState &vts)
    : AudioProcessorEditor(&p), processorRef(p), waveThumbnail(p),
      keyboardComponent(p.keyboardState,
                        juce::MidiKeyboardComponent::horizontalKeyboard) {
  juce::ignoreUnused(processorRef);

  addAndMakeVisible(waveThumbnail);
  addAndMakeVisible(filePicker);
  filePicker.setButtonText("Load File");
  filePicker.onClick = [this] { loadWav(); };

  grainSizeSliderAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(vts, "size", grainSizeSlider));
  grainSizeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalDrag);
  addAndMakeVisible(grainSizeSlider);

  addAndMakeVisible(keyboardComponent);
  setSize(600, 450);
}

PluginEditor::~PluginEditor() {}

void PluginEditor::paint(juce::Graphics &g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized() {
  // layout the positions of your child components here
  filePicker.setBoundsRelative(0.8, 0.02, 0.1, 0.1);
  waveThumbnail.setBoundsRelative(0.0, 0.2, 1.0, 0.4);
  keyboardComponent.setBoundsRelative(0.0, 0.85, 1.0, 0.15);
  grainSizeSlider.setBoundsRelative(0.1, 0.6, 0.2, 0.2);
}

void PluginEditor::loadWav() {
  wavChooser = std::make_unique<juce::FileChooser>(
      "Please select the file you want to load...",
      juce::File::getSpecialLocation(juce::File::userHomeDirectory), "*wav");
  auto chooserFlags = juce::FileBrowserComponent::canSelectFiles |
                      juce::FileBrowserComponent::openMode;
  wavChooser->launchAsync(chooserFlags,
                          [this](const juce::FileChooser &chooser) {
                            auto name = chooser.getResult().getFullPathName();
                            processorRef.loadFile(juce::String(name));
                            waveThumbnail.repaint();
                          });
}
