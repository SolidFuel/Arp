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


#include "SpiralAlgoOptionsComponent.hpp"

constexpr int POSITION_GID = 1002;
constexpr int DIRECTION_GID = 1003;

using SD = SpiralParameters::Direction;

SpiralAlgoOptionsComponent::SpiralAlgoOptionsComponent(SpiralParameters * parms) : params_(parms) {

    top_button_.setButtonText("Top");
    top_button_.setClickingTogglesState(true);
    top_button_.setRadioGroupId(POSITION_GID);
    top_button_.setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnRight);
    addAndMakeVisible(top_button_);
    top_button_.onClick = [this]() { update_position(); };


    bottom_button_.setButtonText("Bottom");
    bottom_button_.setClickingTogglesState(true);
    bottom_button_.setRadioGroupId(POSITION_GID);
    bottom_button_.setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnLeft);
    addAndMakeVisible(bottom_button_);

    direction_group_.setGroupId(DIRECTION_GID);

    in_button_.setButtonText("In");
    in_button_.onClick = [this]() { update_direction(SD::In); };
    direction_group_.add(in_button_);

    out_button_.setButtonText("Out");
    out_button_.onClick = [this]() { update_direction(SD::Out); };
    direction_group_.add(out_button_);

    inout_button_.setButtonText("InOut");
    inout_button_.onClick = [this]() { update_direction(SD::InOut); };
    direction_group_.add(inout_button_);

    outin_button_.setButtonText("OutIn");
    outin_button_.onClick = [this]() { update_direction(SD::OutIn); };
    direction_group_.add(outin_button_);

    addAndMakeVisible(direction_group_);


    position_listener_.onChange = [this](juce::Value &v) { 
        if (int(v.getValue()) == SpiralParameters::StartPosition::Top) {
            top_button_.setToggleState(true, juce::sendNotification);
        } else {
            bottom_button_.setToggleState(true, juce::sendNotification);
        }
    };
    // force the GUI to match the model
    position_listener_.onChange(params_->start_position);

    params_->start_position.addListener(&position_listener_);

    direction_listener_.onChange = [this](juce::Value &v) {
        int new_dir = int(v.getValue());
        switch (new_dir) {
            case SD::In :
                in_button_.setToggleState(true, juce::sendNotification);
                break;
            case SD::Out :
                out_button_.setToggleState(true, juce::sendNotification);
                break;
            case SD::InOut :
                inout_button_.setToggleState(true, juce::sendNotification);
                break;
            case SD::OutIn :
                outin_button_.setToggleState(true, juce::sendNotification);
                break;
            default :
                jassertfalse;
        }
    };

    // force the GUI to match the model
    direction_listener_.onChange(params_->direction);

    params_->direction.addListener(&direction_listener_);



}

SpiralAlgoOptionsComponent::~SpiralAlgoOptionsComponent() {
    params_->start_position.removeListener(&position_listener_);
    params_->direction.removeListener(&direction_listener_);
}

void SpiralAlgoOptionsComponent::update_position() {
    auto state = top_button_.getToggleState() ? 
                SpiralParameters::StartPosition::Top : 
                SpiralParameters::StartPosition::Bottom ;


    params_->start_position = state;
}

void SpiralAlgoOptionsComponent::update_direction(SpiralParameters::Direction direction) { 
    params_->direction = direction;
}



//==============================================================
void SpiralAlgoOptionsComponent::paint(juce::Graphics&g) {
    g.fillAll (getLookAndFeel().findColour (juce::PropertyComponent::backgroundColourId));

}

//==========================================================
void SpiralAlgoOptionsComponent::resized() {

    DBGLOG("SpiralAlgoOptionsComponent::resized called")

    using Grid = juce::Grid;

    Grid grid;
 
    using Track = Grid::TrackInfo;
    using Fr = Grid::Fr;
    using GridItem = juce::GridItem;

    grid.alignItems = juce::Grid::AlignItems::center;
    grid.justifyContent = juce::Grid::JustifyContent::start;
    grid.justifyItems = Grid::JustifyItems::start;
    grid.templateColumns = { Track (Fr (1)), Track(Fr(1)), Track (Fr (3)), Track (Fr (1)) };

    grid.templateRows.add(Track (Fr (1)));
    auto dir_button_height = float(getHeight() * 0.75f);

    // use the prefered width of the "Bottom" button since it should be larger.
    float dir_button_width = float(bottom_button_.getBestWidthForHeight(int(dir_button_height)));    

    grid.items.add(GridItem(top_button_).withHeight(dir_button_height)
        .withWidth(dir_button_width)
        .withJustifySelf(GridItem::JustifySelf::end) );

    grid.items.add(GridItem(bottom_button_).withHeight(dir_button_height).withWidth(dir_button_width));

    grid.items.add(GridItem(direction_group_).withHeight(dir_button_height));

    grid.performLayout (getLocalBounds());

}
