   
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

#include "ParamData.hpp"
#include "AlgorithmParameters.hpp"

#include <juce_audio_processors/juce_audio_processors.h>
 

struct ProcessorParameters  {      

    static const juce::String SPEED_TYPE_ID;
    static const juce::String SPEED_NOTE_ID;
    static const juce::String SPEED_BAR_ID;
    static const juce::String SPEED_MSEC_ID;

    // These are not automatable.  
    RandomParameters random_parameters;
    juce::int64 get_random_seed() const { return juce::int64(random_parameters.seed_value.getValue()); }
    LinearParameters linear_parameters;
    SpiralParameters spiral_parameters;

    juce::Value algorithm_index{juce::var{Algorithm::Random}};
    int get_algo_index() const { return int(algorithm_index.getValue()); }

    // These are automatable and will live in the Value Tree
    juce::AudioParameterChoice* speed;
    juce::AudioParameterFloat*  gate;
    juce::AudioParameterFloat*  gate_range;
    juce::AudioParameterInt*    velocity;
    juce::AudioParameterInt*    velo_range;
    juce::AudioParameterInt*    probability;
    juce::AudioParameterFloat*  timing_delay;
    juce::AudioParameterFloat*  timing_advance;
    juce::AudioParameterChoice* speed_type;
    juce::AudioParameterFloat*  speed_bar;
    juce::AudioParameterFloat*  speed_ms;

    std::unique_ptr<juce::AudioProcessorValueTreeState> apvts;

    ProcessorParameters(juce::AudioProcessor& processor);

    void pick_new_key();

};
