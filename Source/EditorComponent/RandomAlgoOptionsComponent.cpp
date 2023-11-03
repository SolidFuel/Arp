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

#include "RandomAlgoOptionsComponent.hpp"

#include "../Starp.hpp"

//==========================================================
RandomAlgoOptionsComponent::RandomAlgoOptionsComponent(RandomParameters * parms) : params_{parms} {
    value_.referTo(params_->seed_value);
    value_.addListener(this);

    addAndMakeVisible(keyValueLabel_);
    keyValueLabel_.getTextValue().referTo(seed_text_);
    update_seed_display();
    keyLabel_.setText ("Seed", juce::dontSendNotification);
    addAndMakeVisible (keyLabel_);

    changeKeyButton_.setButtonText("New Seed");
    changeKeyButton_.onClick = [this]() { changeKey(); };

    addAndMakeVisible(changeKeyButton_);

}


//==========================================================
void RandomAlgoOptionsComponent::valueChanged(juce::Value &) {
    update_seed_display();
}

void RandomAlgoOptionsComponent::update_seed_display() {
    DBGLOG("RandomAlgoOptionsComponent::update_seed_display called")
    seed_text_ = juce::String::toHexString(juce::int64(value_.getValue()));
}

//==============================================================================
void RandomAlgoOptionsComponent::changeKey() {
    DBGLOG("RandomAlgoOptionsComponent::changeKey called")
    params_->pick_new_key();
}

//==========================================================
void RandomAlgoOptionsComponent::paint(juce::Graphics& g) {
    DBGLOG("RandomAlgoOptionsComponent::paint called");
    g.fillAll (getLookAndFeel().findColour (juce::PropertyComponent::backgroundColourId));

}

//==========================================================
void RandomAlgoOptionsComponent::resized() {

    DBGLOG("RandomAlgoOptionsComponent::resized called");

    juce::Grid grid;
 
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using GridItem = juce::GridItem;

    grid.alignItems = juce::Grid::AlignItems::center;
    grid.justifyContent = juce::Grid::JustifyContent::start;
    grid.justifyItems = juce::Grid::JustifyItems::start;
    grid.templateColumns = { Track (Fr (1)), Track(Fr(3)), Track (Fr (1)), Track (Fr (1))};

    grid.templateRows.add(Track (Fr (1)));
    grid.items.add(GridItem(keyLabel_));
    grid.items.add(GridItem(keyValueLabel_));
    grid.items.add(GridItem(changeKeyButton_).withHeight(float(getHeight() * 0.75)));

    grid.performLayout (getLocalBounds());

}
