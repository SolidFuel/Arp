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

#include "ProcessorParameters.hpp"
#include "ParamData.hpp"

const juce::String DEFAULT_SPEED = "1/8";


//============================================================================

ProcessorParameters::ProcessorParameters(juce::AudioProcessor& processor) {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    random_parameters.pick_new_key();

    juce::StringArray speed_choices;
    
    speed_choices.ensureStorageAllocated(speed_parameter_values.size());
    for (auto const &sv : speed_parameter_values) {
        speed_choices.add(sv.name);
    }

    int default_speed = speed_choices.indexOf(DEFAULT_SPEED);

    // Hosted Parameters
    speed = new juce::AudioParameterChoice({"speed", 1}, "Speed", speed_choices, default_speed);
    layout.add(std::unique_ptr<juce::RangedAudioParameter>(speed));

    gate = new juce::AudioParameterFloat({ "gate", 2 },  "Gate %", 10.0, 200.0, 100.0);
    layout.add(std::unique_ptr<juce::RangedAudioParameter>(gate));

    probability = new juce::AudioParameterInt({"probability", 4}, "Probability", 0, 100, 100);
    layout.add(std::unique_ptr<juce::RangedAudioParameter>(probability));

    velocity = new juce::AudioParameterInt({"velocity", 5}, "Velocity", 1, 127, 100);
    layout.add(std::unique_ptr<juce::RangedAudioParameter>(velocity));

    velo_range = new juce::AudioParameterInt({"velocity_range", 6}, "Vel. Range", 0, 64, 0);
    layout.add(std::unique_ptr<juce::RangedAudioParameter>(velo_range));

    timing_delay = new juce::AudioParameterFloat({ "timing_delay", 7 }, "Delay", 0.0, 30.0, 0.0);
    layout.add(std::unique_ptr<juce::RangedAudioParameter>(timing_delay));

    timing_advance = new juce::AudioParameterFloat({ "timing_advance", 8 }, "Advance", -30.0, 0.0, 0.0);
    layout.add(std::unique_ptr<juce::RangedAudioParameter>(timing_advance));

    apvts = std::unique_ptr<juce::AudioProcessorValueTreeState>(
        new juce::AudioProcessorValueTreeState(
        processor, nullptr, "STARP-PARAMETERS", std::move(layout)));
}