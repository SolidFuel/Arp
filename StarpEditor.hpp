#pragma once

#include "StarpProcessor.hpp"
#include "ParamData.hpp"

//==============================================================================
class StarpEditor  : public juce::AudioProcessorEditor
{
public:
    explicit StarpEditor (StarpProcessor&);
    ~StarpEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    StarpProcessor& processorRef;

    juce::Value algo_value_;
    juce::Value speed_value_;
    juce::Value key_value_;
    juce::Value gate_value_;

private:


    juce::ChoicePropertyComponent algorithmChoice_{ 
        algo_value_, 
        "Algorithm:", 
        AlgorithmChoices, 
        AlgorithmIndexes
    };

    juce::Label keyLabel_{"RandomKeyLabel", "00000000"};

    juce::ChoicePropertyComponent speedChoice_{ 
        speed_value_, 
        "Speed:", 
        SpeedChoices, 
        SpeedIndexes
    };

    juce::Slider gateSlider_{"Gate:"};


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StarpEditor)
};
