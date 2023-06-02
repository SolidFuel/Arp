#include "StarpProcessor.hpp"
#include "StarpEditor.hpp"
#include "Starp.hpp"

#include <iomanip>
#include <cmath>

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


bool operator==(const schedule& lhs, const schedule& rhs){ return lhs.start == rhs.start; }
bool operator<(const schedule& lhs, const schedule& rhs) { return lhs.start < rhs.start; }


//============================================================================
    
StarpProcessor::StarpProcessor() : AudioProcessor (BusesProperties()) {

    // Pick a random key
    juce::Random rng{};
    random_key_ = rng.nextInt64();

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
    dbgout = juce::FileLogger::createDateStampedLogger("Starp", "StarpLogFile", ".txt", "----V500---");
#endif

}

StarpProcessor::~StarpProcessor() {

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

    hash << std::hex << random_key_;

    std::unique_ptr<juce::XmlElement> xml (new juce::XmlElement ("StarpStateInfo"));
    xml->setAttribute ("version", (int) 1);
    xml->setAttribute ("speed", (int) *speed);
    xml->setAttribute ("algorithm", (int) *algorithm_parm);
    xml->setAttribute ("gate", *gate); 
    xml->setAttribute("key", juce::String{random_key_});
    xml->setAttribute("velocity", *velocity);
    xml->setAttribute("velocity_range", *velo_range);
    xml->setAttribute("probability", *probability);
    xml->setAttribute("delay", *timing_delay);
    xml->setAttribute("advance", *timing_advance);

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
            *timing_delay   = (float) xmlState->getDoubleAttribute ("delay", 0.0);
            *timing_advance = (float) xmlState->getDoubleAttribute ("advance", 0.0);

            if (xmlState->hasAttribute("key")) {
                random_key_ = xmlState->getStringAttribute("key", "-42").getLargeIntValue();
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
    rate_ = sampleRate;

    notes_.clearQuick();
    active_notes_.clearQuick();
    

}

void StarpProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

#if STARP_DEBUG
    dbgout->logMessage("RELEASE called");
#endif
    notes_.clear();
    active_notes_.clear();

}

//============================================================================
void StarpProcessor::reassign_algorithm(int new_algo) {
    if (new_algo != current_algo_index) {
        switch (new_algo) {
            case 0 :
                algo_ = std::make_unique<UpAlgorithm>(algo_.get());
                break;
            case 1 :
                algo_ = std::make_unique<DownAlgorithm>(algo_.get());
                break;
            case 2 :
                auto *na = new RandomAlgorithm(algo_.get());
                na->setKey(random_key_);
                na->setDebug(dbgout);
                algo_.reset(na);
                break;
        }
        current_algo_index = new_algo;
    }

}

const position_data &StarpProcessor::compute_block_position() {

    position_data pd{};

    double bpm = 120.0;
    double qpb = 1; // quarter notes per beat

    auto *play_head = getPlayHead();

    if (play_head) {
        auto position = play_head->getPosition();
        if (position) {
            pd.is_playing = position->getIsPlaying();

            auto hostBpm = position->getBpm();
            if (hostBpm) {
                bpm = *hostBpm;
            }
            auto time_sig = position->getTimeSignature();
            if (time_sig) {
                qpb = 4.0 / time_sig->denominator;
            }

            auto opt_pos_qn = position->getPpqPosition();
            if (opt_pos_qn) {
                // position in slots
                pd.position_as_slots = *opt_pos_qn / getSpeedFactor();

                // floor does unexpected things if the number is already integral.
                // so fake it with rouund.
                pd.slot_number = std::round(pd.position_as_slots);

                if ( std::abs(pd.slot_number - pd.position_as_slots) < 0.00001) {
                    pd.position_as_slots = pd.slot_number;
                } else if (pd.slot_number > pd.position_as_slots) {
                    pd.slot_number -= 1.0;
                }
                // how far along in the current slot are we ?
                pd.slot_fraction = pd.position_as_slots - pd.slot_number;
                if (pd.slot_fraction < 0.00001) {
                    pd.slot_fraction = 0.0;
                } else if (pd.slot_fraction > 0.99999 ) {
                    pd.slot_fraction = 0.0;
                }
            }
        }
    }

    pd.samples_per_qn =  static_cast<int>(std::ceil((rate_ * 60.0) / (bpm * qpb))) ;

    return std::move(pd);

}

std::optional<juce::MidiMessage> StarpProcessor::maybe_play_note(int offset, bool notes_changed, double for_slot, double start_pos) {


    if (notes_.size() == 0) {
        algo_->reset();
        return std::nullopt;
    }

    int note_prob = *probability;
    HashRandom prob_rng{"Probability", random_key_, for_slot};
    if (prob_rng.nextInt(0, 101) > note_prob ) {
        return std::nullopt;
    }

    std::optional<juce::MidiMessage> retval;

    int new_note = algo_->getNextNote(for_slot, notes_, notes_changed);
    if (new_note >= 0 ) {
        int range = *velo_range;
        int note_velocity = *velocity;
        if (range > 0)  {
            HashRandom vel_rng{"Velocity", random_key_, for_slot};

            int max = juce::jmin(128, note_velocity + range); 
            int min = juce::jmax(1, note_velocity - range);

            note_velocity = vel_rng.nextInt(min, max);
        }

        retval = juce::MidiMessage::noteOn(
                1, new_note, (std::uint8_t) note_velocity
            );
        
        double end_slot = start_pos + getGate();
        active_notes_.add({new_note, end_slot});
#if STARP_DEBUG
        {
            std::stringstream x;
            x << "--- start " << new_note << " @ " << offset << "; end = " << end_slot;
            dbgout->logMessage(x.str());
        }
#endif

#if STARP_DEBUG
    } else {
        dbgout->logMessage("--- algo could not find note to start");
#endif
    }

    return retval;
}

