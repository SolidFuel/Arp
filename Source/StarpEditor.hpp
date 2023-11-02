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
#include "EditorComponent/AlgorithmComponent.hpp"
#include "EditorComponent/MainComponent.hpp"

//==============================================================================
class StarpEditor  : public juce::AudioProcessorEditor
{
public:
    explicit StarpEditor (StarpProcessor&);
    ~StarpEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;


private:

    StarpProcessor& proc_;

    HeaderComponent header_component;
    AlgorithmComponent algorithm_component;
    MainComponent main_component;


//==========================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StarpEditor)
};
