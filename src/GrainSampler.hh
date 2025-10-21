#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_formats/juce_audio_formats.h"

class GrainSound : public juce::SynthesiserSound {
public:
  GrainSound(const juce::String &name, juce::AudioFormatReader &source,
             const juce::BigInteger &midiNotes, int midiNoteForNormalPitch,
             double attackTimeSecs, double releaseTimeSecs,
             double maxSampleLengthSeconds);

  /** Destructor. */
  ~GrainSound() override;

  /** Returns the sample's name */
  const juce::String &getName() const noexcept { return name; }

  juce::AudioBuffer<float> *getAudioData() const noexcept { return data.get(); }

  void setEnvelopeParameters(juce::ADSR::Parameters parametersToUse) {
    params = parametersToUse;
  }

  bool appliesToNote(int midiNoteNumber) override;
  bool appliesToChannel(int midiChannel) override;

private:
  friend class GrainVoice;

  juce::String name;
  std::unique_ptr<juce::AudioBuffer<float>> data;
  double sourceSampleRate;
  juce::BigInteger midiNotes;
  int length = 0, midiRootNote = 0;

  juce::ADSR::Parameters params;

  JUCE_LEAK_DETECTOR(GrainSound)
};

class GrainVoice : public juce::SynthesiserVoice {
public:
  GrainVoice();
  ~GrainVoice() override;
  bool canPlaySound(juce::SynthesiserSound *) override;
  void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *,
                 int pitchWheel) override;
  void stopNote(float velocity, bool allowTailOff) override;

  void pitchWheelMoved(int newValue) override;
  void controllerMoved(int controllerNumber, int newValue) override;

  void renderNextBlock(juce::AudioBuffer<float> &, int startSample,
                       int numSamples) override;
  using SynthesiserVoice::renderNextBlock;

private:
  double pitchRatio = 0;
  double sourceSamplePosition = 0;
  double lgain = 0;
  double rgain = 0;
};
