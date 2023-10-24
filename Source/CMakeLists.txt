
set (BaseTargetName Starp)

add_compile_definitions("$<$<CONFIG:Release>:STARP_DEBUG=0>")


juce_add_plugin("${BaseTargetName}"
        # VERSION ...                               # Set this if the plugin version is different to the project version
        # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
        # ICON_SMALL ...
        COMPANY_NAME "CodeVamping"
        IS_SYNTH TRUE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT TRUE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD FALSE
        PLUGIN_MANUFACTURER_CODE CVmp
        PLUGIN_CODE Strp
        FORMATS VST3
        PRODUCT_NAME "Starp")

target_sources(${BaseTargetName} PRIVATE
        HashRandom.cpp
        ParamData.cpp
        StarpEditor.cpp
        ProcessorParameters.cpp
        StarpProcessor/processing.cpp
        StarpProcessor/setup.cpp
        )

target_compile_definitions(${BaseTargetName}
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(${BaseTargetName} PRIVATE
        TinySHA1
        juce_recommended_config_flags
        juce_recommended_lto_flags
        juce_recommended_warning_flags)
