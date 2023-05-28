#include "StarpProcessor.h"
#include "StarpEditor.h"

//==============================================================================
    
StarpProcessor::StarpProcessor()
     : AudioProcessor (BusesProperties()) // No audio busses
{
    addParameter (speed = new juce::AudioParameterFloat ({ "speed", 1 }, "Arpeggiator Speed", 0.0, 1.0, 0.5));
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
        currentNote = 0;
        lastNoteValue = -1;
        time = 0;
        rate = static_cast<float> (sampleRate);
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

    // get note duration
    auto noteDuration = static_cast<int> (std::ceil (rate * 0.25f * (0.1f + (1.0f - (*speed)))));

    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        if      (msg.isNoteOn())  notes.add (msg.getNoteNumber());
        else if (msg.isNoteOff()) notes.removeValue (msg.getNoteNumber());
    }

    midiMessages.clear();

    if ((time + numSamples) >= noteDuration)
    {
        auto offset = juce::jmax (0, juce::jmin ((int) (noteDuration - time), numSamples - 1));

        if (lastNoteValue > 0) {
            midiMessages.addEvent (juce::MidiMessage::noteOff (1, lastNoteValue), offset);
            lastNoteValue = -1;
        }

        if (notes.size() > 0) {
            currentNote = (currentNote + 1) % notes.size();
            lastNoteValue = notes[currentNote];
            midiMessages.addEvent (juce::MidiMessage::noteOn  (1, lastNoteValue, (std::uint8_t) 127), offset);
        }

    }

    time = (time + numSamples) % noteDuration;
}


//==============================================================================
bool StarpProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* StarpProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
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

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StarpProcessor();
}
