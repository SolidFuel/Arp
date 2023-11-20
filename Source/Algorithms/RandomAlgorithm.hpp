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

#include "AlgoBase.hpp"
#include "../AlgorithmParameters.hpp"

#include "../ValueListener.hpp"
#include "../HashRandom.hpp"
#include "../Starp.hpp"



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

    Algorithm get_algo() const override { return Algorithm:: Random; }


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


