/****
 * Starp - Stable Random Arpeggiator Plugin 
 * Copyright (C) 2023 Mark Hollomon
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the 
 * Free Software Foundation, either version 3 of the License, or (at your 
 * option) any later version. This program is distributed in the hope that it 
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the LICENSE file
 * in the root directory.
 ****/

#include "ParamData.hpp"

//========================================================================

juce::Array<juce::var> AlgorithmIndexes {{
    juce::var{Algorithm::Random},
    juce::var{Algorithm::Linear},

}};

juce::StringArray AlgorithmChoices{{
    "Random",
    "Linear"
}};

//========================================================================

juce::Array<juce::var> SpeedIndexes {{
    juce::var{Speed::Sixteenth},
    juce::var{Speed::EighthTriplet},
    juce::var{Speed::SixteenthDotted},
    juce::var{Speed::Eighth},
    juce::var{Speed::QuarterTriplet},
    juce::var{Speed::EighthDotted},
    juce::var{Speed::Quarter},
    juce::var{Speed::HalfTriplet},
    juce::var{Speed::QuarterDotted},
    juce::var{Speed::Half},

}};

juce::StringArray SpeedChoices{{
    "1/16",
    "1/8t",
    "1/16d",
    "1/8",
    "1/4t",
    "1/8d",
    "1/4",
    "1/2t",
    "1/4d",
    "1/2"
}};

