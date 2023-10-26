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

#pragma once

#include "StarpProcessor.hpp"
#include "ParamData.hpp"
#include "EditorComponent/HeaderComponent.hpp"
#include "EditorComponent/MainComponent.hpp"

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

    HeaderComponent header_component;
    MainComponent main_component;



private:


    juce::ChoicePropertyComponent algorithmChoice_{ 
        algo_value_, 
        "Algorithm:", 
        AlgorithmChoices, 
        AlgorithmIndexes
    };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StarpEditor)
};
