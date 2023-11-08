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
    juce::var{Algorithm::Spiral},

}};

juce::StringArray AlgorithmChoices{{
    "Random",
    "Linear",
    "Spiral",
}};

 
//=======================================================================
juce::Array<speed_value> speed_parameter_values = {
    speed_value{"1/32"  , 0.125},
    speed_value{"1/16t" , 0.5/3.0 },
    speed_value{"1/32d" , 0.1875 },
    speed_value{"1/16q" , 0.20 },
    speed_value{"1/16"  , 0.25 },
    speed_value{"1/8t"  , 1.0/3.0 },
    speed_value{"1/16d" , 0.375},
    speed_value{"1/8q"  , 0.40},
    speed_value{"1/8"   , 0.50},
    speed_value{"1/4t"  , 2.0/3.0 },
    speed_value{"1/8d"  , 0.75},
    speed_value{"1/4q"  , 0.8 },
    speed_value{"1/4"   , 1.0 },
    speed_value{"1/2t"  , 4.0/3.0 },
    speed_value{"1/4d"  , 1.5 },
    speed_value{"1/2q"  , 1.6 },
    speed_value{"1/2"   , 2.0 },
    speed_value{"1/1t"  , 8.0/3.0 },
    speed_value{"1/2d"  , 3.0 },
    speed_value{"1/1q"  , 3.2 },
    speed_value{"1/1"   , 4.0 },
};


juce::StringArray SpeedTypes{{ "note", "bar", "msec" }};