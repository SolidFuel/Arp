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

#include "../StarpProcessor.hpp"
#include "../StarpEditor.hpp"
#include "../Starp.hpp"

#include <iomanip>
#include <cmath>

juce::FileLogger *dbgout = nullptr;

speed_value speed_parameter_values[] = {
    speed_value{"1/16"  , 0.25},
    speed_value{"1/8t"  , 0.33333333 },
    speed_value{"1/16d" , 0.375},
    speed_value{"1/8"   , 0.50},
    speed_value{"1/4t"  , 0.66666667 },
    speed_value{"1/8d"  , 0.75},
    speed_value{"1/4"   , 1.0 },
    speed_value{"1/2t"  , 1.33333333 },
    speed_value{"1/4d"  , 1.5 },
    speed_value{"1/2"   , 2.0 },
};


constexpr int default_algo_index = Algorithm::Random;

//============================================================================
// PLAYED_NOTE methods
//============================================================================
bool operator==(const played_note& lhs, const played_note& rhs) {
    return lhs.note_value == rhs.note_value;
}

bool operator<(const played_note& lhs, const played_note& rhs) {
    return lhs.note_value < rhs.note_value;
}


bool operator==(const schedule& lhs, const schedule& rhs){ return lhs.start == rhs.start; }
bool operator<(const schedule& lhs, const schedule& rhs) { return lhs.start < rhs.start; }



    

//============================================================================
void StarpProcessor::getStateInformation (juce::MemoryBlock& destData) {

    DBGLOG("GET STATE called");

    auto state = parameters.apvts->copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->setAttribute("key", juce::String{parameters.random_key_});
    copyXmlToBinary(*xml, destData);

}

void StarpProcessor::setStateInformation (const void* data, int sizeInBytes) {
    DBGLOG("SET STATE called");

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(parameters.apvts->state.getType())) {
            parameters.apvts->replaceState(juce::ValueTree::fromXml(*xmlState));
            if (xmlState->hasAttribute("key")) {
                parameters.random_key_ = xmlState->getStringAttribute("key", "-42").getLargeIntValue();
            }            
        }
    }  
}



//============================================================================
//============================================================================
void StarpProcessor::prepareToPlay (double sampleRate, int) {

    DBGLOG("PREPARE called");
    sample_rate_ = sampleRate;
    last_position_ = -1;

    // This will finesse the bypass logic so we don't reset again.
    last_block_call_ = juce::Time::currentTimeMillis();

    reset_data();

}

void StarpProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

    DBGLOG("RELEASE called");
    incoming_notes_.clear();
    active_notes_.clear();

}

//============================================================================
void StarpProcessor::reassign_algorithm(int new_algo) {
    if (new_algo != current_algo_index_) {
        DBGLOG("Changing to new algorithm # ", new_algo);
        switch (new_algo) {
            case Algorithm::Random :
                {
                    auto *na = new RandomAlgorithm(algo_obj_.get());
                    na->setKey(parameters.random_key_);
                    na->setDebug(dbgout);
                    algo_obj_.reset(na);
                }
                break;
            case Algorithm::Up :
                algo_obj_ = std::make_unique<UpAlgorithm>(algo_obj_.get());
                break;
            case Algorithm::Down :
                algo_obj_ = std::make_unique<DownAlgorithm>(algo_obj_.get());
                break;
        }
        current_algo_index_ = new_algo;
    } else if (current_algo_index_ == Algorithm::Random) {
        // Not great that this is done each block,
        // but not that many cycles I suppose.
        auto *ra = dynamic_cast<RandomAlgorithm *>(algo_obj_.get());
        ra->setKey(parameters.random_key_);
    }

}

//============================================================================
const position_data StarpProcessor::compute_block_position() {

    position_data pd{};

    double bpm = 120.0;
    double quarter_notes_per_beat = 1;

    auto *play_head = getPlayHead();

    if (play_head) {
        auto position = play_head->getPosition();
        if (position) {
            pd.is_playing = position->getIsPlaying();

            auto hostBpm = position->getBpm();
            if (hostBpm) {
                bpm = *hostBpm;
            }
            auto time_sig = position->getTimeSignature();
            if (time_sig) {
                quarter_notes_per_beat = 4.0 / time_sig->denominator;
            }

            auto opt_pos_qn = position->getPpqPosition();
            if (opt_pos_qn) {
                // position in slots
                auto pos_in_slots = *opt_pos_qn / getSpeedFactor();
                pd.set_position(pos_in_slots);
            }
        }
    }

    pd.samples_per_qn =  static_cast<int>(std::ceil((sample_rate_ * 60.0) / (bpm * quarter_notes_per_beat)));

    if (! pd.is_playing ) {
        // need to synthesize the data
        pd.set_position(fake_clock_sample_count_ / (pd.samples_per_qn * getSpeedFactor()));
    }

    return pd;

}

