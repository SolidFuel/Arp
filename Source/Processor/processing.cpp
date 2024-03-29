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

#include "../PluginProcessor.hpp"
#include "../PluginEditor.hpp"
#include "../Starp.hpp"

#include <iomanip>
#include <cmath>



//============================================================================
// PLAYED_NOTE methods
//============================================================================
bool operator==(const played_note& lhs, const played_note& rhs) {
    return lhs.note_value == rhs.note_value;
}

bool operator<(const played_note& lhs, const played_note& rhs) {
    return lhs.note_value < rhs.note_value;
}


bool operator==(const schedule& lhs, const schedule& rhs){ return std::abs(lhs.start - rhs.start) < 0.000001; }
bool operator<(const schedule& lhs, const schedule& rhs) { return lhs.start < rhs.start; }



//============================================================================
//============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int) {

    DBGLOG("PREPARE called");
    sample_rate_ = sampleRate;
    last_position_ = -1;
    algo_changed_ = true;

    // This will finesse the bypass logic so we don't reset again.
    last_block_call_ = juce::Time::currentTimeMillis();

    DBGLOG("HiResTimer = ", juce::Time::getHighResolutionTicksPerSecond())

    reset_data();

}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

    DBGLOG("RELEASE called");
    incoming_notes_.clear();
    active_notes_.clear();
    scheduled_notes_.clear();

}

//============================================================================
void PluginProcessor::update_algorithm(int new_algo) {

    DBGLOG("PluginProcessor::update_algorithm called # ", new_algo)

    if (!algo_obj_ || algo_obj_->get_algo() != new_algo) {
        DBGLOG("Changing to new algorithm # ", new_algo);

        // AlgUp and Down will only show up if setStateInformation is
        // somehow flawed. But lets be cautions.
        switch (new_algo) {
            case Algorithm::Random :
                algo_obj_ = std::make_unique<RandomAlgorithm>(&parameters_.random_parameters);
                break;
            case Algorithm::AlgUp :
                parameters_.linear_parameters.direction = LinearParameters::Direction::Up;
                parameters_.linear_parameters.zigzag = false;
                parameters_.algorithm_index = Algorithm::Linear;
                algo_obj_ = std::make_unique<LinearAlgorithm>(&parameters_.linear_parameters);
                break;
            case Algorithm::AlgDown :
                parameters_.linear_parameters.direction = LinearParameters::Direction::Down;
                parameters_.linear_parameters.zigzag = false;
                parameters_.algorithm_index = Algorithm::Linear;
                algo_obj_ = std::make_unique<LinearAlgorithm>(&parameters_.linear_parameters);
                break;
            case Algorithm::Linear :
                algo_obj_ = std::make_unique<LinearAlgorithm>(&parameters_.linear_parameters);
                break;
            case Algorithm::Spiral :
                algo_obj_ = std::make_unique<SpiralAlgorithm>(&parameters_.spiral_parameters);
                break;
        }

    }

}

//============================================================================
 void PluginProcessor::update_position_data(int64 tick_count) {

    double bpm = 120.0;
    double quarter_notes_per_beat = 1;
    juce::AudioPlayHead::TimeSignature time_sig{4, 4};

    pd.tick_count = tick_count;

    auto *play_head = getPlayHead();

    if (play_head) {
        auto position = play_head->getPosition();
        if (position) {
            pd.is_playing = position->getIsPlaying();

            auto hostBpm = position->getBpm();
            if (hostBpm) {
                bpm = *hostBpm;
            }
            auto host_time_sig = position->getTimeSignature();
            if (host_time_sig) {
                time_sig = *host_time_sig;
            }

            quarter_notes_per_beat = 4.0 / time_sig.denominator;
            pd.speed = getSpeedFactor(bpm, time_sig);

            auto opt_pos_qn = position->getPpqPosition();
            if (opt_pos_qn) {
                // position in slots
                pd.set_position(*opt_pos_qn);
            }
        }
    }

    pd.samples_per_qn =  static_cast<int>(std::ceil((sample_rate_ * 60.0) / 
                (bpm * quarter_notes_per_beat)));

    if (! pd.is_playing ) {
        // need to synthesize the data
        pd.speed = getSpeedFactor(bpm, time_sig);
        pd.set_position(fake_clock_sample_count_ / pd.samples_per_qn);
    }

    pd.ticks_per_slot = int64(juce::Time::getHighResolutionTicksPerSecond() /
                ((bpm * quarter_notes_per_beat) / (pd.speed * 60.0)  ));

}

