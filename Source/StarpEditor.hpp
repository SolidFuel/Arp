#pragma once

#include "StarpProcessor.hpp"
#include "ParamData.hpp"

using  SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;


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
    StarpProcessor& proc_;

    juce::Value algo_value_;
    juce::Value key_value_;

private:


    juce::ChoicePropertyComponent algorithmChoice_{ 
        algo_value_, 
        "Algorithm:", 
        AlgorithmChoices, 
        AlgorithmIndexes
    };

    juce::Label keyLabel_{"RandomKeyLabel", "00000000"};

    juce::Label speedLabel_;
    juce::Slider speedSlider_;
    std::unique_ptr<SliderAttachment> speedAttachment_;


    juce::Label gateLabel_;
    juce::Slider gateSlider_;
    std::unique_ptr<SliderAttachment> gateAttachment_;

    juce::Label veloLabel_;
    juce::Slider veloSlider_;
    std::unique_ptr<SliderAttachment> veloAttachment_;

    juce::Label probabilityLabel_;
    juce::Slider probabilitySlider_;
    std::unique_ptr<SliderAttachment> probabilityAttachment_;

    juce::Label veloRangeLabel_;
    juce::Slider veloRangeSlider_;
    std::unique_ptr<SliderAttachment> veloRangeAttachment_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StarpEditor)
};
