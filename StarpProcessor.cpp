#include "StarpProcessor.hpp"
#include "StarpEditor.hpp"
#include "Starp.hpp"

#include <iomanip>

speed_value speed_parameter_values[] = {
    speed_value{"1/16", 0.25},
    speed_value{"1/8" , 0.50},
    speed_value{"1/4" , 1.0 },
    speed_value{"1/2" , 2.0 },
};

constexpr int default_speed = 2;

constexpr int default_algo_index = 0; // UpAlgorithm

//============================================================================
// PLAYED_NOTE methods
//============================================================================
bool operator==(const played_note& lhs, const played_note& rhs) {
    return lhs.note_value == rhs.note_value;
}

bool operator<(const played_note& lhs, const played_note& rhs) {
    return lhs.note_value < rhs.note_value;
}




//============================================================================
    
StarpProcessor::StarpProcessor() : AudioProcessor (BusesProperties()) {

    // Pick a random key
    juce::Random rng{};
    randomKey = rng.nextInt64();

    // Set up the Audio Parameters
    addParameter(algorithm_parm = new juce::AudioParameterChoice({"algorithm", 3}, "Algorithm", {"up", "down", "random"}, default_algo_index));

    juce::StringArray choices;
    for (auto const &v : speed_parameter_values) {
        choices.add(v.name);
    }
    speed = new juce::AudioParameterChoice({"speed", 1}, "Speed", choices, default_speed);
    addParameter(speed);

    addParameter(probability    = new juce::AudioParameterInt({"probability", 4}, "Probability", 0, 100, 100));
    addParameter(gate           = new juce::AudioParameterFloat({ "gate", 2 },  "Gate %", 10.0, 200.0, 100.0));
    addParameter(velocity       = new juce::AudioParameterInt({"velocity", 5}, "Velocity", 1, 127, 100));
    addParameter(velo_range     = new juce::AudioParameterInt({"vel. range", 6}, "Vel. Range", 0, 64, 0));
    addParameter(timing_delay   = new juce::AudioParameterFloat({ "timing_delay", 7 },   "Delay",    0.0, 30.0, 0.0));
    addParameter(timing_advance = new juce::AudioParameterFloat({ "timing_advance", 8 }, "Advance", -30.0, 0.0, 0.0));

    current_algo_index = -1;

#if STARP_DEBUG
    dbgout = juce::FileLogger::createDateStampedLogger("Starp", "StarpLogFile", ".txt", "----V40---");
#endif

}

StarpProcessor::~StarpProcessor() {
    releaseResources();

    if (algo != nullptr) {
        delete algo;
        algo = nullptr;
    }

    if (dbgout != nullptr) {
        delete dbgout;
        dbgout = nullptr;
    }

}

//============================================================================
void StarpProcessor::getStateInformation (juce::MemoryBlock& destData) {

#if STARP_DEBUG
    dbgout->logMessage("GET STATE called");
#endif
    std::stringstream hash;

    hash << std::hex << randomKey;

    std::unique_ptr<juce::XmlElement> xml (new juce::XmlElement ("StarpStateInfo"));
    xml->setAttribute ("version", (int) 1);
    xml->setAttribute ("speed", (int) *speed);
    xml->setAttribute ("algorithm", (int) *algorithm_parm);
    xml->setAttribute ("gate", *gate); 
    xml->setAttribute("key", juce::String{randomKey});
    xml->setAttribute("velocity", *velocity);
    xml->setAttribute("velocity_range", *velo_range);
    xml->setAttribute("probability", *probability);

#if STARP_DEBUG
    dbgout->logMessage(xml->toString());
#endif
    copyXmlToBinary (*xml, destData);

}

void StarpProcessor::setStateInformation (const void* data, int sizeInBytes) {
#if STARP_DEBUG
    dbgout->logMessage("SET STATE called");
#endif

    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName ("StarpStateInfo"))  {
#if STARP_DEBUG
            dbgout->logMessage(xmlState->toString());
#endif
            *speed          = xmlState->getIntAttribute("speed", 2);
            *algorithm_parm = xmlState->getIntAttribute ("algorithm", 0);
            *gate           = (float) xmlState->getDoubleAttribute ("gate", 100.0);
            *velocity       = xmlState->getIntAttribute("velocity", 100);
            *velo_range     = xmlState->getIntAttribute("velocity_range", 0);
            *probability    = xmlState->getIntAttribute("probability", 100);

            if (xmlState->hasAttribute("key")) {
                randomKey = xmlState->getStringAttribute("key", "-42").getLargeIntValue();
            }
        }
    }
}



//============================================================================
//============================================================================
void StarpProcessor::prepareToPlay (double sampleRate, int) {

#if STARP_DEBUG
    dbgout->logMessage("PREPARE called");
#endif
    notes.clear();
    rate = sampleRate;

    if (active_notes == nullptr) {
        active_notes = new juce::Array<played_note>();
    } else {
        active_notes->clearQuick();
    }

}

void StarpProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

#if STARP_DEBUG
    dbgout->logMessage("RELEASE called");
#endif

    if (active_notes != nullptr) {
        delete active_notes;
        active_notes = nullptr;
    }

}

//============================================================================

void StarpProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages) {
    // A pure MIDI plugin shouldn't be provided any audio data
    jassert (buffer.getNumChannels() == 0);

    // however we use the buffer to get timing information
    auto numSamples = buffer.getNumSamples();

    double bpm = 120.0;
    double qpb = 1; // quarter notes per beat
    double slot_fraction = 0.0;
    double slots = -1.0;
    bool is_playing = false;

    int new_algo = *algorithm_parm;

    if (new_algo != current_algo_index) {
        AlgorithmBase *tmp = algo ;
        switch (new_algo) {
            case 0 :
                algo = new UpAlgorithm(tmp);
                break;
            case 1 :
                algo = new DownAlgorithm(tmp);
                break;
            case 2 :
                auto *na = new RandomAlgorithm(tmp);
                na->setKey(randomKey);
                na->setDebug(dbgout);
                algo = na;
                break;
        }
        delete tmp;
        current_algo_index = new_algo;
    }

    auto *play_head = getPlayHead();

    if (play_head) {
        auto position = play_head->getPosition();
        if (position) {
            is_playing = position->getIsPlaying();

            auto hostBpm = position->getBpm();
            if (hostBpm) {
                bpm = *hostBpm;
            }
            auto time_sig = position->getTimeSignature();
            if (time_sig) {
                qpb = 4.0 / time_sig->denominator;
            }

            auto opt_pos_qn = position->getPpqPosition();
            jassert(opt_pos_qn);
            if (opt_pos_qn) {
                // position in slots - will be more useful later.
                slots = *opt_pos_qn / getSpeedFactor();
                // how far along in the current slot are we ?
                slot_fraction = (slots - std::floor(slots));
                if (slot_fraction < 0.00001) {
                    slot_fraction = 0.0;
                } else if (slot_fraction > 0.99999 ) {
                    slot_fraction = 0.0;
                }
            }
        }
    }

    int samples_per_qn =  static_cast<int>(std::ceil((rate * 60.0) / (bpm * qpb))) ;

    double slots_in_buffer = (double(numSamples) / double(samples_per_qn)) / getSpeedFactor();
    



#if STARP_DEBUG
    if (is_playing) {
        std::stringstream x;
        x << "START Slots = " << slots << "; slot_fraction = " << std::setprecision(10) << slot_fraction 
            << "; gate = " << getGate() << "; slots_in_buffer = " << slots_in_buffer;
        x << "; numSamples = " << numSamples << "; samples_per_qn = " << samples_per_qn;
        dbgout->logMessage(x.str());
     }
#endif

    // get slot duration

    auto slotDuration = static_cast<int>(std::ceil(double(samples_per_qn) * getSpeedFactor()));

    bool notes_changed = false;

    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        if      (msg.isNoteOn())  { notes.add(msg.getNoteNumber()); notes_changed = true; }
        else if (msg.isNoteOff()) { notes.removeValue(msg.getNoteNumber()); notes_changed = true; }
    }

    midiMessages.clear();

     auto *new_notes = new juce::Array<played_note>();


    for (int idx = 0; idx < active_notes->size(); ++idx) {
        auto thisNote = active_notes->getUnchecked(idx);
        auto note_value = thisNote.note_value;
        auto end_slot = thisNote.end_slot;

        if ( (slots + slots_in_buffer) > end_slot ) {
            // This is the number of samples into the buffer where the note should turn off
            int offset = int((end_slot - slots) * slotDuration);
#if STARP_DEBUG
            {
                std::stringstream x;
                x << "stop " << note_value << " @ " << offset;
                dbgout->logMessage(x.str());
            }
#endif
            midiMessages.addEvent (juce::MidiMessage::noteOff (1, note_value), offset);

        } else {
            // we didn't stop it, so copy to the new list
            new_notes->add(thisNote);
        }
    }

    delete active_notes;
    active_notes = new_notes;

    if ( (slot_fraction == 0.0) || (slot_fraction + slots_in_buffer) > 1.0 ) {

        // start a new note
        // This is the number of samples into the buffer where the note should turn on
        int offset = 0;
        if (slot_fraction > 0.0) {
            offset = int((1.0 - slot_fraction) * double(slotDuration));
        } 

        if (offset < (numSamples-2) ) {
            if (notes.size() > 0) {
                int new_note = algo->getNextNote(slots, notes, notes_changed);
                if (new_note >= 0 ) {
                    int note_prob = *probability;
                    HashRandom prob_rng{"Probability", randomKey, slots};
                    if (prob_rng.nextInt(0, 101) <= note_prob ) {
                        int range = *velo_range;
                        int note_velocity = *velocity;
                        if (range > 0)  {
                            HashRandom vel_rng{"Velocity", randomKey, slots};

                            int max = juce::jmin(128, note_velocity + range); 
                            int min = juce::jmax(1, note_velocity - range);

                            note_velocity = vel_rng.nextInt(min, max);
                        }

                        midiMessages.addEvent (
                            juce::MidiMessage::noteOn(1, new_note, (std::uint8_t) note_velocity), 
                            offset
                        );
                        double end_slot = slots + getGate();
                        active_notes->add({new_note, end_slot});
#if STARP_DEBUG
                        {
                            std::stringstream x;
                            x << "start " << new_note << " @ " << offset << "; slot = " << end_slot;
                            dbgout->logMessage(x.str());
                        }
#endif
                    }
#if STARP_DEBUG
                } else {
                    dbgout->logMessage("algo could not find note to start");
#endif
                }
            } else {
                algo->reset();
            }
        }

    }

#if STARP_DEBUG
    if (is_playing) {
        std::stringstream x;
        x << "END " << "active count " << active_notes->size();
        dbgout->logMessage(x.str());
    }
#endif


}


//============================================================================
bool StarpProcessor::hasEditor() const
{
    // (change this to false if you choose to not supply an editor)
    return true;
}

juce::AudioProcessorEditor* StarpProcessor::createEditor() {
    //return new AudioPluginAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this); 
}

//============================================================================

double StarpProcessor:: getSpeedFactor() {
    return speed_parameter_values[*speed].multiplier;
}

double StarpProcessor::getGate() {
    return *gate / 100.0;
}

//============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StarpProcessor();
}
