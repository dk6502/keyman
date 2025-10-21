#pragma once

#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_formats/juce_audio_formats.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/juce_core.h"
#include <atomic>

class PluginProcessor : public juce::AudioProcessor {
public:
  PluginProcessor();
  ~PluginProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override;

  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;

  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  int getNumSamplerSounds() { return sampler.getNumSounds(); };
  juce::AudioBuffer<float> &getWaveForm() const;

  void loadFile(const juce::String &path);

  juce::MidiKeyboardState keyboardState;

private:
  juce::Synthesiser sampler;
  const int numVoices{3};
  juce::AudioFormatManager formatManager;

  juce::AudioProcessorValueTreeState params;
  juce::AudioProcessorValueTreeState::ParameterLayout parameterLayout();

  std::atomic<bool> isNotePlayed{false};
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
