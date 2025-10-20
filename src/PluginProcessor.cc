#include "PluginProcessor.hh"
#include "PluginEditor.hh"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_formats/juce_audio_formats.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/juce_core.h"
#include <memory>

PluginProcessor::PluginProcessor()
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      params(*this, nullptr, "Params", parameterLayout()) {
  formatManager.registerBasicFormats();
  for (int i = 0; i < numVoices; i++) {
    sampler.addVoice(new juce::SamplerVoice);
  }
}

PluginProcessor::~PluginProcessor() {}

const juce::String PluginProcessor::getName() const { return JucePlugin_Name; }

bool PluginProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool PluginProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool PluginProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const { return 0.0; }

int PluginProcessor::getNumPrograms() { return 1; }

int PluginProcessor::getCurrentProgram() { return 0; }

void PluginProcessor::setCurrentProgram(int index) {
  juce::ignoreUnused(index);
}

const juce::String PluginProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);
  return {};
}

void PluginProcessor::changeProgramName(int index,
                                        const juce::String &newName) {
  juce::ignoreUnused(index, newName);
}

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
  juce::ignoreUnused(sampleRate, samplesPerBlock);
  sampler.setCurrentPlaybackSampleRate(sampleRate);
}

void PluginProcessor::releaseResources() {}

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}

void PluginProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                   juce::MidiBuffer &midiMessages) {
  juce::ignoreUnused(midiMessages);

  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());
  sampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void PluginProcessor::loadFile(const juce::String &path) {
  sampler.clearSounds();
  const auto file = juce::File(path);

  if (const std::unique_ptr<juce::AudioFormatReader> reader{
          formatManager.createReaderFor(file)}) {
    juce::BigInteger range;
    range.setRange(0, 128, true);
    sampler.addSound(
        new juce::SamplerSound("Sample", *reader, range, 60, 0.1, 0.1, 10.0));
  }
}

bool PluginProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor *PluginProcessor::createEditor() {
  return new PluginEditor(*this);
}

void PluginProcessor::getStateInformation(juce::MemoryBlock &destData) {
  juce::ignoreUnused(destData);
}

void PluginProcessor::setStateInformation(const void *data, int sizeInBytes) {
  juce::ignoreUnused(data, sizeInBytes);
}

juce::AudioBuffer<float>& PluginProcessor::getWaveForm() const {
    if (const auto sound = dynamic_cast<juce::SamplerSound*>(sampler.getSound(sampler.getNumSounds() - 1).get()))
    {
        return *sound->getAudioData();
    }

    static juce::AudioBuffer<float> dummybuffer;

    return dummybuffer;
}

juce::AudioProcessorValueTreeState::ParameterLayout
PluginProcessor::parameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      "Attack", "attack", juce::NormalisableRange<float>(0.0, 5.0), 0.5f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      "Sustain", "sustain", juce::NormalisableRange<float>(0.0, 5.0), 0.5f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      "Decay", "decay", juce::NormalisableRange<float>(0.0, 1.0), 0.5f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      "Release", "release", juce::NormalisableRange<float>(0.0, 1.0), 0.5f));

  return layout;
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new PluginProcessor();
}
