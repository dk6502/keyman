#include "WaveThumbnail.hh"
#include "PluginProcessor.hh"
#include "juce_core/juce_core.h"
#include "juce_graphics/juce_graphics.h"

WaveThumbnail::WaveThumbnail(PluginProcessor &p) : processor(p) {
}

void WaveThumbnail::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::cadetblue.darker());
    const auto waveform = processor.getWaveForm();

    if (waveform.getNumSamples() > 0) {
        juce::Path p;
        audioPoints.clear();

        const auto ratio = waveform.getNumSamples()/getWidth();
        const auto buffer = waveform.getReadPointer(0);

        for (int sample = 0; sample < waveform.getNumSamples(); sample += ratio) {
            audioPoints.push_back(buffer[sample]);
        }

        const auto height = static_cast<float>(getHeight());

        g.setColour(juce::Colours::white);
        p.startNewSubPath(0.0f,height/2.0f);

        for(int sample = 0; sample < audioPoints.size(); ++sample) {
            auto point = juce::jmap<float> (audioPoints[sample], -1.0f, 1.0f, height, 0);
            p.lineTo(static_cast<float>(sample), point);
        }
        g.strokePath(p, juce::PathStrokeType (2));

        g.setColour (juce::Colours::white);
        g.setFont (15.0f);
        auto textBounds = getLocalBounds().reduced (10, 10);
        g.drawFittedText (filename, textBounds, juce::Justification::topRight, 1);

        auto playHeadPosition = juce::jmap<int> (processor.getNumSamplerSounds(), 0, processor.getWaveForm().getNumSamples(), 0, getWidth());

        g.setColour (juce::Colours::white);

        const auto playPosition = static_cast<float>(playHeadPosition);

        g.drawLine (playPosition, 0, playPosition, static_cast<float>(getHeight()), 2.0f);

        g.setColour (juce::Colours::black.withAlpha (0.2f));
        g.fillRect (0, 0, playHeadPosition, getHeight());
    } else {
        g.setColour (juce::Colours::white);
        g.setFont (40.0f);
        g.drawFittedText ("Drop an Audio File to Load", getLocalBounds(), juce::Justification::centred, 1);
    }
}

bool WaveThumbnail::isInterestedInFileDrag (const juce::StringArray& files)
{
    for (const auto& file : files)
    {
        if (file.contains (".wav") || file.contains (".mp3") || file.contains (".aif"))
        {
            return true;
        }
    }

    return false;
}

void WaveThumbnail::filesDropped (const juce::StringArray& files, int x, int y)
{
    for (const auto& file : files)
    {
        if (isInterestedInFileDrag (file))
        {
            auto myFile = std::make_unique<juce::File>(file);
            filename = myFile->getFileNameWithoutExtension();

            processor.loadFile (file);
        }
    }

    repaint();
}

void WaveThumbnail::resized() {

}
