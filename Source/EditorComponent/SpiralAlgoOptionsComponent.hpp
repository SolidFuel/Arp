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

#include "../AlgorithmParameters.hpp"
#include "../ValueListener.hpp"
#include "ButtonGroupComponent.hpp"

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>


class SpiralAlgoOptionsComponent : public juce::Component {
public :
    SpiralAlgoOptionsComponent(SpiralParameters * parms);

    void paint(juce::Graphics&) override;
    void resized() override;

    ~SpiralAlgoOptionsComponent();


private :

    SpiralParameters *params_;

    // These two are a radio group
    juce::TextButton top_button_;
    juce::TextButton bottom_button_;

    // These 4 are a radio group

    juce::TextButton in_button_;
    juce::TextButton out_button_;
    juce::TextButton inout_button_;
    juce::TextButton outin_button_;

    ButtonGroupComponent direction_group_;

    ValueListener direction_listener_;
    ValueListener position_listener_;

    void update_direction(SpiralParameters::Direction direction);
    void update_position();


//==========================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpiralAlgoOptionsComponent)


};