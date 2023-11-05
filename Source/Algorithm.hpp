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

#include "Starp.hpp"

#include "HashRandom.hpp"
#include "AlgorithmEnum.hpp"
#include "ValueListener.hpp"
#include "AlgorithmParameters.hpp"
#include <juce_audio_processors/juce_audio_processors.h>


class AlgorithmBase {

public:

    AlgorithmBase() = default;

    virtual int getNextNote(double timeline_slot, const juce::SortedSet<int> &notes, bool notes_changed)  = 0;

    virtual void reset() {
    }

    virtual Algorithm get_algo() const = 0;

    virtual ~AlgorithmBase() {}

};

class LinearAlgorithm : public AlgorithmBase {

private: 
    LinearParameters * p_;

    int direction = LinearParameters::Direction::Up;
    bool zigzag = false;
    bool restart = false;

    int clock = -1;

    int last_slot = 0;


    ValueListener direction_listener_;
    ValueListener zigzag_listener_;
    ValueListener restart_listener_;

    void update_parameters() {
        DBGLOG("LinearAlgorithm::update_parameters called")
        direction = p_->get_direction();
        zigzag = p_->get_zigzag();
        restart = p_->get_restart();
    }

public :
    LinearAlgorithm(LinearParameters * p) : p_{p} {
        update_parameters();

        direction_listener_.onChange = [this](juce::Value &) {
            update_parameters();
        };

        p_->direction.addListener(&direction_listener_);

        zigzag_listener_.onChange = [this](juce::Value &) {
            update_parameters();
        };

        p_->zigzag.addListener(&zigzag_listener_);

        restart_listener_.onChange = [this](juce::Value &) {
            update_parameters();
        };

        p_->restart.addListener(&restart_listener_);
    }

    ~LinearAlgorithm() {
        p_->direction.removeListener(&direction_listener_);
        p_->zigzag.removeListener(&zigzag_listener_);
        p_->restart.removeListener(&restart_listener_);
    }

    Algorithm get_algo() const { return Algorithm::Linear; }

    int getNextNote(double slot, const juce::SortedSet<int> &notes, bool notes_changed) override {

        DBGLOG("Linear GETNEXTNOTE called slot = ", int(slot), " changed = ", notes_changed)

        auto note_count = notes.size();

        if (note_count == 0) {
            return -1;
        } else if (note_count == 1) {
            return notes[0];
        }

        if (notes_changed) {
            clock = 0;
        } else {
            // This is to make up for the fact that if
            // the probability function decides to not play
            // a note in slot, we won't get called. But we want
            // the clock to 'tick' in time with the slot even
            // if we don't choose a note.
            clock += int(slot) - last_slot;
        }
        last_slot = int(slot);

        clock = restart ? clock : int(slot);

        DBGLOG("   GNN notes = ", note_count," clock = ", clock, " restart = ", restart);


        int index = 0;

        if (zigzag) {
            // -2 because we don't want to repeat the top and bottom.
            auto cycle_length = 2 * note_count - 2;

            index = clock % cycle_length;
            DBGLOG("   GNN zigzag cl = ", cycle_length, " index1 = ", index )
            auto correction = ((index - (note_count-1))*2);
            auto condition = index >= note_count;
            index -= condition * correction;
            DBGLOG("   GNN zigzag" , " corr = ", correction, " cond = ", condition, " index = ", index)

        } else {
            index = clock % note_count;
        }

        if (direction == LinearParameters::Direction::Down) {
            index = note_count - 1 - index;
        }

        return notes[index];
    }
};

//===================================================================
class RandomAlgorithm : public AlgorithmBase {

    // Non owning
    RandomParameters *p_;

    juce::int64 seed_;
    bool replace_;

    juce::SortedSet<int> available_notes;

    int last_note = -1;


public:

    RandomAlgorithm(RandomParameters *p) : p_(p) {

        seed_listener_.onChange = [this](juce::Value &) {
            update_parameters();
        };
        p_->seed_value.addListener(&seed_listener_);

        replace_listener_.onChange = [this](juce::Value &) {
            update_parameters();
        };        
        p_->replace.addListener(&replace_listener_);

        update_parameters();
    }

    void reset() override {
        AlgorithmBase::reset();
        last_note = -1;
        available_notes.clearQuick();
    }

    Algorithm get_algo() const { return Algorithm:: Random; }


    int getNextNote(double timeline_slot, const juce::SortedSet<int> &notes, bool notes_changed) override {

        DBGLOG("Random GETNEXTNOTE called slot = ", int(timeline_slot), " changed = ", notes_changed)

        if (notes_changed || available_notes.isEmpty()) {
            available_notes.clearQuick();
            available_notes.addSet(notes);
        }

        int num_notes = available_notes.size();
        DBGLOG("available note count = ", num_notes)

        if (num_notes == 0) {
            return -1;
        } 
        
        if ( num_notes == 1 ) {
            last_note = available_notes[0];
            available_notes.clear();
            return last_note;
        }

        last_note = getRandom(timeline_slot, last_note, available_notes);
        if (!replace_) {
            available_notes.removeValue(last_note);
        }
        return last_note;
    }

    virtual ~RandomAlgorithm() override {
        // We don't own the p_, so it might out live us.
        // explicitly remove the listener.
        p_->seed_value.removeListener(&seed_listener_);
        p_->replace.removeListener(&replace_listener_);
    };

private :

    ValueListener seed_listener_;
    ValueListener replace_listener_;

    void update_parameters() {
        DBGLOG("RandomAlgorithm::update_parameters called")
        seed_ = p_->get_seed();
        DBGLOG("RandomAlgorithm::update_parameters seed done")
        replace_ = p_->get_replace();
        DBGLOG("RandomAlgorithm::update_parameters replace done")
    }


    int getRandom(double slot, int note_to_avoid, const juce::SortedSet<int> & notes) {


        HashRandom rng{"Note", seed_, slot};

        for (int j=0; j < 20; ++j) {
            // need to do better, but this is an okay proof of concept
           int maybe =  notes[rng.nextInt(0, notes.size())];
           if (maybe != note_to_avoid)
            return maybe;
        }

        return -1;
    }

};
