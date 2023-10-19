#pragma once

#include "Algorithm.hpp"
#include "ParamData.hpp"

#include <shared_plugin_helpers/shared_plugin_helpers.h>

#include <juce_audio_processors/juce_audio_processors.h>

#include <fstream>

struct speed_value {
    juce::String name;
    double multiplier;
};

struct played_note {
    int note_value;
    double end_slot;

};

bool operator==(const played_note& lhs, const played_note& rhs);
bool operator<(const played_note& lhs, const played_note& rhs);

//============================================================================
struct position_data {
    double position_as_slots = -1.0;
    double slot_fraction = 0.0;
    double slot_number = -1.0;
    int samples_per_qn;
    bool is_playing = false;
};

//============================================================================
struct schedule {
    double slot_number;
    double start;
};

bool operator==(const schedule& lhs, const schedule& rhs);
bool operator<(const schedule& lhs, const schedule& rhs);

//==============================================================================
class StarpProcessor  : public PluginHelpers::ProcessorBase
{
public:
    //==============================================================================
    StarpProcessor();
    ~StarpProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout&) const override { return true; }

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override {return JucePlugin_Name;}

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void set_algo_index(int i) { algo_index = i; }

    //==============================================================================
    // Parameters
    juce::int64 random_key_ = 0L;

    int algo_index = Algorithm::Random;


private:

    struct Parameters
    {        
        juce::AudioParameterChoice* speed;
        juce::AudioParameterFloat*  gate;
        juce::AudioParameterInt*    velocity;
        juce::AudioParameterInt*    velo_range;
        juce::AudioParameterInt*    probability;
        juce::AudioParameterFloat*  timing_delay;
        juce::AudioParameterFloat*  timing_advance;

        std::unique_ptr<juce::AudioProcessorValueTreeState> apvts;

        Parameters(StarpProcessor& processor);

    };

    Parameters parameters;

    //==============================================================================


    double rate_;

    juce::SortedSet<int> notes_;

    juce::Array<played_note> active_notes_;

    juce::Array<schedule> scheduled_notes_;


    std::unique_ptr<AlgorithmBase> algo_obj_;

    double next_scheduled_slot_number = -1.0;

    bool last_play_state_ = false;
    bool last_bypassed_state_ = false;

    double getSpeedFactor();
    double getGate();

    long long last_block_call_ = -1;

    int current_algo_index_ = -1;
    void reassign_algorithm(int new_algo);
    const position_data compute_block_position();
    std::optional<juce::MidiMessage>maybe_play_note(bool notes_changed, double for_slot, double start_pos);

    void schedule_note(double current_pos, double slot_number);

    void reset_data();

public:
    Parameters* getParameters() { return &parameters; }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StarpProcessor)
};
