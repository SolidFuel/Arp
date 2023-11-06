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

#pragma once

#include <juce_core/juce_core.h>

#include "AlgorithmEnum.hpp"

extern juce::Array<juce::var> AlgorithmIndexes;
extern juce::StringArray AlgorithmChoices;

//========================================================================
struct speed_value {
    juce::String name;
    double multiplier;
};


extern juce::Array<speed_value> speed_parameter_values;