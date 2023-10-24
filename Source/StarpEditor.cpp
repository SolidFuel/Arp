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

#include "StarpProcessor.hpp"
#include "StarpEditor.hpp"

#include "Starp.hpp"




//==============================================================================
StarpEditor::StarpEditor (StarpProcessor& p)
    : AudioProcessorEditor (&p), proc_ (p) {


    auto params = p.getParameters();
    auto apvts = params->apvts.get();
    
    //==============================================
    addAndMakeVisible(keyValueLabel_);
    keyValueLabel_.getTextValue().referTo(key_value_);
    key_value_.setValue(juce::String::toHexString(params->random_key_));
    keyLabel_.setText ("Seed", juce::dontSendNotification);
    addAndMakeVisible (keyLabel_);


    //==============================================

    speedLabel_.setText ("Speed", juce::dontSendNotification);
    addAndMakeVisible (speedLabel_);
    addAndMakeVisible(speedSlider_);
    speedAttachment_.reset (new SliderAttachment (*apvts, "speed", speedSlider_));


    gateLabel_.setText ("Gate %", juce::dontSendNotification);
    addAndMakeVisible (gateLabel_);
    gateSlider_.setTextValueSuffix("%");
    addAndMakeVisible(gateSlider_);
    gateAttachment_.reset (new SliderAttachment (*apvts, "gate", gateSlider_));

    veloLabel_.setText ("Velocity", juce::dontSendNotification);
    addAndMakeVisible (veloLabel_);
    addAndMakeVisible(veloSlider_);
    veloAttachment_.reset (new SliderAttachment (*apvts, "velocity", veloSlider_));

    veloRangeLabel_.setText ("Velocity Range", juce::dontSendNotification);
    addAndMakeVisible (veloRangeLabel_);
    addAndMakeVisible(veloRangeSlider_);
    veloRangeAttachment_.reset (new SliderAttachment (*apvts, "velocity_range", veloRangeSlider_));

    probabilityLabel_.setText ("Probability", juce::dontSendNotification);
    addAndMakeVisible (probabilityLabel_);
    addAndMakeVisible(probabilitySlider_);
    probabilityAttachment_.reset (new SliderAttachment (*apvts, "probability", probabilitySlider_));

    //==============================================
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(600, 250);
}

StarpEditor::~StarpEditor() {
    
}

//==============================================================================
void StarpEditor::paint (juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void StarpEditor::resized() {
    juce::Grid grid;
 
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using GridItem = juce::GridItem;

    grid.alignItems = juce::Grid::AlignItems::start;
    grid.justifyContent = juce::Grid::JustifyContent::start;
    grid.justifyItems = juce::Grid::JustifyItems::start;
    grid.templateColumns = { Track (Fr (1)), Track(Fr(5)) };

    grid.templateRows.add(Track (Fr (1)));
    grid.items.add(GridItem(keyLabel_));
    grid.items.add(GridItem(keyValueLabel_));


    grid.templateRows.add(Track (Fr (1)));
    grid.items.add(GridItem(speedLabel_));
    grid.items.add(GridItem(speedSlider_));

    grid.templateRows.add(Track (Fr (1)));
    grid.items.add(GridItem(gateLabel_));
    grid.items.add(GridItem(gateSlider_));

    grid.templateRows.add(Track (Fr (1)));
    grid.items.add(GridItem(probabilityLabel_));
    grid.items.add(GridItem(probabilitySlider_));

    grid.templateRows.add(Track (Fr (1)));
    grid.items.add(GridItem(veloLabel_));
    grid.items.add(GridItem(veloSlider_));

    grid.templateRows.add(Track (Fr (1)));
    grid.items.add(GridItem(veloRangeLabel_));
    grid.items.add(GridItem(veloRangeSlider_));


    grid.performLayout (getLocalBounds());
    
    }
