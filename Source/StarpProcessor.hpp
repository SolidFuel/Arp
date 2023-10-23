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

#include "Algorithm.hpp"
#include "ParamData.hpp"

#include <shared_plugin_helpers/shared_plugin_helpers.h>

#include <juce_audio_processors/juce_audio_processors.h>

#include <fstream>

struct speed_value {
    juce::String name;
    double multiplier;
};

struct played_note {
    int note_value;
    double end_slot;

};

bool operator==(const played_note& lhs, const played_note& rhs);
bool operator<(const played_note& lhs, const played_note& rhs);

//============================================================================
struct position_data {
    double position_as_slots = -1.0;
    double slot_fraction = 0.0;
    double slot_number = -1.0;
    int samples_per_qn;
    bool is_playing = false;
};

//============================================================================
struct schedule {
    double slot_number;
    double start;
};

bool operator==(const schedule& lhs, const schedule& rhs);
bool operator<(const schedule& lhs, const schedule& rhs);

//==============================================================================
class StarpProcessor  : public PluginHelpers::ProcessorBase
{
public:
    //==============================================================================
    StarpProcessor();
    ~StarpProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout&) const override { return true; }

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override {return JucePlugin_Name;}

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void set_algo_index(int i) { algo_index = i; }

    //==============================================================================
    // Parameters

    int algo_index = Algorithm::Random;


private:

    struct Parameters
    {        
        juce::int64 random_key_ = 0L;

        juce::AudioParameterChoice* speed;
        juce::AudioParameterFloat*  gate;
        juce::AudioParameterInt*    velocity;
        juce::AudioParameterInt*    velo_range;
        juce::AudioParameterInt*    probability;
        juce::AudioParameterFloat*  timing_delay;
        juce::AudioParameterFloat*  timing_advance;

        std::unique_ptr<juce::AudioProcessorValueTreeState> apvts;

        Parameters(StarpProcessor& processor);

    };

    Parameters parameters;

    //==============================================================================


    // Sample rate
    double sample_rate_;

    // Set of notes we can choose from if we need
    // to schedule something.
    juce::SortedSet<int> notes_;

    // Notes we have sent the note-on for but need to wait
    // to send the note-off
    juce::Array<played_note> active_notes_;

    // Notes we are waiting to send the note-on for.
    juce::Array<schedule> scheduled_notes_;


    int current_algo_index_ = -1;
    void reassign_algorithm(int new_algo);
    std::unique_ptr<AlgorithmBase> algo_obj_;

    double next_scheduled_slot_number = -1.0;

    bool last_play_state_ = false;
    bool last_bypassed_state_ = false;

    double getSpeedFactor();
    double getGate();


    // Last time in millisecs that processBlock was called.
    // Used to detect bypass. If we haven't been called in
    // 100 ms then assume we've been bypassed.
    long long last_block_call_ = -1;

    // position when processBlock was last called.
    // Used to detect looping.
    double last_position_ = -1;

    const position_data compute_block_position();
    std::optional<juce::MidiMessage>maybe_play_note(bool notes_changed, double for_slot, double start_pos);

    void schedule_note(double current_pos, double slot_number);

    void reset_data();

public:
    Parameters* getParameters() { return &parameters; }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StarpProcessor)
};
