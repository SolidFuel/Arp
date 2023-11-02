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

#if STARP_DEBUG
    std::unique_ptr<juce::FileLogger> dbgout = 
        std::unique_ptr<juce::FileLogger>(juce::FileLogger::createDateStampedLogger("Starp", "StarpLogFile", ".txt", "--------V2--------"));
#endif


StarpProcessor::StarpProcessor() : /* juce::AudioProcessor(getDefaultProperties()), */
        parameters(*this) {



    algo_listener_.onChange = [this](juce::Value &) { algo_changed_ = true; };
    parameters.algorithm_index.addListener(&algo_listener_);

    DBGLOG("Finished Processor Constructor")

}

StarpProcessor::~StarpProcessor() {

    DBGLOG("StarpProcessor destructor called")
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

constexpr int CURRENT_STATE_VERSION = 2;
constexpr char* XML_TOP_TAG = "Starp-Preset";

void StarpProcessor::getStateInformation (juce::MemoryBlock& destData) {

    DBGLOG("GET STATE called");

    auto xml = std::make_unique<juce::XmlElement>(XML_TOP_TAG);
    xml->setAttribute("version", CURRENT_STATE_VERSION);

    DBGLOG("  Created Top")

    //--------------------------------------
    auto state = parameters.apvts->copyState();
    auto apvts_xml =state.createXml();
    // createXml gives back a unqiue_ptr. So we need to unwrap it.
    xml->addChildElement(apvts_xml.release());
    DBGLOG("  Wrote ValueTree")

    //--------------------------------------
    xml->setAttribute("key", juce::String{parameters.get_random_seed()});
    xml->setAttribute("algorithm", parameters.get_algo_index());
    DBGLOG("  Wrote attributes")

    //--------------------------------------
    auto *child = xml->createNewChildElement("LinearParameters");
    child->setAttribute("direction", parameters.linear_parameters.get_direction());
    child->setAttribute("zigzag", parameters.linear_parameters.get_zigzag());
    child->setAttribute("restart", parameters.linear_parameters.get_restart());
    DBGLOG("  Wrote LinearParameters")

    //--------------------------------------
    DBGLOG("XML out =", xml->toString());
    copyXmlToBinary(*xml, destData);
    DBGLOG("  Done")

}

//============================================================================
void StarpProcessor::parseCurrentXml(const juce::XmlElement * elem) {

    DBGLOG("StarpProcessor::parseCurrentXml called")

    auto *child = elem->getChildByName(parameters.apvts->state.getType());
    if (child) {
        parameters.apvts->replaceState(juce::ValueTree::fromXml(*child));
    }

    child = elem->getChildByName("LinearParameters");
    if (child) {
        parameters.linear_parameters.direction = 
            child->getIntAttribute("direction", LinearParameters::Direction::Up);
        parameters.linear_parameters.zigzag = 
            child->getBoolAttribute("zigzag", false);
        parameters.linear_parameters.restart = 
            child->getBoolAttribute("restart", false);
    }

    parameters.random_parameters.seed_value = 
        elem->getStringAttribute("key", juce::String{parameters.get_random_seed()})
            .getLargeIntValue();
    parameters.algorithm_index = 
        elem->getIntAttribute("algorithm", Algorithm::Random);

}

//============================================================================
void StarpProcessor::parseOriginalXml(const juce::XmlElement * xml) {

        DBGLOG("StarpProcessor::parseOriginalXml called")

        if (xml->hasTagName(parameters.apvts->state.getType())) {
            parameters.apvts->replaceState(juce::ValueTree::fromXml(*xml));

            auto child = xml->getChildByName("LinearParameters");
            if (child) {
                parameters.linear_parameters.direction = 
                    child->getIntAttribute("direction", LinearParameters::Direction::Up);
                parameters.linear_parameters.zigzag = 
                    child->getBoolAttribute("zigzag", false);
            }

            parameters.random_parameters.seed_value = 
                xml->getStringAttribute("key", juce::String{parameters.get_random_seed()})
                        .getLargeIntValue();
            
            auto algo = xml->getIntAttribute("algorithm", Algorithm::Random);
            DBGLOG("   input algo = ", algo)
            if (algo == Algorithm::Down ) {
                algo = Algorithm::Linear;
                parameters.linear_parameters.direction = LinearParameters::Direction::Down;
                parameters.linear_parameters.zigzag = false;
            } else if (algo == Algorithm::Up ) {
                algo = Algorithm::Linear;
                parameters.linear_parameters.direction = LinearParameters::Direction::Up;
                parameters.linear_parameters.zigzag = false;
            }
            parameters.algorithm_index.setValue(algo);
            DBGLOG("   final algo = ", parameters.get_algo_index())
        }
}

//============================================================================
// Read Serialize Parameters from the host and set our state.
//
void StarpProcessor::setStateInformation (const void* data, int sizeInBytes) {
    DBGLOG("SET STATE called");

    auto xml = getXmlFromBinary(data, sizeInBytes);

    if (xml) {
        DBGLOG("XML in =", xml->toString());

        if (xml->hasTagName(XML_TOP_TAG)) {
            int version = xml->getIntAttribute("version");
            if (version == CURRENT_STATE_VERSION) {
                parseCurrentXml(xml.get());
            } else {
                jassert(false);
            }

        } else {
            parseOriginalXml(xml.get());
        }

    }  else {
        DBGLOG("   NO XML decoded")
    }
}

//============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StarpProcessor();
}
