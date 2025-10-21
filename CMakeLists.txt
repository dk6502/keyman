cmake_minimum_required(VERSION 3.22)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(keyman VERSION 0.0.1)

add_subdirectory(JUCE)

juce_add_plugin(keyman
    # VERSION ...
    # ICON_BIG ...
    # ICON_SMALL ...
    COMPANY_NAME Dylan K.
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE
    # COPY_PLUGIN_AFTER_BUILD TRUE/FALSE
    PLUGIN_MANUFACTURER_CODE Dkr6
    PLUGIN_CODE KEY1

    FORMATS AU VST3 Standalone
    PRODUCT_NAME "Keyman")


# juce_generate_juce_header(AudioPluginExample)
#
#juce_add_binary_data(assets
#    SOURCES
#    assets/bg.png
#    assets/NimbusSanL-Bol.otf
#)

target_sources(keyman
    PRIVATE
    src/PluginProcessor.cc
    src/PluginEditor.cc
    src/WaveThumbnail.cc
        )

target_compile_definitions(keyman
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)


target_link_libraries(keyman
    PRIVATE
        juce::juce_core
        juce::juce_dsp
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
