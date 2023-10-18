#include "ParamData.hpp"

//========================================================================

juce::Array<juce::var> AlgorithmIndexes {{
    juce::var{Algorithm::Random},
    juce::var{Algorithm::Up},
    juce::var{Algorithm::Down},

}};

juce::StringArray AlgorithmChoices{{
    "Random",
    "Up",
    "Down"
}};

//========================================================================

juce::Array<juce::var> SpeedIndexes {{
    juce::var{Speed::Sixteenth},
    juce::var{Speed::Eighth},
    juce::var{Speed::Quarter},
    juce::var{Speed::Half},

}};

juce::StringArray SpeedChoices{{
    "1/16",
    "1/8",
    "1/4",
    "1/2"
}};