void StarpProcessor::schedule_note(double current_pos, double slot_number) {
    HashRandom rng{"Humanize", random_key_, slot_number};

    float variance = rng.nextFloat(*timing_advance, *timing_delay);

    double sched_start = slot_number + (variance/100.0);

    if (current_pos <= sched_start) {
        next_scheduled_slot_number = slot_number;
        scheduled_notes_.addUsingDefaultSort({slot_number, sched_start});
#if STARP_DEBUG
        {
            std::stringstream x;
            x << "--- scheduling slot " << slot_number << " @ " << sched_start;
            dbgout->logMessage(x.str());
        }
    } else  {
        std::stringstream x;
        x << "--- tried to scheduling slot " << slot_number << " @ " << sched_start << " but late (" << current_pos << ")";
        dbgout->logMessage(x.str());
#endif
        
    }

}


//============================================================================
void StarpProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiBuffer) {
    // A pure MIDI plugin shouldn't be provided any audio data
    jassert (buffer.getNumChannels() == 0);

    // however we use the buffer to get timing information
    auto numSamples = buffer.getNumSamples();

    reassign_algorithm(*algorithm_parm);

    position_data pd = compute_block_position();

    double slots_in_buffer = (double(numSamples) / double(pd.samples_per_qn)) / getSpeedFactor();
    
    auto slotDuration = static_cast<int>(std::ceil(double(pd.samples_per_qn) * getSpeedFactor()));


    if (pd.is_playing != last_play_state) {
        dbgout->logMessage("--- Play state change");
        next_scheduled_slot_number = -1.0;
        scheduled_notes_.clearQuick();
        last_play_state = pd.is_playing;
    }

#if STARP_DEBUG
    if (pd.is_playing) {
        std::stringstream x;
        x << "START Slots = " << pd.position_as_slots << "; slot_number = " << pd.slot_number << 
             "; slot_fraction = " << std::setprecision(10) << pd.slot_fraction 
            << "; gate = " << getGate() << "; slots_in_buffer = " << slots_in_buffer;
        dbgout->logMessage(x.str());
     }
#endif



    // === Read Midi Messages and update notes_ set
    bool notes_changed = false;

    for (const auto metadata : midiBuffer)
    {
        const auto msg = metadata.getMessage();
        if      (msg.isNoteOn())  { notes_.add(msg.getNoteNumber()); notes_changed = true; }
        else if (msg.isNoteOff()) { notes_.removeValue(msg.getNoteNumber()); notes_changed = true; }
    }

    midiBuffer.clear();

    juce::Array<played_note> new_notes{};

    //------------------------------------------------------------------------
    // Stop any notes that will end during this buffer
    for (int idx = 0; idx < active_notes_.size(); ++idx) {

        auto thisNote = active_notes_.getUnchecked(idx);
        auto note_value = thisNote.note_value;
        auto end_slot = thisNote.end_slot;

        if ( (pd.position_as_slots + slots_in_buffer) > end_slot ) {
            // This is the number of samples into the buffer where the note should turn off
            int offset = int((end_slot - pd.position_as_slots) * slotDuration);
#if STARP_DEBUG
            {
                std::stringstream x;
                x << "stop " << note_value << " @ " << offset;
                dbgout->logMessage(x.str());
            }
#endif
            midiBuffer.addEvent(juce::MidiMessage::noteOff (1, note_value), offset);

        } else {
            // we didn't stop it, so copy to the new list
            new_notes.add(thisNote);
        }
    }

    active_notes_.clearQuick();
    active_notes_.addArray(new_notes);

    //------------------------------------------------------------------------
    // Scehdule note if required.
    if (next_scheduled_slot_number < pd.slot_number) {
        schedule_note(pd.position_as_slots, pd.slot_number);
    }
    if ( (pd.slot_fraction > 0.0) &&  next_scheduled_slot_number <= pd.slot_number) {
        schedule_note(pd.position_as_slots, pd.slot_number+ 1.0);
    }

#if STARP_DEBUG
    if (pd.is_playing && scheduled_notes_.size() > 0) {
        std::stringstream x;
        x << "; next_sched = " << scheduled_notes_[0].slot_number << "; next_start = " << scheduled_notes_[0].start;
        dbgout->logMessage(x.str());
     }
#endif

    while(scheduled_notes_.size() > 0) {

        if ((pd.position_as_slots + slots_in_buffer) > scheduled_notes_[0].start ) {
            // start a new note
            // This is the number of samples into the buffer where the note should turn on
            int offset = juce::jmax(0, int((scheduled_notes_[0].start - pd.position_as_slots) * double(slotDuration)));
            // if the note happens right at the end of the buffer, weird things happen.
            // So, don't play it now, wait for the next buffer.
            if (offset < numSamples - 2) {
                auto msg = maybe_play_note(offset, notes_changed, scheduled_notes_[0].slot_number, scheduled_notes_[0].start );
                if (msg) {
                    midiBuffer.addEvent(*msg, offset);
                }
                scheduled_notes_.remove(0);
            } else {
                break;
            }
        } else {
            break;
        }
    }

#if STARP_DEBUG
    if (pd.is_playing) {
        std::stringstream x;
        x << "END " << "active count " << active_notes_.size();
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
