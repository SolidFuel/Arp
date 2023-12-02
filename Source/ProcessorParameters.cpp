/****
 * solidArp - Stable Random Arpeggiator Plugin 
 * Copyright (C) 2023 Solid Fuel
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

const juce::String DEFAULT_NOTE_SPEED = "1/8";
constexpr float DEFAULT_BAR_SPEED = 8;
constexpr float DEFAULT_MS_SPEED = 250;


//============================================================================

const juce::String ProcessorParameters::SPEED_TYPE_ID = "speed_type";
const juce::String ProcessorParameters::SPEED_NOTE_ID = "speed";
const juce::String ProcessorParameters::SPEED_BAR_ID  = "speed_bar";
const juce::String ProcessorParameters::SPEED_MSEC_ID = "speed_msec";



ProcessorParameters::ProcessorParameters(juce::AudioProcessor& processor) {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    random_parameters.pick_new_key();

    juce::StringArray speed_choices;
    
    speed_choices.ensureStorageAllocated(speed_parameter_values.size());
    for (auto const &sv : speed_parameter_values) {
        speed_choices.add(sv.name);
    }

    int default_speed = speed_choices.indexOf(DEFAULT_NOTE_SPEED);

    auto reverse_bar_range = juce::NormalisableRange<float>(1.0f, 32.0f,
        [] (auto rangeStart, auto rangeEnd, auto normalised)
            { return juce::jmap (normalised, rangeEnd, rangeStart); },
        [] (auto rangeStart, auto rangeEnd, auto value)
            { return juce::jmap (value, rangeEnd, rangeStart, 0.0f, 1.0f); },
        [] (auto, auto, auto value)
            { return std::round(value); });

    auto bar_attributes = juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](auto value, auto) { return juce::String(value, 0); });

    // Hosted Parameters
    speed = new juce::AudioParameterChoice({SPEED_NOTE_ID, 1}, "Speed in Notes", speed_choices, default_speed);
    layout.add(std::unique_ptr<juce::RangedAudioParameter>(speed));

    auto gate_norm_range = juce::NormalisableRange<float>{10.0, 200.0, 0.1f};
    gate = new juce::AudioParameterFloat({ "gate", 2 },  "Gate %", gate_norm_range, 100.0);
    layout.add(std::unique_ptr<juce::RangedAudioParameter>(gate));

    auto gate_range_range = juce::NormalisableRange<float>{0.0, 100.0, 1.0};
    gate_range = new juce::AudioParameterFloat({ "gate_range", 3 },  "Gate Range", gate_range_range, 0.0);
    layout.add(std::unique_ptr<juce::RangedAudioParameter>(gate_range));

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

    speed_type = new juce::AudioParameterChoice({SPEED_TYPE_ID, 9}, "Speed Type", SpeedTypes, SpeedType::Note);
    layout.add(std::unique_ptr<juce::RangedAudioParameter>(speed_type));

    speed_bar = new juce::AudioParameterFloat({SPEED_BAR_ID, 10}, "Speed per Bars", 
            reverse_bar_range, DEFAULT_BAR_SPEED, bar_attributes);
    layout.add(std::unique_ptr<juce::RangedAudioParameter>(speed_bar));

    speed_ms = new juce::AudioParameterFloat({ SPEED_MSEC_ID, 11 }, "Speed in msec", 10, 1000, DEFAULT_MS_SPEED);
    layout.add(std::unique_ptr<juce::RangedAudioParameter>(speed_ms));






    apvts = std::unique_ptr<juce::AudioProcessorValueTreeState>(
        new juce::AudioProcessorValueTreeState(
        processor, nullptr, "STARP-PARAMETERS", std::move(layout)));
}