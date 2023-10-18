#pragma once

#include <juce_core/juce_core.h>

//========================================================================
enum Algorithm {
    Random,
    Up,
    Down
};

extern juce::Array<juce::var> AlgorithmIndexes;
extern juce::StringArray AlgorithmChoices;

//========================================================================
enum Speed {
    Sixteenth,
    Eighth,
    Quarter,
    Half
};

extern juce::Array<juce::var> SpeedIndexes;
extern juce::StringArray SpeedChoices;
