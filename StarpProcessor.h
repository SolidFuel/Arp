#pragma once

#include "Algorithm.h"

#include <juce_audio_processors/juce_audio_processors.h>

#include <fstream>

struct speed_value {
    juce::String name;
    double multiplier;
};

struct played_note {
    int note_value;
    double start_slot;

};

bool operator==(const played_note& lhs, const played_note& rhs);
bool operator<(const played_note& lhs, const played_note& rhs);


//==============================================================================
class StarpProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    StarpProcessor();
    ~StarpProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override { return true; };
    bool producesMidi() const override { return true; };
    bool isMidiEffect() const override { return true; };
    double getTailLengthSeconds() const override { return 0.0; };

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    juce::AudioParameterChoice* speed;
    juce::AudioParameterChoice* algorithm_parm;
    juce::AudioParameterFloat* gate;
    
    int current_algo_index = -1;
    double rate;
    int currentNote;

    juce::SortedSet<int> notes;
    int slotCount;

    juce::Array<played_note> *active_notes = nullptr;

    juce::FileLogger *dbgout = nullptr;

    AlgorithmBase *algo = nullptr;

    double getSpeedFactor();
    double getGate();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StarpProcessor)
};