//============================================================================
std::optional<juce::MidiMessage> PluginProcessor::maybe_play_note(
        double for_slot, double start_pos) {

    DBGLOG("maybe_play_note called ", for_slot, " / ", start_pos)
    if (incoming_notes_.size() == 0) {
        algo_obj_->reset();
        return std::nullopt;
    }

    int note_prob = parameters_.probability->get();

    HashRandom prob_rng{"Probability", parameters_.get_random_seed(), for_slot};
    if (prob_rng.nextInt(0, 101) > note_prob ) {
        return std::nullopt;
    }

    std::optional<juce::MidiMessage> retval;

    int new_note = algo_obj_->getNextNote(for_slot, incoming_notes_, notes_changed_);
    notes_changed_ = false;

    if (new_note >= 0 ) {

        int range = parameters_.velo_range->get();
        int note_velocity = parameters_.velocity->get();

        if (range > 0)  {
            HashRandom vel_rng{"Velocity", parameters_.get_random_seed(), for_slot};


            int max = juce::jmin(128, note_velocity + range); 
            int min = juce::jmax(1, note_velocity - range);

            note_velocity = vel_rng.nextInt(min, max);
        }

        retval = juce::MidiMessage::noteOn(
                1, new_note, (std::uint8_t) note_velocity
            );
        
        double gate = getGate(for_slot);
        DBGLOG("--- gate = ", gate)
        auto end_tick =  int64 (pd.tick_count + 
            ((start_pos - pd.position_as_slots + gate) * pd.ticks_per_slot ));

        active_notes_.add({new_note, end_tick});
        DBGLOG("--- start ", new_note, " @ ", start_pos, "; end = ", end_tick)

    } else {
        DBGLOG("--- algo could not find note to start");
    }

    return retval;
}

//============================================================================
void PluginProcessor::schedule_note(double current_pos, double slot_number, 
                    bool can_advance) {

    HashRandom rng{"Humanize", parameters_.get_random_seed(), slot_number};

    auto advance = parameters_.timing_advance->get() * can_advance;
    float variance = rng.nextFloat(
            advance,
            parameters_.timing_delay->get());

    double sched_start = slot_number + (variance/100.0);

    if (sched_start >= current_pos ) {
        last_scheduled_slot_number_ = slot_number;
        scheduled_notes_.addUsingDefaultSort({slot_number, sched_start});
        DBGLOG("--- scheduling slot ", slot_number, " @ ", sched_start);
    } else if (can_advance) {
        // try again without advance
        schedule_note(current_pos, slot_number, false);
    } else {
        DBGLOG("--- tried to schedule slot ", slot_number,
            " @ ", sched_start, " but late (", current_pos,  ")");
    }

}

//============================================================================
void PluginProcessor::reset_data(bool clear_incoming) {
    DBGLOG("   reset_data called");

    last_scheduled_slot_number_ = -1.0;
    scheduled_notes_.clearQuick();
    //active_notes_.clearQuick();

    if (clear_incoming) {
        incoming_notes_.clearQuick();
        notes_changed_ = false;
    }

    DBGLOG("   reset_data finished"); 
}

//============================================================================
void PluginProcessor::processBlock (juce::AudioBuffer<double>& buffer,
                                    juce::MidiBuffer& midiBuffer) {

    auto ticks = juce::Time::getHighResolutionTicks();

    //DBGLOG("--- ProcessBlock START")

    auto sample_count = buffer.getNumSamples();

    for (auto i = 0; i < buffer.getNumChannels(); ++i)
        buffer.clear (i, 0, sample_count);

    processMidi(sample_count, ticks, midiBuffer);

}

//============================================================================
void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                    juce::MidiBuffer& midiBuffer) {


    //DBGLOG("--- ProcessBlock START")

    auto ticks = juce::Time::getHighResolutionTicks();

    auto sample_count = buffer.getNumSamples();

    for (auto i = 0; i < buffer.getNumChannels(); ++i)
        buffer.clear (i, 0, sample_count);

    processMidi(sample_count, ticks, midiBuffer);

}

