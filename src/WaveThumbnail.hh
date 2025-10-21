#pragma once

#include "PluginProcessor.hh"
#include "juce_core/juce_core.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <vector>

class WaveThumbnail : public juce::Component,
                      public juce::FileDragAndDropTarget {
public:
  explicit WaveThumbnail(PluginProcessor &p);
  void paint(juce::Graphics &) override;
  void resized() override;
  bool isInterestedInFileDrag(const juce::StringArray &files) override;
  void filesDropped(const juce::StringArray &files, int x, int y) override;

private:
  std::vector<float> audioPoints;
  bool shouldBePainting{false};

  juce::String filename;

  PluginProcessor &processor;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveThumbnail);
};
