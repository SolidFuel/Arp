#include "StarpProcessor.h"
#include "StarpEditor.h"

#include <iomanip>

speed_value speed_parameter_values[] = {
    speed_value{"1/16", 0.25},
    speed_value{"1/8" , 0.50},
    speed_value{"1/4" , 1.0 },
    speed_value{"1/2" , 2.0 },
};

int default_speed = 2;

bool operator==(const played_note& lhs, const played_note& rhs) {
    return lhs.note_value == rhs.note_value;
}

bool operator<(const played_note& lhs, const played_note& rhs) {
    return lhs.note_value < rhs.note_value;
}



//==============================================================================
    
StarpProcessor::StarpProcessor()
     : AudioProcessor (BusesProperties()) // No audio busses
{
    //addParameter (speed = new juce::AudioParameterFloat ({ "speed", 1 }, "Arpeggiator Speed", 0.0, 1.0, 0.5));
    juce::StringArray choices;
    for (auto const &v : speed_parameter_values) {
        choices.add(v.name);
    }
    speed = new juce::AudioParameterChoice({"speed", 1}, "Speed", choices, default_speed);
    addParameter(speed);

    addParameter (gate = new juce::AudioParameterFloat ({ "gate", 2 },  "Gate %", 10.0, 200.0, 100.0));
    addParameter(algorithm_parm = new juce::AudioParameterChoice({"algorithm", 1}, "Aglo", {"up", "down"}, 0));

    if (algo == nullptr) {
        algo = (AlgorithmBase *)new UpAlgorithm();
        current_algo_index = 0;
    }

    dbgout = juce::FileLogger::createDateStampedLogger("Starp", "StarpLogFile", ".txt", "----V40---");


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

//==============================================================================
void StarpProcessor::getStateInformation (juce::MemoryBlock& destData) {

    dbgout->logMessage("GET STATE called");

    std::unique_ptr<juce::XmlElement> xml (new juce::XmlElement ("StarpStateInfo"));
    xml->setAttribute ("version", (int) 1);
    xml->setAttribute ("speed", (int) *speed);
    xml->setAttribute ("algorithm", (int) *algorithm_parm);
    xml->setAttribute ("gate", *gate); 
    copyXmlToBinary (*xml, destData);

}

void StarpProcessor::setStateInformation (const void* data, int sizeInBytes) {

    dbgout->logMessage("SET STATE called");

    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName ("StarpStateInfo"))
        {
            *speed          = xmlState->getIntAttribute("speed", 2);
            *algorithm_parm = xmlState->getIntAttribute ("algorithm", 0);
            *gate           = (float) xmlState->getDoubleAttribute ("gate", 100.0);
        }
    }
}



//==============================================================================
const juce::String StarpProcessor::getName() const
{
    return JucePlugin_Name;
}

int StarpProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int StarpProcessor::getCurrentProgram()
{
    return 0;
}

void StarpProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String StarpProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void StarpProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void StarpProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
        juce::ignoreUnused (samplesPerBlock);

        notes.clear();
        rate = sampleRate;
        slotCount = 0;

        if (active_notes == nullptr) {
            active_notes = new juce::Array<played_note>();
        }

}

void StarpProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

    if (active_notes != nullptr) {
        delete active_notes;
        active_notes = nullptr;
    }

}

bool StarpProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const {
    juce::ignoreUnused (layouts);
    return true;
}

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
        AlgorithmBase *tmp = (algo == nullptr) ? new UpAlgorithm() : algo ;
        switch (new_algo) {
            case 0 :
                algo = new UpAlgorithm(tmp);
                break;
            case 1 :
                algo = new DownAlgorithm(tmp);
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
    
    if (is_playing) {
        std::stringstream x;
        x << "START Slots = " << slots << "; slot_fraction = " << std::setprecision(10) << slot_fraction 
            << "; gate = " << getGate() << "; slots_in_buffer = " << slots_in_buffer;
        x << "; numSamples = " << numSamples << "; samples_per_qn = " << samples_per_qn;
        dbgout->logMessage(x.str());
    }

    // get slot duration

    auto slotDuration = static_cast<int>(std::ceil(double(samples_per_qn) * getSpeedFactor()));
    //auto noteDuration = static_cast<int>(std::ceil(double(samples_per_qn) * getSpeedFactor() * getGate()));

    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        if      (msg.isNoteOn())  notes.add (msg.getNoteNumber());
        else if (msg.isNoteOff()) notes.removeValue (msg.getNoteNumber());
    }

    midiMessages.clear();

     auto *new_notes = new juce::Array<played_note>();


    for (int idx = 0; idx < active_notes->size(); ++idx) {
        auto thisNote = active_notes->getUnchecked(idx);
        auto note_value = thisNote.note_value;
        auto start_slot = thisNote.start_slot;

        if ( (slots - start_slot + slots_in_buffer) > getGate() ) {
            // This is the number of samples into the buffer where the note should turn off
            int offset = int((getGate() - (slots - start_slot)) * double(slotDuration));
            {
                std::stringstream x;
                x << "stop " << note_value << " @ " << offset;
                dbgout->logMessage(x.str());
            }
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

        if (offset < (numSamples-2) && notes.size() > 0) {
            int new_note_value = algo->getNextValue(notes);
            int new_note = notes[new_note_value];
            midiMessages.addEvent (juce::MidiMessage::noteOn  (1, new_note, (std::uint8_t) 127), offset);
            double start_slot;
            if (offset == 0) {
                start_slot = slots;
            } else {
                start_slot = slots - slot_fraction + 1.0;
            }
            active_notes->add({new_note, start_slot});
            {
                std::stringstream x;
                x << "start " << new_note << " @ " << offset << "; slot = " << start_slot;
                dbgout->logMessage(x.str());
            }
        }

    }

    if (is_playing) {
        std::stringstream x;
        x << "END " << "active count " << active_notes->size();
        dbgout->logMessage(x.str());
    }



}


//==============================================================================
bool StarpProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* StarpProcessor::createEditor() {
    //return new AudioPluginAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this); 
}

//==============================================================================

double StarpProcessor:: getSpeedFactor() {
    return speed_parameter_values[*speed].multiplier;
}

double StarpProcessor::getGate() {
    return *gate / 100.0;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StarpProcessor();
}
