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

#include "MainComponent.hpp"

MainComponent::MainComponent (ProcessorParameters *params) {


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
    speedSlider_.setTooltip("How often a note will (possibly) be generated");
    addAndMakeVisible(speedSlider_);
    speedAttachment_.reset (new SliderAttachment (*apvts, "speed", speedSlider_));


    gateLabel_.setText ("Gate %", juce::dontSendNotification);
    addAndMakeVisible (gateLabel_);
    gateSlider_.setTextValueSuffix("%");
    gateSlider_.setTooltip("How long the note will be on as a proportion of the speed.");
    addAndMakeVisible(gateSlider_);
    gateAttachment_.reset (new SliderAttachment (*apvts, "gate", gateSlider_));

    veloLabel_.setText ("Velocity", juce::dontSendNotification);
    addAndMakeVisible (veloLabel_);
    veloSlider_.setTooltip("MIDI velocity of the generated note");
    addAndMakeVisible(veloSlider_);
    veloAttachment_.reset (new SliderAttachment (*apvts, "velocity", veloSlider_));

    veloRangeLabel_.setText ("Velocity Range", juce::dontSendNotification);
    addAndMakeVisible (veloRangeLabel_);
    veloRangeSlider_.setTooltip("Range of variance (+/-) of the velocity");
    addAndMakeVisible(veloRangeSlider_);
    veloRangeAttachment_.reset (new SliderAttachment (*apvts, "velocity_range", veloRangeSlider_));

    probabilityLabel_.setText ("Probability", juce::dontSendNotification);
    addAndMakeVisible (probabilityLabel_);
    probabilitySlider_.setTooltip("Chance of a note being generated");
    addAndMakeVisible(probabilitySlider_);
    probabilityAttachment_.reset (new SliderAttachment (*apvts, "probability", probabilitySlider_));

    advanceLabel_.setText ("Timing Advance", juce::dontSendNotification);
    addAndMakeVisible (advanceLabel_);
    advanceSlider_.setTooltip("Variance in the timing - this sets the how much it might be early");
    addAndMakeVisible(advanceSlider_);
    advanceAttachment_.reset (new SliderAttachment (*apvts, "timing_advance", advanceSlider_));

    delayLabel_.setText ("Timing Delay", juce::dontSendNotification);
    addAndMakeVisible (delayLabel_);
    delaySlider_.setTooltip("Variance in the timing - this sets the how much it might be late");
    addAndMakeVisible(delaySlider_);
    delayAttachment_.reset (new SliderAttachment (*apvts, "timing_delay", delaySlider_));


}

//==============================================================================
void MainComponent::paint (juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

//==============================================================================
void MainComponent::resized() {
    
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

    grid.templateRows.add(Track (Fr (1)));
    grid.items.add(GridItem(advanceLabel_));
    grid.items.add(GridItem(advanceSlider_));

    grid.templateRows.add(Track (Fr (1)));
    grid.items.add(GridItem(delayLabel_));
    grid.items.add(GridItem(delaySlider_));

    grid.performLayout (getLocalBounds());
    
    }

