/****
 * solidArp - Stable Random Arpeggiator Plugin 
 * Copyright (C) 2023 Solid Fuel
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the 
 * Free Software Foundation, either version 3 of the License, or (at your 
 * option) any later version. This program is distributed in the hope that it 
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the LICENSE file
 * in the root directory.
 ****/

#pragma once

#include "../AlgorithmParameters.hpp"

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include <solidfuel/solidfuel.hpp>

using namespace solidfuel;


class LinearAlgoOptionsComponent : public juce::Component {
public :
    LinearAlgoOptionsComponent(LinearParameters * parms);

    void paint(juce::Graphics&) override;
    void resized() override;

    ~LinearAlgoOptionsComponent() override;


private :

    LinearParameters *params_;

    // These two are a radio group
    juce::TextButton up_button_;
    juce::TextButton down_button_;

    juce::ToggleButton zigzag_button_;
    juce::ToggleButton restart_button_;

    ValueListener direction_listener_;
    ValueListener zigzag_listener_;
    ValueListener restart_listener_;

    void update_direction();
    void update_zigzag();
    void update_restart();


//==========================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LinearAlgoOptionsComponent)

};