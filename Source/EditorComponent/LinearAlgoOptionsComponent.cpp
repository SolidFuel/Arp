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


#include "LinearAlgoOptionsComponent.hpp"

constexpr int GROUP_ID = 1001;

LinearAlgoOptionsComponent::LinearAlgoOptionsComponent(LinearParameters * parms) : params_(parms) {

    up_button_.setButtonText("Up");
    up_button_.setClickingTogglesState(true);
    up_button_.setRadioGroupId(GROUP_ID);
    addAndMakeVisible(up_button_);
    up_button_.onClick = [this]() { update_direction(); };


    down_button_.setButtonText("Down");
    down_button_.setClickingTogglesState(true);
    down_button_.setRadioGroupId(GROUP_ID);
    addAndMakeVisible(down_button_);

    zigzag_button_.setButtonText("Zigzag");
    addAndMakeVisible(zigzag_button_);
    zigzag_button_.onClick = [this]() { update_zigzag(); };

    restart_button_.setButtonText("Restart");
    restart_button_.setTooltip("Restart the sequence whenever the midi notes change");
    addAndMakeVisible(restart_button_);
    restart_button_.onClick = [this]() { update_restart(); };

    direction_listener_.onChange = [this](juce::Value &v) { 
        if (int(v.getValue()) == LinearParameters::Direction::Up) {
            up_button_.setToggleState(true, juce::sendNotification);
        } else {
            down_button_.setToggleState(true, juce::sendNotification);
        }
    };
    // force the GUI to match the model
    direction_listener_.onChange(params_->direction);

    params_->direction.addListener(&direction_listener_);

    zigzag_listener_.onChange = [this](juce::Value &) {
        zigzag_button_.setToggleState(params_->get_zigzag(), juce::sendNotification);
    };

    // force the GUI to match the model
    zigzag_listener_.onChange(params_->zigzag);
    params_->zigzag.addListener(&zigzag_listener_);

    restart_listener_.onChange = [this](juce::Value &) {
        restart_button_.setToggleState(params_->get_restart(), juce::sendNotification);
    };

    // force the GUI to match the model
    restart_listener_.onChange(params_->restart);
    params_->restart.addListener(&restart_listener_);
}

LinearAlgoOptionsComponent::~LinearAlgoOptionsComponent() {
    params_->direction.removeListener(&direction_listener_);
    params_->zigzag.removeListener(&zigzag_listener_);
    params_->restart.removeListener(&restart_listener_);
}

void LinearAlgoOptionsComponent::update_direction() {
    auto state = up_button_.getToggleState() ? 
                LinearParameters::Direction::Up : 
                LinearParameters::Direction::Down ;


    params_->direction = state;
}

void LinearAlgoOptionsComponent::update_zigzag() {
    params_->zigzag = zigzag_button_.getToggleState();
}

void LinearAlgoOptionsComponent::update_restart() {
    params_->restart = restart_button_.getToggleState();
}

//==============================================================
void LinearAlgoOptionsComponent::paint(juce::Graphics&g) {
    DBGLOG("LinearAlgoOptionsComponent::paint called");
    g.fillAll (getLookAndFeel().findColour (juce::PropertyComponent::backgroundColourId));

}

//==========================================================
void LinearAlgoOptionsComponent::resized() {

    DBGLOG("LinearAlgoOptionsComponent::resized called")

    juce::Grid grid;
 
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using GridItem = juce::GridItem;

    grid.alignItems = juce::Grid::AlignItems::start;
    grid.justifyContent = juce::Grid::JustifyContent::start;
    grid.justifyItems = juce::Grid::JustifyItems::start;
    grid.templateColumns = { Track (Fr (1)), Track(Fr(1)), Track (Fr (2)), Track (Fr (2)) };

    grid.templateRows.add(Track (Fr (1)));
    grid.items.add(GridItem(up_button_));
    grid.items.add(GridItem(down_button_));
    grid.items.add(GridItem(zigzag_button_));
    grid.items.add(GridItem(restart_button_));

    grid.performLayout (getLocalBounds());

}


