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
#include "../Starp.hpp"

class SpiralAlgorithm : public AlgorithmBase {

private: 
    SpiralParameters * p_;

    int direction = SpiralParameters::Direction::In;
    int start_position = SpiralParameters::StartPosition::Top;

    int clock_ = -1;

    int last_slot = 0;

    juce::Array<int> index_map_;


    ValueListener direction_listener_;
    ValueListener start_position_listener_;

    void update_parameters() {
        DBGLOG("SpiralAlgorithm::update_parameters called")
        direction = p_->get_direction();
        start_position = p_->get_start_position();
        index_map_.clearQuick();
        DBGLOG("   direction = ", direction, "; start =", start_position)
    }

public :
    SpiralAlgorithm(SpiralParameters * p) : p_{p} {
        update_parameters();

        direction_listener_.onChange = [this](juce::Value &) {
            update_parameters();
        };

        p_->direction.addListener(&direction_listener_);

        start_position_listener_.onChange = [this](juce::Value &) {
            update_parameters();
        };

        p_->start_position.addListener(&start_position_listener_);

    }

    virtual ~SpiralAlgorithm() override {
        p_->direction.removeListener(&direction_listener_);
        p_->start_position.removeListener(&start_position_listener_);
    }

    Algorithm get_algo() const override { return Algorithm::Spiral; }

    void reset() override {
        index_map_.clearQuick();
        clock_ = 0;
    }

    int getNextNote(double slot, const juce::SortedSet<int> &notes, bool notes_changed) override {

        auto note_count = notes.size();

        DBGLOG("Spiral GETNEXTNOTE called slot = ", int(slot), "; changed = ", notes_changed, "; count = ", note_count)

        if (note_count == 0) {
            return -1;
        } else if (note_count == 1) {
            return notes[0];
        }

        if (notes_changed) {
            clock_ = 0;
        } else {
            // This is to make up for the fact that if
            // the probability function decides to not play
            // a note in slot, we won't get called. But we want
            // the clock to 'tick' in time with the slot even
            // if we don't choose a note.
            clock_ += int(slot) - last_slot;
        }
        last_slot = int(slot);

        DBGLOG("   GNN notes = ", note_count," clock = ", clock_ );

        if (notes_changed || index_map_.isEmpty()) {
            index_map_.clearQuick();
            switch (direction) {
                using SD = SpiralParameters::Direction;
                case SD::In :
                    render_in_algorithm(note_count, false);
                    break;
                case SD::Out :
                    render_out_algorithm(note_count, false);
                    break;
                case SD::InOut :
                    render_in_algorithm(note_count, false);
                    render_out_algorithm(note_count, true);
                    break;
                case SD::OutIn :
                    render_out_algorithm(note_count, false);
                    render_in_algorithm(note_count, true);
                    break;
            }
        }


        int map_index = clock_ % index_map_.size();
        int index = index_map_[map_index];
        DBGLOG("   GNN : map_index = ", map_index, "; index = ", index)

        return notes[index];
    }

private :
    void render_in_algorithm(int note_count, bool second) {
        DBGLOG("SpiralAlogrithm::render_in_algorithm called.")
        DBGLOG("    ria: position = ", start_position, "; note_count = ", note_count, "; second = ", second)

        int pointers[2] = {
            0, note_count-1
        };

        int added = 0;
        int position = start_position;

        DBGLOG("    ria: p0 = ", pointers[0], "; p1 = ", pointers[1], "; position = ", position)

        while (added < note_count ) {

            // if we are the second half, skip our first note
            if (!second || (added > 0))
                index_map_.add(pointers[position]);
            added += 1;
            pointers[position] += (1-2*position);
            position = 1-position;
            DBGLOG("    ria: p0 = ", pointers[0], "; p1 = ", pointers[1], "; position = ", position)
        }
    }
        
    void render_out_algorithm(int note_count, bool second) {
        DBGLOG("SpiralAlogrithm::render_out_algorithm called.")
        DBGLOG("    roa: start position = ", start_position, "; note_count = ", note_count, "; second = ", second)

        using SP = SpiralParameters::StartPosition;

        int mid = (note_count -1)/ 2;
        bool odd = (note_count %2 == 1);

        int pointers[2];

        if (start_position == SP::Bottom) {
            pointers[SP::Bottom] = mid;
            pointers[SP::Top] = mid + 1;
        } else {
            pointers[SP::Bottom] = mid - odd;
            pointers[SP::Top] = mid + (!odd);
        }


        int added = 0;
        int position;
        if (odd) {
            position = start_position;
        } else {
            position = 1 - start_position;
        }

        DBGLOG("    roa: mid = ", mid, "; odd = ", odd, "; p0 = ", pointers[0], "; p1 = ", pointers[1], "; position = ", position)

        while (added < note_count ) {

            // if we are the second half, skip our first note
            if (!second || (added > 0))
                index_map_.add(pointers[position]);
            added += 1;
            pointers[position] -= (1-2*position);
            position = 1-position;
            DBGLOG("    roa: p0 = ", pointers[0], "; p1 = ", pointers[1], "; position = ", position)
        }

    }
};
