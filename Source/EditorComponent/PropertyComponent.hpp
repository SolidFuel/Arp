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

#include "AlgoChoiceComponent.hpp"
#include "RandomAlgoOptionsComponent.hpp"
#include "LinearAlgoOptionsComponent.hpp"
#include "BoxComponent.hpp"
#include "../ProcessorParameters.hpp"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

using  SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

class PropertyComponent : public juce::Component {


public:

    PropertyComponent(ProcessorParameters *params);

    void paint(juce::Graphics&) override;
    void resized() override;

private:

    using BCO = BoxComponent::Orientation;

    ProcessorParameters *params_ = nullptr;

    juce::Label speedLabel_;
    juce::Slider speedSlider_;
    std::unique_ptr<SliderAttachment> speedAttachment_;


    juce::Label gateLabel_;
    juce::Slider gateSlider_;
    std::unique_ptr<SliderAttachment> gateAttachment_;

    juce::Label probabilityLabel_;
    juce::Slider probabilitySlider_;
    std::unique_ptr<SliderAttachment> probabilityAttachment_;

    BoxComponent veloBox_{BCO::Horizontal};
    juce::Label veloLabel_;
    juce::Slider veloSlider_;
    std::unique_ptr<SliderAttachment> veloAttachment_;

    BoxComponent veloRangeBox_{BCO::Horizontal};
    juce::Label veloRangeLabel_;
    juce::Slider veloRangeSlider_;
    std::unique_ptr<SliderAttachment> veloRangeAttachment_;

    BoxComponent advanceBox_{BCO::Horizontal};
    juce::Label advanceLabel_;
    juce::Slider advanceSlider_;
    std::unique_ptr<SliderAttachment> advanceAttachment_;

    BoxComponent delayBox_{BCO::Horizontal};
    juce::Label delayLabel_;
    juce::Slider delaySlider_;
    std::unique_ptr<SliderAttachment> delayAttachment_;

    BoxComponent veloGroup_{BCO::Vertical, true};
    BoxComponent timingGroup_{BCO::Vertical, true};

//==========================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PropertyComponent)
};