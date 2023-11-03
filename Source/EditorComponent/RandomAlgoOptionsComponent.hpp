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

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>


using  SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

class RandomAlgoOptionsComponent : public juce::Component, juce::Value::Listener {
public :
    RandomAlgoOptionsComponent(RandomParameters * parms);

    void valueChanged(juce::Value &v);
    void paint(juce::Graphics&) override;
    void resized() override;

    void addListener(juce::Value::Listener *l) { value_.addListener(l); }


private :

    RandomParameters *params_;
    juce::Value value_;

    juce::Value seed_text_;

    juce::TextButton changeKeyButton_;

    juce::Label keyLabel_;
    juce::Label keyValueLabel_{"RandomKeyLabel", "00000000"};

    void changeKey();
    void update_seed_display();



};