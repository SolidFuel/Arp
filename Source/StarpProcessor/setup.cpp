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


juce::PluginHostType StarpProcessor::host_type;

StarpProcessor::StarpProcessor() : /* juce::AudioProcessor(getDefaultProperties()), */
        parameters(*this) {


    algo_index = Algorithm::Random;

#if STARP_DEBUG
    dbgout = juce::FileLogger::createDateStampedLogger("Starp", "StarpLogFile", ".txt", "--------V2--------");
#endif

}

StarpProcessor::~StarpProcessor() {

    if (dbgout != nullptr) {
        delete dbgout;
        dbgout = nullptr;
    }

}

bool StarpProcessor::isMidiEffect() const {
    return host_type.isReaper();
};

juce::AudioProcessor::BusesProperties StarpProcessor::getDefaultProperties() {
    auto retval = BusesProperties();

    if (! host_type.isReaper()) {
        // Unless we are on reaper, act as if we are a synth.
        retval =  retval.withOutput("Output", juce::AudioChannelSet::stereo(), true);
    }
    
    return retval;
}


juce::AudioProcessorEditor* StarpProcessor::createEditor() {
    return new StarpEditor (*this);
}


//============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StarpProcessor();
}
