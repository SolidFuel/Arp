#include "StarpProcessor.h"
#include "StarpEditor.h"

speed_value speed_parameter_values[] = {
    speed_value{"1/16", 0.25},
    speed_value{"1/8" , 0.50},
    speed_value{"1/4" , 1.0 },
    speed_value{"1/2" , 2.0 },
};

int default_speed = 2;


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
}

StarpProcessor::~StarpProcessor()
{
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
        currentNote = -1;
        lastNoteValue = -1;
        time = 0;
        rate = static_cast<double> (sampleRate);
}

void StarpProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
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

    auto *play_head = getPlayHead();

    if (play_head) {
        auto position = play_head->getPosition();
        if (position) {
            auto hostBpm = position->getBpm();
            if (hostBpm) {
                bpm = *hostBpm;
            }
            auto time_sig = position->getTimeSignature();
            if (time_sig) {
                qpb = 4.0 / time_sig->denominator;
            }
        }
    }
    int samples_per_qn =  static_cast<int>(std::ceil((rate * 60.0) / (bpm * qpb))) ;
    

    // get note duration
    auto noteDuration = static_cast<int>(std::ceil(double(samples_per_qn) * getSpeedFactor()));

    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        if      (msg.isNoteOn())  notes.add (msg.getNoteNumber());
        else if (msg.isNoteOff()) notes.removeValue (msg.getNoteNumber());
    }

    midiMessages.clear();

    auto offset = 0;
    int time_passed = numSamples;

    if ((time + numSamples) >= noteDuration) {
        offset = juce::jmax (0, juce::jmin ((int) (noteDuration - time), numSamples - 1));

        if (lastNoteValue > 0) {
            midiMessages.addEvent (juce::MidiMessage::noteOff (1, lastNoteValue), offset);
            lastNoteValue = -1;
        }
    }

    if (lastNoteValue < 0 ) {

        if (notes.size() > 0) {
            currentNote = (currentNote + 1) % notes.size();
            lastNoteValue = notes[currentNote];
            midiMessages.addEvent (juce::MidiMessage::noteOn  (1, lastNoteValue, (std::uint8_t) 127), offset);
            time = numSamples - offset;
            time_passed = 0;
        }
    }

    time = (time + time_passed) % noteDuration;
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
void StarpProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void StarpProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}


double StarpProcessor:: getSpeedFactor() {
    return speed_parameter_values[*speed].multiplier;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StarpProcessor();
}
