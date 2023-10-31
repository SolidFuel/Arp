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

#include <juce_audio_processors/juce_audio_processors.h>

#include "HashRandom.hpp"
#include "AlgorithmEnum.hpp"

#include "Starp.hpp"

struct RandomParameters {
    // Even with the L suffix it uses a 32 bit int. So
    // need to cast to int64 intentionally.
    juce::Value seed_value{juce::var{juce::int64{0L}}};

    void pick_new_key() {
        juce::Random rng{};
        seed_value = rng.nextInt64();
    }
};

class AlgorithmBase {

protected:
    int last_index = -1;

public:

    AlgorithmBase() = default;

    virtual int getNextNote(double timeline_slot, const juce::SortedSet<int> &notes, bool notes_changed)  = 0;

    virtual void reset() {
        last_index = -1;
    }

    virtual Algorithm get_algo() const = 0;

    virtual ~AlgorithmBase() {}

};

class UpAlgorithm : public AlgorithmBase {
    using AlgorithmBase::AlgorithmBase;

    Algorithm get_algo() const  { return Algorithm:: Up; }

    int getNextNote(double, const juce::SortedSet<int> &notes, bool) override {
        if (notes.size() > 0) {
            last_index += 1;
            last_index = last_index % notes.size();
            return notes[last_index];
        } else {
            last_index = -1;
            return -1;
        }
    }

};

class DownAlgorithm : public AlgorithmBase {
    using AlgorithmBase::AlgorithmBase;

    Algorithm get_algo() const { return Algorithm:: Down; }

    int getNextNote(double, const juce::SortedSet<int> &notes, bool) override {
        if (notes.size() > 0) {

            if (last_index <= 0) {
                last_index = notes.size();
            }
            last_index -= 1;
            return notes[last_index];
        } else {
            last_index = -1;
            return -1;
        }
    }

};

class RandomAlgorithm : public AlgorithmBase, juce::Value::Listener {

    // Non owning
    RandomParameters *p_;

    juce::int64 key_;

    juce::SortedSet<int> available_notes;

    int last_note = -1;


public:

    RandomAlgorithm(RandomParameters *p) : p_(p) {
        key_ = juce::int64(p_->seed_value.getValue());
        p_->seed_value.addListener(this);
    }

    void valueChanged(juce::Value &) {
        DBGLOG("RandomAlgorithm::valueChanged called")
        key_ = juce::int64(p_->seed_value.getValue());
    }

    juce::int64 getKey() const {
        return key_;
    }

    void reset() override {
        AlgorithmBase::reset();
        last_note = -1;
        available_notes.clearQuick();
    }

    Algorithm get_algo() const { return Algorithm:: Random; }


    int getNextNote(double timeline_slot, const juce::SortedSet<int> &notes, bool notes_changed) override {

        if (available_notes.isEmpty()) {
            available_notes.addSet(notes);
        } else if (notes_changed) {
            available_notes.clearQuick();
            // Add all the stuff from the new set.
            available_notes.addSet(notes);
        }

        int num_notes = available_notes.size();
        DBGLOG("available note count = ", num_notes)

        if (num_notes > 0) {
            if (num_notes == 1) {
                last_note = available_notes[0];
                available_notes.clear();
                return last_note;
            } else {
                last_note = getRandom(timeline_slot, last_note, available_notes);
                available_notes.removeValue(last_note);
                return last_note;
            }
        } else {
            return -1;
        }
    }

    virtual ~RandomAlgorithm() override {
        // We don't own the p_, so it might out live us.
        // explicitly remove the listener.
        p_->seed_value.removeListener(this);
    };

private :
    int getRandom(double slot, int note_to_avoid, const juce::SortedSet<int> & notes) {


        HashRandom rng{"Note", key_, slot};

        for (int j=0; j < 20; ++j) {
            // need to do better, but this is an okay proof of concept
           int maybe =  notes[rng.nextInt(0, notes.size())];
           if (maybe != note_to_avoid)
            return maybe;
        }

        return -1;
    }

};
