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

#pragma once

#include <cmath>

//============================================================================
struct position_data {

    // original position as given by the play head
    double qn_position = -1.0;

    double position_as_slots = -1.0;
    // slot_number is a whole number
    // slot_number + slot_fraction == position_as_slot
    double slot_fraction = 0.0;
    double slot_number = -1.0;

    int samples_per_qn;
    bool is_playing = false;

    // This represents how long the plugin has been active
    // in hires timer ticks. We use this to compute the stop
    // time for notes independent of the playhead.
    int64_t tick_count = 0;

    int64_t ticks_per_slot;

    // speed should be realtive to the quarter note.
    double speed = 1.0;

    // **NOTE** set speed before calling this function!!
    //
    void set_position(double new_position) {

        qn_position = new_position;

        position_as_slots = new_position / speed;

        slot_number = std::floor(position_as_slots);

        // how far along in the current slot are we ?
        slot_fraction = position_as_slots - slot_number;
        if (slot_fraction < 0.00001) {
            slot_fraction = 0.0;
        } else if (slot_fraction > 0.99999 ) {
            // floor(x) < x always. So, if x is whole number
            // we'll get the next lowest whole number.
            // This code tries to compensates for this.
            slot_fraction = 0.0;
            slot_number += 1;
        }


       
    }
};
