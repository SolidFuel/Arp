
set (BaseTargetName Starp)

add_compile_definitions("$<$<CONFIG:Release>:STARP_DEBUG=0>")

include(GetGitRevisionDescription)

get_git_head_revision(GIT_REF GIT_HASH)

string(SUBSTRING "${GIT_HASH}" 0 7 GIT_SHORT_HASH )

message("GIT_HASH = " ${GIT_HASH})
message("GIT_SHORT_HASH = " ${GIT_SHORT_HASH})
message("GIT_REF = " ${GIT_REF})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(version.hpp.in version.hpp @ONLY)

string(TOUPPER "${CMAKE_BUILD_TYPE}" Starp_build_type)
if(${Starp_build_type} STREQUAL "RELEASE")
        set(Starp_PRODUCT_NAME "Starp")
        set(Starp_PLUGIN_CODE "Strp" )
else()
        set(Starp_PRODUCT_NAME "StarpDev")
        set(Starp_PLUGIN_CODE "StrD")
endif()

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
        PLUGIN_CODE ${Starp_PLUGIN_CODE}
        FORMATS VST3
        PRODUCT_NAME ${Starp_PRODUCT_NAME} )

target_sources(${BaseTargetName} PRIVATE
        HashRandom.hpp HashRandom.cpp
        position_data.hpp ValueListener.hpp
        Algorithms/AlgoBase.hpp
        Algorithms/RandomAlgorithm.hpp
        Algorithms/LinearAlgorithm.hpp
        Algorithms/SpiralAlgorithm.hpp
        AlgorithmEnum.hpp AlgorithmParameters.hpp Algorithm.hpp
        ParamData.hpp ParamData.cpp
        StarpEditor.hpp StarpEditor.cpp
        EditorComponent/BoxComponent.hpp
        EditorComponent/BoxComponent.cpp
        EditorComponent/ButtonGroupComponent.hpp
        EditorComponent/ButtonGroupComponent.cpp
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
        StarpProcessor.hpp
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
