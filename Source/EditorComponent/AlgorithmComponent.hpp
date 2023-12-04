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


#include "AlgoChoiceComponent.hpp"
#include "RandomAlgoOptionsComponent.hpp"
#include "LinearAlgoOptionsComponent.hpp"
#include "SpiralAlgoOptionsComponent.hpp"

#include "../ProcessorParameters.hpp"
#include "../Starp.hpp"

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class AlgorithmComponent : public juce::Component, juce::Value::Listener {


public:

    AlgorithmComponent(ProcessorParameters *params);

    void valueChanged(juce::Value &v) override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:

    ProcessorParameters *params_ = nullptr;

    AlgoChoiceComponent algoComponent_;
    RandomAlgoOptionsComponent randomComponent_;
    LinearAlgoOptionsComponent linearComponent_;
    SpiralAlgoOptionsComponent spiralComponent_;

//==========================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AlgorithmComponent)


};