#include "GrainSampler.hh"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_formats/juce_audio_formats.h"
#include "juce_core/juce_core.h"
#include "juce_graphics/juce_graphics.cpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>


GrainSound::GrainSound (const juce::String& soundName,
                            juce::AudioFormatReader& source,
                            const juce::BigInteger& notes,
                            int midiNoteForNormalPitch,
                            double attackTimeSecs,
                            double releaseTimeSecs,
                            double maxSampleLengthSeconds)
    : name (soundName),
      sourceSampleRate (source.sampleRate),
      midiNotes (notes),
      midiRootNote (midiNoteForNormalPitch)
{
    if (sourceSampleRate > 0 && source.lengthInSamples > 0)
    {
        length = juce::jmin ((int) source.lengthInSamples,
                       (int) (maxSampleLengthSeconds * sourceSampleRate));

        data.reset (new juce::AudioBuffer<float> (juce::jmin (2, (int) source.numChannels), length + 4));

        source.read (data.get(), 0, length + 4, 0, true, true);

        params.attack  = static_cast<float> (attackTimeSecs);
        params.release = static_cast<float> (releaseTimeSecs);
    }
}


GrainSound::~GrainSound()
{
}

bool GrainSound::appliesToNote (int midiNoteNumber)
{
    return midiNotes[midiNoteNumber];
}

bool GrainSound::appliesToChannel (int /*midiChannel*/)
{
    return true;
}

GrainVoice::GrainVoice() {
    for (int i = 0;i < 50; i++){
        grainsL.push_back(std::vector<float>(getSampleRate()));
    }
};
GrainVoice::~GrainVoice() {};

bool GrainVoice::canPlaySound(juce::SynthesiserSound *s) {
  return dynamic_cast<const GrainSound *>(s) != nullptr;
}

void GrainVoice::startNote(int midiNoteNumber, float velocity,
                           juce::SynthesiserSound *s,
                           int) {
  pos = 0;
  sample_inc = 0;
  if (auto *sound = dynamic_cast<const GrainSound *>(s)) {
      pitchRatio = std::pow(2.0, (midiNoteNumber - sound->midiRootNote) / 12.0) *
                 sound->sourceSampleRate / getSampleRate();
    lgain = velocity;
    rgain = velocity;
    auto& data = *sound->data;
    grainAdsr.setSampleRate(sound->sourceSampleRate);
    grainAdsr.setParameters(juce::ADSR::Parameters {
        0.01,
        (delay_size)/(float) sound->sourceSampleRate,
        0.0,
        0.0,
    });
    const float* const inR = data.getNumChannels() > 1 ? data.getReadPointer (1) : nullptr;
    for (size_t grain = 0; grain<grainsL.size(); grain++){
        grainsL[grain].clear();
        const float* const inL = data.getReadPointer (0);
        grainAdsr.noteOn();
        for (size_t sample = 0; sample<getSampleRate(); sample++) {
            grainsL[grain].push_back(inL[std::clamp(0,(int) sound->getAudioData()->getNumSamples(),(int)(pitchRatio*delay_size*grain+ sample * pitchRatio))]);
            grainsL[grain][sample] *= grainAdsr.getNextSample();
        }
        grainAdsr.reset();
    }
    juce::ignoreUnused(inR);
  }
}


void GrainVoice::stopNote(float, bool) {
    clearCurrentNote();
}

void GrainVoice::pitchWheelMoved(int) {};
void GrainVoice::controllerMoved(int, int) {};

void GrainVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (static_cast<GrainSound*> (getCurrentlyPlayingSound().get()))
    {
        float* outL = outputBuffer.getWritePointer (0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;

        for (int i = 0; i < numSamples - startSample; i++)
        {
            pos++;

            if (pos % delay_size == 0 && sample_inc < 49) {
              sample_inc++;
            } else if (sample_inc == 48) stopNote(0, false);

            size_t grainpos = pos%delay_size;
            *outL++ = lgain*grainsL[sample_inc][grainpos];
            *outR++ = lgain*grainsL[sample_inc][grainpos];

        }
    }
}