//============================================================================
std::optional<juce::MidiMessage> StarpProcessor::maybe_play_note(
    bool notes_changed, double for_slot, double start_pos) {


    if (incoming_notes_.size() == 0) {
        algo_obj_->reset();
        return std::nullopt;
    }

    int note_prob = parameters.probability->get();

    HashRandom prob_rng{"Probability", parameters.random_key_, for_slot};
    if (prob_rng.nextInt(0, 101) > note_prob ) {
        return std::nullopt;
    }

    std::optional<juce::MidiMessage> retval;

    int new_note = algo_obj_->getNextNote(for_slot, incoming_notes_, notes_changed);
    if (new_note >= 0 ) {

        int range = parameters.velo_range->get();
        int note_velocity = parameters.velocity->get();

        if (range > 0)  {
            HashRandom vel_rng{"Velocity", parameters.random_key_, for_slot};


            int max = juce::jmin(128, note_velocity + range); 
            int min = juce::jmax(1, note_velocity - range);

            note_velocity = vel_rng.nextInt(min, max);
        }

        retval = juce::MidiMessage::noteOn(
                1, new_note, (std::uint8_t) note_velocity
            );
        
        double end_slot = start_pos + getGate();
        active_notes_.add({new_note, end_slot});
        DBGLOG("--- start ", new_note, " @ ", start_pos, "; end = ", end_slot)

    } else {
        DBGLOG("--- algo could not find note to start");
    }

    return retval;
}

void StarpProcessor::schedule_note(double current_pos, double slot_number, bool can_advance) {
    HashRandom rng{"Humanize", parameters.random_key_, slot_number};

    auto advance = parameters.timing_advance->get() * can_advance;
    float variance = rng.nextFloat(
            advance,
            parameters.timing_delay->get());

    double sched_start = slot_number + (variance/100.0);

    if (current_pos <= sched_start) {
        last_scheduled_slot_number = slot_number;
        scheduled_notes_.addUsingDefaultSort({slot_number, sched_start});
        DBGLOG("--- scheduling slot ", slot_number, " @ ", sched_start);
    } else  {
        DBGLOG("--- tried to schedule slot ", slot_number,
            " @ ", sched_start, " but late (", current_pos,  ")");
    }

}

//============================================================================
void StarpProcessor::reset_data() {
    DBGLOG("   reset_data called");

    last_scheduled_slot_number = -1.0;
    scheduled_notes_.clearQuick();
    incoming_notes_.clearQuick();
    active_notes_.clearQuick();
    if (algo_obj_) {
        algo_obj_->reset();
    }

    DBGLOG("   reset_data finished"); 
}

