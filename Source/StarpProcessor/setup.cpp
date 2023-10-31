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


#if STARP_DEBUG
    dbgout = juce::FileLogger::createDateStampedLogger("Starp", "StarpLogFile", ".txt", "--------V2--------");
#endif

    seed_listener_.onChange = [this](juce::Value &) { seed_changed = true; };
    parameters.random_parameters.seed_value.addListener(&seed_listener_);

    algo_listener_.onChange = [this](juce::Value &) { algo_changed = true; };
    parameters.algorithm_index.addListener(&algo_listener_);

    DBGLOG("Finished Processor Constructor")

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

    DBGLOG("------- Setting Up Editor -----------");
    return new StarpEditor (*this);
}


//============================================================================
// Serialize Parameters for the host to save for us.
//
void StarpProcessor::getStateInformation (juce::MemoryBlock& destData) {

    DBGLOG("GET STATE called");

    auto state = parameters.apvts->copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->setAttribute("key", juce::String{parameters.get_random_seed()});
    xml->setAttribute("algorithm", parameters.get_algo_index());
    DBGLOG("XML out =", xml->toString());
    copyXmlToBinary(*xml, destData);

}

//============================================================================
// Read Serialize Parameters from the host and set our state.
//
void StarpProcessor::setStateInformation (const void* data, int sizeInBytes) {
    DBGLOG("SET STATE called");

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr) {
        DBGLOG("XML in =", xmlState->toString());

        if (xmlState->hasTagName(parameters.apvts->state.getType())) {
            parameters.apvts->replaceState(juce::ValueTree::fromXml(*xmlState));
            parameters.random_parameters.seed_value = xmlState->getStringAttribute("key", "-42").getLargeIntValue();
            auto algo = xmlState->getIntAttribute("algorithm", Algorithm::Random);
            DBGLOG("   algorithm = ", algo)
            parameters.algorithm_index.setValue(algo);
            DBGLOG("   value = ", parameters.get_algo_index())
        }
    }  
}

//============================================================================
void StarpProcessor::update_algo_cb() {
    DBGLOG("StarpProcessor::update_algo_cb called");
    algo_changed = true;
}

//============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StarpProcessor();
}
