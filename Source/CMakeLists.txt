
set (BaseTargetName "${SF_PROJECT}")

add_compile_definitions("$<$<CONFIG:Release>:SF_DEBUG=0>")

include(GetGitRevisionDescription)

get_git_head_revision(GIT_REF GIT_HASH)

string(SUBSTRING "${GIT_HASH}" 0 7 GIT_SHORT_HASH )

message("GIT_HASH = " ${GIT_HASH})
message("GIT_SHORT_HASH = " ${GIT_SHORT_HASH})
message("GIT_REF = " ${GIT_REF})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(version.hpp.in version.hpp @ONLY)

string(TOUPPER "${CMAKE_BUILD_TYPE}" upper_build_type)
if(${upper_build_type} STREQUAL "RELEASE")
        set(PLUGIN_PRODUCT_NAME "${SF_PROJECT}")
        set(PLUGIN_PRODUCT_CODE "Sfar" )
else()
        set(PLUGIN_PRODUCT_NAME "${SF_PROJECT}Dev")
        set(PLUGIN_PRODUCT_CODE "SfaD")
endif()

string(TOUPPER "${PLUGIN_PRODUCT_NAME}" PLUGIN_NAME_UPPER)

configure_file(version.hpp.in version.hpp @ONLY)

juce_add_plugin("${BaseTargetName}"
        # VERSION ...                               # Set this if the plugin version is different to the project version
        # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
        # ICON_SMALL ...
        COMPANY_NAME "SolidFuel"
        IS_SYNTH TRUE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT TRUE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD FALSE
        PLUGIN_MANUFACTURER_CODE Sdfl
        PLUGIN_CODE ${PLUGIN_PRODUCT_CODE}
        FORMATS VST3
        PRODUCT_NAME ${PLUGIN_PRODUCT_NAME} )

target_sources(${BaseTargetName} PRIVATE
        HashRandom.hpp HashRandom.cpp
        position_data.hpp
        Algorithms/AlgoBase.hpp
        Algorithms/RandomAlgorithm.hpp
        Algorithms/LinearAlgorithm.hpp
        Algorithms/SpiralAlgorithm.hpp
        AlgorithmEnum.hpp AlgorithmParameters.hpp Algorithm.hpp
        ParamData.hpp ParamData.cpp
        PluginEditor.hpp PluginEditor.cpp
        EditorComponent/OverlayComponent.hpp
        EditorComponent/AlgoChoiceComponent.hpp
        EditorComponent/AlgoChoiceComponent.cpp
        EditorComponent/RandomAlgoOptionsComponent.hpp
        EditorComponent/RandomAlgoOptionsComponent.cpp
        EditorComponent/LinearAlgoOptionsComponent.hpp
        EditorComponent/LinearAlgoOptionsComponent.cpp
        EditorComponent/SpiralAlgoOptionsComponent.hpp
        EditorComponent/SpiralAlgoOptionsComponent.cpp
        EditorComponent/AlgorithmComponent.hpp
        EditorComponent/AlgorithmComponent.cpp
        EditorComponent/PropertyComponent.hpp
        EditorComponent/PropertyComponent.cpp
        EditorComponent/HeaderComponent.hpp
        EditorComponent/HeaderComponent.cpp
        ProcessorParameters.hpp ProcessorParameters.cpp
        PluginProcessor.hpp
        Processor/processing.cpp
        Processor/setup.cpp
        )

target_compile_definitions(${BaseTargetName}
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(${BaseTargetName} PRIVATE
        TinySHA1
        solidfuel
        juce_recommended_config_flags
        juce_recommended_lto_flags
        juce_recommended_warning_flags)