//============================================================================
void StarpProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                    juce::MidiBuffer& midiBuffer) {
    auto numSamples = buffer.getNumSamples();

    // A pure MIDI plugin shouldn't be provided any audio data
    // but Ableton Live (and others) can't handle a pure midi effect.
    // jassert (buffer.getNumChannels() == 0);
    // In Ableton (and others) we actually have an audio output.
    // So we need to zero it.

    for (auto i = 0; i < buffer.getNumChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    reassign_algorithm(algo_index);

    position_data pd = compute_block_position();


    double slots_in_buffer = (double(numSamples) / double(pd.samples_per_qn)) / getSpeedFactor();
    
    auto slot_duration = static_cast<int>(std::ceil(double(pd.samples_per_qn) * getSpeedFactor()));

    auto this_call_time = juce::Time::currentTimeMillis();

    bool do_cleanup = false;

    if (this_call_time - last_block_call_ > 100) {
        DBGLOG("--- Was Bypassed - resetting");
        do_cleanup = true;
    }

    if (pd.is_playing != last_play_state_) {
        DBGLOG("--- Play state changed to ", pd.is_playing);
        last_play_state_ = pd.is_playing;
        do_cleanup = true;
    }

    if (pd.position_as_slots < last_position_) {
        // we must have looped without stopping.
        // clear everything and restart. Do we need to send note-offs ?
        // Assume not for now.
        DBGLOG("--- LOOPING - resetting data");
        do_cleanup = true;
    }
    last_position_ = pd.position_as_slots;

    if ( do_cleanup) {
        reset_data();
    }

#if STARP_DEBUG
    if (pd.is_playing || !midiBuffer.isEmpty() || !incoming_notes_.isEmpty() || !active_notes_.isEmpty() ) {
        std::stringstream x;
        x << "START playing = " << pd.is_playing
            << ";\n     pos_as_slots = " << pd.position_as_slots << "; slot_number = " << pd.slot_number
            << "; slot_fraction = " << std::setprecision(10) << pd.slot_fraction 
            << ";\n     gate = " << getGate() << "; slots_in_buffer = " << slots_in_buffer;
        dbgout->logMessage(x.str());
    }
#endif


    // === Read Midi Messages and update incoming_notes_ set
    bool notes_changed = false;

    juce::MidiBuffer newBuffer;

    for (const auto metadata : midiBuffer)
    {
        const auto msg = metadata.getMessage();
        if      (msg.isNoteOn())  { incoming_notes_.add(msg.getNoteNumber()); notes_changed = true; }
        else if (msg.isNoteOff()) { 
            if (incoming_notes_.contains(msg.getNoteNumber())) {
                incoming_notes_.removeValue(msg.getNoteNumber());
                notes_changed = true; 
            } else {
                // if we didn't see the NoteOn assume we were bypassed and put the message back in the buffer.
                newBuffer.addEvent(msg, metadata.samplePosition);
            }
        } else {
            newBuffer.addEvent(msg, metadata.samplePosition);
        }
    }

    midiBuffer.swapWith(newBuffer);

    DBGLOG("    notes_changed = ", notes_changed);

    juce::Array<played_note> new_notes{};

    //------------------------------------------------------------------------
    // Stop any notes that will end during this buffer
    for (int idx = 0; idx < active_notes_.size(); ++idx) {

        auto thisNote = active_notes_.getUnchecked(idx);
        auto note_value = thisNote.note_value;
        auto end_slot = thisNote.end_slot;

        if ( (pd.position_as_slots + slots_in_buffer) > end_slot ) {
            // This is the number of samples into the buffer where the note should turn off.
            // If the arp speed was changed while the note was playing, it is possible that
            // the offset could be calculated as negative. The jmin is to choose instead to
            // end it immediately.
            // Note -- to make this work really correctly, we need to fix the off position
            // on something other than slots. Say - the "qn" position instead.
            int offset = juce::jmin(0, int((end_slot - pd.position_as_slots) * slot_duration));
            DBGLOG("stop ", note_value, " @ ", offset);
            midiBuffer.addEvent(juce::MidiMessage::noteOff (1, note_value), offset);

        } else {
            // we didn't stop it, so copy to the new list
            new_notes.add(thisNote);
        }
    }

    active_notes_.clearQuick();
    active_notes_.addArray(new_notes);

    //------------------------------------------------------------------------
    // Schedule note if required.
    if (! incoming_notes_.isEmpty()) {
        if (last_scheduled_slot_number < pd.slot_number) {
            schedule_note(pd.position_as_slots, pd.slot_number, false);
        }
        if ( (pd.slot_fraction > 0.0) &&  last_scheduled_slot_number <= pd.slot_number) {
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
            if (offset < numSamples - 2) {
                auto msg = maybe_play_note(notes_changed, scheduled_notes_[0].slot_number, scheduled_notes_[0].start );
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

    if (pd.is_playing) {
        DBGLOG("END ", "active count ", active_notes_.size());
    } else if (incoming_notes_.isEmpty()) {
        // if the incoming midi has no notes that are still
        // playing AND the play head is not moving, then take
        // this opportunity to reset the fake clock.
        fake_clock_sample_count_ = 0;
    } else {
        fake_clock_sample_count_ += numSamples;
    }

    last_block_call_ = juce::Time::currentTimeMillis();

}


//============================================================================

double StarpProcessor:: getSpeedFactor() {
    return speed_parameter_values[parameters.speed->getIndex()].multiplier;
}

double StarpProcessor::getGate() {
    return parameters.gate->get() / 100.0;
}