//============================================================================
void PluginProcessor::processMidi(int sample_count, int64 tick_count,
                juce::MidiBuffer& midiBuffer ) {

    if (algo_changed_) {
        algo_changed_ = false;
        update_algorithm(parameters_.get_algo_index());
    }

    update_position_data(tick_count);

    double slots_in_buffer = (double(sample_count) / double(pd.samples_per_qn)) / pd.speed;
    // How long is the buffer in HiRez ticks
    // All the casting is due to the fact that slots_in_buffer will
    // always be (0, 1] so integer math will come out as zero.
    int64 ticks_in_buffer = int64(double(pd.ticks_per_slot) * slots_in_buffer);

    // how many samples in a slot
    auto slot_duration = static_cast<int>(std::ceil(double(pd.samples_per_qn) * pd.speed));

    bool do_cleanup = false;
    bool clear_incoming = true;
    bool play_transition = false;

    if (pd.is_playing != last_play_state_) {
        DBGLOG("--- Play state changed to ", pd.is_playing);
        last_play_state_ = pd.is_playing;
        do_cleanup = true;
        play_transition = true;
    }

    if (pd.position_as_slots < last_position_) {
        // we must have looped without stopping.
        // clear everything and restart. 
        // This isn't really right. If we are looping without stopping,
        // then we should let things that started at the end of the loop
        // continue. See also below where we stop all active notes.
        DBGLOG("--- LOOPING - resetting data");
        do_cleanup = true;
        // During loops, Cubase has a habit of sending the Note-Ons
        // that happen at the beginning of the loop in the ending
        // block of the loop. Don't clear the notes that we have
        // seen so that we hold on to them.
        if (host_type.isCubase() && !play_transition) {
            DBGLOG("--- LOOPING - Doing Cubase specifics.");
            clear_incoming = false;
        }
    }
    last_position_ = pd.position_as_slots;

    if ( do_cleanup) {
        reset_data(clear_incoming);
    }

    //DBGLOG("processBlock setup done")


#if SF_DEBUG
    if (pd.is_playing || !midiBuffer.isEmpty() 
            || !incoming_notes_.isEmpty() 
            || !active_notes_.isEmpty() ) {
        std::stringstream x;
        x << "START playing = " << pd.is_playing   << "; ppq_pos = " << pd.qn_position 
            << "; pos_as_slots = " << pd.position_as_slots 
            << ";\n     slot_number = " << pd.slot_number
            << "; slot_fraction = " << std::setprecision(7) << pd.slot_fraction 
            << ";\n     ticks_in_buffer = " << ticks_in_buffer
            << "; last_sched_slot = " << last_scheduled_slot_number_;
        dbgout->logMessage(x.str());
    }
#endif


    // === Read Midi Messages and update incoming_notes_ set
    juce::MidiBuffer newBuffer;

    // These are used if debug is set
    int on_count = 0;
    int off_count = 0;
    int stop_count = 0;

    juce::ignoreUnused(on_count, off_count, stop_count);

    for (const auto metadata : midiBuffer)
    {
        const auto msg = metadata.getMessage();
        if      (msg.isNoteOn())  { 
            incoming_notes_.add(msg.getNoteNumber()); 
            notes_changed_ = true;
            on_count += 1;
        } else if (msg.isNoteOff()) { 
            if (incoming_notes_.contains(msg.getNoteNumber())) {
                incoming_notes_.removeValue(msg.getNoteNumber());
                notes_changed_ = true;
                off_count += 1;
            } else {
                // if we didn't see the NoteOn assume we were bypassed 
                // and put the message back in the buffer.
                newBuffer.addEvent(msg, metadata.samplePosition);
            }
        } else {
            newBuffer.addEvent(msg, metadata.samplePosition);
        }
    }

    midiBuffer.swapWith(newBuffer);

    juce::Array<played_note> new_notes{};

    //DBGLOG("input buffer processed")

    //------------------------------------------------------------------------
    // Stop any notes that will end during this buffer
    for (int idx = 0; idx < active_notes_.size(); ++idx) {

        auto thisNote = active_notes_.getUnchecked(idx);
        auto note_value = thisNote.note_value;
        auto end_tick = thisNote.end_tick;
        auto end_buffer = pd.tick_count + ticks_in_buffer;

        if (play_transition && !pd.is_playing) {
            // We stopped, so turn all active notes off
            DBGLOG("stop ", note_value, " @ ", 2);
            midiBuffer.addEvent(
                juce::MidiMessage::noteOff (1, note_value), sample_count - 2);
            stop_count += 1;
        } else if ( end_buffer > end_tick ) {
            // This is the number of samples into the buffer where the note should turn off.
            int offset = int((end_tick - end_buffer) / pd.ticks_per_slot);
            DBGLOG("stop ", note_value, " @ ", offset);
            midiBuffer.addEvent(juce::MidiMessage::noteOff (1, note_value), offset);
            stop_count += 1;

        } else {
            // we didn't stop it, so copy to the new list
            new_notes.add(thisNote);
        }
    }

    active_notes_.clearQuick();
    active_notes_.addArray(new_notes);

    //DBGLOG("active notes processed")

    if (pd.is_playing || !incoming_notes_.isEmpty() || !active_notes_.isEmpty() ) {
        DBGLOG("incoming = ", incoming_notes_.size(), "; active = ", 
                active_notes_.size(), "; changed = ", notes_changed_, 
                "; on = ", on_count, "; off = ", off_count, 
                "; stop = ", stop_count
        )
    }

    //------------------------------------------------------------------------
    // Schedule note if required.
    if (! incoming_notes_.isEmpty()) {
        if (last_scheduled_slot_number_ < pd.slot_number) {
            schedule_note(pd.position_as_slots, pd.slot_number, false);
        }
        if ( (pd.slot_fraction > 0.5) &&  last_scheduled_slot_number_ <= pd.slot_number) {
            schedule_note(pd.position_as_slots, pd.slot_number+ 1.0, true);
        }

    }

    if (scheduled_notes_.size() > 0) {
        DBGLOG("-- sched size = ", scheduled_notes_.size(), " next_sched = " , 
            scheduled_notes_[0].slot_number , "; next_start = ", 
            scheduled_notes_[0].start);
    }

    while(scheduled_notes_.size() > 0) {

        DBGLOG("-- Spinning on scheduled_notes array")
        if ((pd.position_as_slots + slots_in_buffer) > scheduled_notes_[0].start ) {
            // start a new note
            // This is the number of samples into the buffer where the note should turn on
            int offset = juce::jmax(0, int((scheduled_notes_[0].start - pd.position_as_slots) * double(slot_duration)));

            // if the note happens right at the end of the buffer, weird things happen.
            // So, don't play it now, wait for the next buffer.
            if (offset < sample_count - 2) {
                auto msg = maybe_play_note(scheduled_notes_[0].slot_number, scheduled_notes_[0].start );
                if (msg) {
                    DBGLOG("--- Adding msg to buffer at offset ", offset);
                    midiBuffer.addEvent(*msg, offset);
                }
                scheduled_notes_.remove(0);
            } else {
                break;
            }
        } else {
            break;
        }
    }

    //DBGLOG("scheduled notes processed")


    if (pd.is_playing) {
        DBGLOG("END ", "incoming = ", incoming_notes_.size(), " active count = ", active_notes_.size());
    } else if (incoming_notes_.isEmpty() && active_notes_.isEmpty()) {
        // if the incoming midi has no notes that are still
        // playing AND the play head is not moving, then take
        // this opportunity to reset the fake clock.
        fake_clock_sample_count_ = 0;
    } else {
        fake_clock_sample_count_ += sample_count;
    }

    last_block_call_ = juce::Time::currentTimeMillis();

    //DBGLOG("-- Done")


}


