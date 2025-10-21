#pragma once

#include "PluginProcessor.hh"
#include "WaveThumbnail.hh"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <memory>

class PluginEditor : public juce::AudioProcessorEditor {
public:
  explicit PluginEditor(PluginProcessor &);
  ~PluginEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

  void loadWav();

private:
  PluginProcessor &processorRef;
  juce::Synthesiser sampler;
  WaveThumbnail waveThumbnail;

  juce::TextButton filePicker;
  std::unique_ptr<juce::FileChooser> wavChooser;

  juce::MidiKeyboardComponent keyboardComponent;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
