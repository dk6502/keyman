#include "GrainSampler.hh"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_formats/juce_audio_formats.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/juce_core.h"
#include "juce_dsp/juce_dsp.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>

GrainSound::GrainSound(const juce::String &soundName,
                       juce::AudioFormatReader &source,
                       const juce::BigInteger &notes,
                       int midiNoteForNormalPitch, double attackTimeSecs,
                       double releaseTimeSecs, double maxSampleLengthSeconds)
    : name(soundName), sourceSampleRate(source.sampleRate), midiNotes(notes),
      midiRootNote(midiNoteForNormalPitch) {
  if (sourceSampleRate > 0 && source.lengthInSamples > 0) {
    length = juce::jmin((int)source.lengthInSamples,
                        (int)(maxSampleLengthSeconds * sourceSampleRate));

    data.reset(new juce::AudioBuffer<float>(
        juce::jmin(2, (int)source.numChannels), length + 4));

    source.read(data.get(), 0, length + 4, 0, true, true);

    params.attack = static_cast<float>(attackTimeSecs);
    params.release = static_cast<float>(releaseTimeSecs);
  }
}

GrainSound::~GrainSound() {}

bool GrainSound::appliesToNote(int midiNoteNumber) {
  return midiNotes[midiNoteNumber];
}

bool GrainSound::appliesToChannel(int /*midiChannel*/) { return true; }

GrainVoice::GrainVoice(juce::AudioProcessorValueTreeState *p) {
  for (size_t i = 0; i < grains.size(); i++) {
    grains[i].setSize(1, 96000);
  }
  grainSizeParam =
      dynamic_cast<juce::AudioParameterFloat *>(p->getParameter("size"));
};
GrainVoice::~GrainVoice() {};

bool GrainVoice::canPlaySound(juce::SynthesiserSound *s) {
  return dynamic_cast<const GrainSound *>(s) != nullptr;
}

void GrainVoice::startNote(int midiNoteNumber, float velocity,
                           juce::SynthesiserSound *s, int) {
  pos = 0;
  sample_inc = 0;
  if (auto *sound = dynamic_cast<const GrainSound *>(s)) {
    pitchRatio = std::pow(2.0, (midiNoteNumber - sound->midiRootNote) / 12.0) *
                 sound->sourceSampleRate / getSampleRate();
    lgain = velocity;
    rgain = velocity;
    auto &data = *sound->data;
    const float *const inL = data.getReadPointer(0);
    const float *const inR =
        data.getNumChannels() > 1 ? data.getReadPointer(1) : nullptr;
    grain_size = (size_t)getSampleRate() * grainSizeParam->get();
    for (size_t grain = 0; grain < grains.size(); grain++) {
      float *out = grains[grain].getWritePointer(0);
      for (size_t sample = 0; sample < grain_size; sample++) {
        *out++ =
            inL[std::clamp(0, (int)(sound->getAudioData()->getNumSamples()),
                           (int)(grain * grain_size + sample * pitchRatio))];
      }
      hann.multiplyWithWindowingTable(grains[grain].getWritePointer(0),
                                      grain_size);
    }
    juce::ignoreUnused(inR);
  }
}

void GrainVoice::stopNote(float, bool) { clearCurrentNote(); }

void GrainVoice::pitchWheelMoved(int) {};
void GrainVoice::controllerMoved(int, int) {};

void GrainVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer,
                                 int startSample, int numSamples) {
  if (static_cast<GrainSound *>(getCurrentlyPlayingSound().get())) {
    const float *in = grains[sample_inc].getReadPointer(0);
    float *outL = outputBuffer.getWritePointer(0, startSample);
    float *outR = outputBuffer.getNumChannels() > 1
                      ? outputBuffer.getWritePointer(1, startSample)
                      : nullptr;

    for (int i = 0; i < numSamples - startSample; i++) {
      pos++;

      if (pos % grain_size == 0 && sample_inc < grain_count - 1) {
        sample_inc++;
      } else if (sample_inc == grain_count - 2)
        stopNote(0.0, false);
      size_t grainpos = pos % grain_size;
      *outL++ += in[grainpos];
      *outR++ += in[grainpos];
    }
  }
}
