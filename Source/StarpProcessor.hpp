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
#include "ProcessorParameters.hpp"

#include "position_data.hpp"
#include <juce_audio_processors/juce_audio_processors.h>

#include <fstream>

#include "ValueListener.hpp"


struct played_note {
    int note_value;
    double end_slot;

};

bool operator==(const played_note& lhs, const played_note& rhs);
bool operator<(const played_note& lhs, const played_note& rhs);


//============================================================================
struct schedule {
    double slot_number;
    double start;
};

bool operator==(const schedule& lhs, const schedule& rhs);
bool operator<(const schedule& lhs, const schedule& rhs);

//==============================================================================
class StarpProcessor  : public juce::AudioProcessor {

public:
    //==============================================================================
    // These are in setup.cpp
    StarpProcessor();
    ~StarpProcessor() override;

    static juce::AudioProcessor::BusesProperties getDefaultProperties();

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;


    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlock (juce::AudioBuffer<double>&, juce::MidiBuffer&) override;

    void processMidi(int sample_count, juce::MidiBuffer&);

    //==============================================================================

    //==============================================================================
    // These are in setup.cpp

    bool isBusesLayoutSupported (const BusesLayout&) const override { return true; }

    const juce::String getName() const override {return JucePlugin_Name;}

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    // depends on the host we are in
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}


    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:


    ProcessorParameters parameters_;

    //==============================================================================


    // Used to set other things based on the Host
    // defined in setup.hpp
    static juce::PluginHostType host_type;

    // Sample rate
    double sample_rate_;

    // Set of notes we can choose from if we need
    // to schedule something.
    juce::SortedSet<int> incoming_notes_;

    // Has incoming_notes_ changed since the last
    // time we called the note picking algoirthm ?
    bool notes_changed_ = false;

    // Notes we have sent the note-on for but need to wait
    // to send the note-off
    juce::Array<played_note> active_notes_;

    // Notes we are waiting to send the note-on for.
    juce::Array<schedule> scheduled_notes_;


    bool algo_changed_ = false;
    void update_algorithm(int new_algo);
    std::unique_ptr<AlgorithmBase> algo_obj_;

    double last_scheduled_slot_number_ = -1.0;

    bool last_play_state_ = false;

    double getSpeedFactor(double bpm, const juce::AudioPlayHead::TimeSignature &time_sig);
    double getGate(double slot);

    // Last time in millisecs that processBlock was called.
    // Used to detect bypass. If we haven't been called in
    // 100 ms then assume we've been bypassed.
    long long last_block_call_ = -1;

    // position when processBlock was last called.
    // Used to detect looping.
    double last_position_ = -1;

    double fake_clock_sample_count_ = 0;

    const position_data compute_block_position();
    std::optional<juce::MidiMessage>maybe_play_note(double for_slot, double start_pos);

    void schedule_note(double current_pos, double slot_number, bool can_advance);

    void reset_data(bool clear_incoming = true);

    void parseCurrentXml(const juce::XmlElement * elem);
    void parseOriginalXml(const juce::XmlElement * elem);

    juce::SharedResourcePointer<juce::TooltipWindow> tooltipWindow;

    ValueListener algo_listener_;

public:
    ProcessorParameters* getParameters() { return &parameters_; }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StarpProcessor)
};
