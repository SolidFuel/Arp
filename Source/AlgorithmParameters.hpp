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

struct LinearParameters {
    enum Direction { Up, Down };

    juce::Value direction{Up};
    int get_direction() const { return int(direction.getValue()); }

    juce::Value zigzag{juce::var{false}};
    bool get_zigzag() const { return bool(zigzag.getValue()); }

    juce::Value restart{juce::var{false}};
    bool get_restart() const { return bool(restart.getValue()); }
};
