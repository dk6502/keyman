#pragma once

#include "PluginProcessor.hh"
#include "WaveThumbnail.hh"
#include "juce_audio_basics/juce_audio_basics.h"

class PluginEditor : public juce::AudioProcessorEditor {
public:
  explicit PluginEditor(PluginProcessor &);
  ~PluginEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  PluginProcessor &processorRef;
  juce::Synthesiser sampler;
  WaveThumbnail waveThumbnail;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
