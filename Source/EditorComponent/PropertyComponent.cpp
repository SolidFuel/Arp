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

#include "PropertyComponent.hpp"
#include "../Starp.hpp"


PropertyComponent::PropertyComponent (ProcessorParameters *params) : params_(params) {

    DBGLOG("Setting up PropertyComponent");
    auto apvts = params->apvts.get();

    //==============================================

    speedLabel_.setText ("Speed", juce::dontSendNotification);
    addAndMakeVisible (speedLabel_);
    speedSlider_.setTooltip("How often a note will (possibly) be generated");
    speedAttachment_.reset (new SliderAttachment (*apvts, "speed", speedSlider_));
    addAndMakeVisible(speedSlider_);

    // --------- Gate ------------
    gateLabel_.setText ("Gate %", juce::dontSendNotification);
    gateLabel_.setTooltip("How long the note will be on as a proportion of the speed.");
    gateSlider_.setTextValueSuffix("%");
    gateSlider_.setTooltip("How long the note will be on as a proportion of the speed.");
    gateAttachment_.reset (new SliderAttachment (*apvts, "gate", gateSlider_));
    addAndMakeVisible (gateLabel_);
    addAndMakeVisible(gateSlider_);

    // --------- Probability ------------
    probabilityLabel_.setText ("Probability", juce::dontSendNotification);
    probabilityLabel_.setTooltip("Chance of a note being generated");
    probabilitySlider_.setTextValueSuffix("%");
    probabilitySlider_.setTooltip("Chance of a note being generated");
    probabilityAttachment_.reset (new SliderAttachment (*apvts, "probability", probabilitySlider_));

    addAndMakeVisible (probabilityLabel_);
    addAndMakeVisible(probabilitySlider_);

    // --------- Velocity ------------
    veloLabel_.setText ("Velocity", juce::dontSendNotification);
    veloLabel_.setTooltip("MIDI velocity of the generated note");
    veloSlider_.setTooltip("MIDI velocity of the generated note");
    veloAttachment_.reset (new SliderAttachment (*apvts, "velocity", veloSlider_));

    veloBox_.addAndMakeVisible(veloLabel_);
    veloBox_.addAndMakeVisible(veloSlider_);

    // --------- Velocity Range ------------
    veloRangeLabel_.setText ("Range", juce::dontSendNotification);
    veloRangeLabel_.setTooltip("Range of variance (+/-) of the velocity");
    veloRangeSlider_.setTooltip("Range of variance (+/-) of the velocity");
    veloRangeAttachment_.reset (new SliderAttachment (*apvts, "velocity_range", veloRangeSlider_));

    veloRangeBox_.addAndMakeVisible (veloRangeLabel_);
    veloRangeBox_.addAndMakeVisible(veloRangeSlider_);

    // --------- Advance ------------
    advanceLabel_.setText ("Advance", juce::dontSendNotification);
    advanceLabel_.setTooltip("Variance in the timing - this sets the how much it might be early");

    advanceSlider_.setTooltip("Variance in the timing - this sets the how much it might be early");
    advanceAttachment_.reset (new SliderAttachment (*apvts, "timing_advance", advanceSlider_));

    advanceBox_.addAndMakeVisible (advanceLabel_);
    advanceBox_.addAndMakeVisible(advanceSlider_);

    // --------- Delay ------------
    delayLabel_.setText ("Delay", juce::dontSendNotification);
    delayLabel_.setTooltip("Variance in the timing - this sets the how much it might be late");

    delaySlider_.setTooltip("Variance in the timing - this sets the how much it might be late");
    delayAttachment_.reset (new SliderAttachment (*apvts, "timing_delay", delaySlider_));

    delayBox_.addAndMakeVisible (delayLabel_);
    delayBox_.addAndMakeVisible(delaySlider_);

    // --------- Velocity Group ------------
    veloGroup_.addAndMakeVisible(veloBox_);
    veloGroup_.addAndMakeVisible(veloRangeBox_);
    veloGroup_.setText("Velocity");

    addAndMakeVisible(veloGroup_);

    // --------- Timing Group ------------
    timingGroup_.addAndMakeVisible(advanceBox_);
    timingGroup_.addAndMakeVisible(delayBox_);
    timingGroup_.setText("Timing");

    addAndMakeVisible(timingGroup_);
   

}


//==============================================================================
void PropertyComponent::paint (juce::Graphics& g) {
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

//==============================================================================
void PropertyComponent::resized() {

    DBGLOG("PropertyComponent::resized called")
    
    juce::Grid grid;
 
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using GridItem = juce::GridItem;

    grid.alignItems = juce::Grid::AlignItems::center;
    grid.justifyContent = juce::Grid::JustifyContent::start;
    grid.justifyItems = juce::Grid::JustifyItems::start;
    grid.templateColumns = { Track (Fr (1)), Track(Fr(4)), Track (Fr (1)) };

    //----------------------------------------

    grid.templateRows.add(Track (Fr (10)));
    grid.items.add(GridItem(speedLabel_));
    grid.items.add(GridItem(speedSlider_).withArea(GridItem::Span(1), GridItem::Span(2)));

    grid.templateRows.add(Track (Fr (10)));
    grid.items.add(GridItem(gateLabel_));
    grid.items.add(GridItem(gateSlider_).withArea(GridItem::Span(1), GridItem::Span(2)));

    grid.templateRows.add(Track (Fr (10)));
    grid.items.add(GridItem(probabilityLabel_));
    grid.items.add(GridItem(probabilitySlider_).withArea(GridItem::Span(1), GridItem::Span(2)));

    veloBox_.layoutTemplate = { Track (Fr (1)), Track(Fr(5)) };
    veloRangeBox_.layoutTemplate = { Track (Fr (1)), Track(Fr(5)) };
    veloGroup_.layoutTemplate = { Track (Fr (1)), Track(Fr(1)) };

    grid.templateRows.add(Track (Fr (23)));
    grid.items.add(GridItem(veloGroup_).withArea(GridItem::Span(1), GridItem::Span(3)));

    advanceBox_.layoutTemplate = { Track (Fr (1)), Track(Fr(5)) };
    delayBox_.layoutTemplate = { Track (Fr (1)), Track(Fr(5)) };
    timingGroup_.layoutTemplate = { Track (Fr (1)), Track(Fr(1)) };

    grid.templateRows.add(Track (Fr (23)));
    grid.items.add(GridItem(timingGroup_).withArea(GridItem::Span(1), GridItem::Span(3)));

    grid.performLayout (getLocalBounds());
    
}

