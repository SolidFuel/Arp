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

};