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

#include <juce_core/juce_core.h>

#include <string>
#include <memory>

class HashRandom {
    std::unique_ptr<juce::Random> rng_;

    void initialize(const std::uint8_t digest[20]);
    
public:
    HashRandom(const std::uint8_t digest[20]);
    HashRandom(const std::string &category, long long key, double slot);
    int nextInt(int min, int max);
    float nextFloat(float min, float max);

};