//============================================================================
// returns the "speed" of notes based on the value of a quarter note = 1
double PluginProcessor:: getSpeedFactor(double bpm, const juce::AudioPlayHead::TimeSignature &time_sig) {
    auto speed_type = parameters_.speed_type->getIndex();
    double speed_factor = 1;

    double qn_per_beat = (4.0 / time_sig.denominator);

    switch (speed_type) {
        case SpeedType::Note :
            // The array already has the speed relative to a quarter note.
            speed_factor = speed_parameter_values[parameters_.speed->getIndex()].multiplier;
            break;
        case SpeedType::Bar :
        {
            //  3/4 =  3 * (4/4) = 3
            // 12/8 = 12 * (4/8) = 6
            //  2/2 =  2 * (4/2) = 4
            auto qn_per_bar = time_sig.numerator * qn_per_beat;
            speed_factor = qn_per_bar / parameters_.speed_bar->get();
        }
            break;
        case SpeedType::MSec :
        {
            auto ms_per_qn = 1000.0 / ( (bpm / 60.0) * qn_per_beat );
            speed_factor = parameters_.speed_ms->get() / ms_per_qn;
        }
            break;
        default :
            jassertfalse;
    }

    return speed_factor;
}

double PluginProcessor::getGate(double slot) {

    DBGLOG("--- getGate called = ", slot)
    HashRandom rng{"Gate", parameters_.get_random_seed(), slot};
    auto base = parameters_.gate->get() / 100.0f;
    auto range = parameters_.gate_range->get() / 100.0f;

    DBGLOG("---    base = ", base, "; range = ", range)

    if (range > 0 ) {
        auto gate = rng.nextFloat(base-range, base+range);
        DBGLOG("---    returning ", gate)
        return gate;
    } else {
        DBGLOG("---    returning ", base)
        return base;
    }

}

