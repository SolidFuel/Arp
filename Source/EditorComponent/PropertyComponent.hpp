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

#include "OverlayComponent.hpp"
#include "AlgoChoiceComponent.hpp"
#include "RandomAlgoOptionsComponent.hpp"
#include "LinearAlgoOptionsComponent.hpp"
#include "../ProcessorParameters.hpp"

#include <solidfuel/solidfuel.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

using  SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using namespace solidfuel;

class SpeedTypeChoiceComponent : public juce::ChoicePropertyComponent {
private :
    juce::Value value_ptr_;
    ValueListener listener_;

public : 
    SpeedTypeChoiceComponent() :
        juce::ChoicePropertyComponent("")
    {
            
        choices = SpeedTypes;
        listener_.onChange = [this](juce::Value &){ refresh(); };
        refresh();
    }

    void setValue(juce::Value &ptr) {
        value_ptr_.referTo(ptr);
        value_ptr_.addListener(&listener_);
    }

    virtual void setIndex(int newIndex)	override {
        value_ptr_.setValue(newIndex);
    }
    virtual int getIndex()	const override {
        return int(value_ptr_.getValue());
    }


    /*
     * Don't paint the label
     */

    void paint (juce::Graphics& g) override {
        auto& lf = getLookAndFeel();

        lf.drawPropertyComponentBackground (g, getWidth(), getHeight(), *this);
    }

    void resized() override {
        if (auto c = getChildComponent (0)) {

            auto bounds = getLocalBounds();
            bounds.reduce(8, 0);
            c->setBounds(bounds);
        }
    }


};


//==============================================================
class PropertyComponent : public juce::Component {


public:

    PropertyComponent(ProcessorParameters *params);

    void paint(juce::Graphics&) override;
    void resized() override;

private:

    using BCO = BoxComponent::Orientation;

    ProcessorParameters *params_ = nullptr;

    juce::Value speed_type_value_{SpeedType::Note};


    BoxComponent speedBox_{BCO::Horizontal, true};

    SpeedTypeChoiceComponent speedType_;

    OverlayComponent<juce::Slider> speedComponent_;

    juce::Slider speedNoteSlider_;
    juce::Slider speedBarSlider_;
    juce::Slider speedMSecSlider_;
    std::unique_ptr<SliderAttachment> speedNoteAttachment_;
    std::unique_ptr<SliderAttachment> speedBarAttachment_;
    std::unique_ptr<SliderAttachment> speedMSecAttachment_;

    ValueListener speed_type_listener_;
    juce::Slider *current_speed_slider_;

    juce::Label probabilityLabel_;
    juce::Slider probabilitySlider_;
    std::unique_ptr<SliderAttachment> probabilityAttachment_;

    BoxComponent gateBox_{BCO::Horizontal};
    juce::Label gateLabel_;
    juce::Slider gateSlider_;
    std::unique_ptr<SliderAttachment> gateAttachment_;

    BoxComponent gateRangeBox_{BCO::Horizontal};
    juce::Label gateRangeLabel_;
    juce::Slider gateRangeSlider_;
    std::unique_ptr<SliderAttachment> gateRangeAttachment_;

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

    BoxComponent gateGroup_{BCO::Vertical, true};
    BoxComponent veloGroup_{BCO::Vertical, true};
    BoxComponent timingGroup_{BCO::Vertical, true};

    void update_speed_type(SpeedType st);

//==========================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PropertyComponent)
};