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




PropertyComponent::PropertyComponent (ProcessorParameters *params) : 
        params_(params)
{

    DBGLOG("Setting up PropertyComponent");
    auto apvts = params->apvts.get();

    //==============================================

    // --------- Speed ------------
    

    speedNoteSlider_.setTooltip("How often a note will (possibly) be generated as a note value.");
    speedNoteAttachment_.reset (new SliderAttachment (*apvts, ProcessorParameters::SPEED_NOTE_ID, speedNoteSlider_));
    speedComponent_.add(speedNoteSlider_);
    speedBarSlider_.setTooltip("How often a note will (possibly) be generated (in notes per bar).");
    speedBarAttachment_.reset (new SliderAttachment (*apvts, ProcessorParameters::SPEED_BAR_ID, speedBarSlider_));
    speedComponent_.add(speedBarSlider_);
    speedMSecSlider_.setTooltip("How often a note will (possibly) be generated in milliseconds");
    speedMSecAttachment_.reset (new SliderAttachment (*apvts, ProcessorParameters::SPEED_MSEC_ID, speedMSecSlider_));
    speedComponent_.add(speedMSecSlider_);


    speed_type_value_.referTo(params_->apvts->getParameterAsValue(ProcessorParameters::SPEED_TYPE_ID));
    speed_type_listener_.onChange = [this](juce::Value &v) {
        update_speed_type(SpeedType(int(v.getValue())));
    };
    update_speed_type(SpeedType(int(speed_type_value_.getValue())));
    speed_type_value_.addListener(&speed_type_listener_);

    speedType_.setValue(speed_type_value_);
    speedType_.refresh();

    speedBox_.add(speedType_, 0, 8);
    speedBox_.add(speedComponent_);
    speedBox_.setText("Speed");
    addAndMakeVisible(speedBox_);


    // --------- Probability ------------
    probabilityLabel_.setText ("Probability", juce::dontSendNotification);
    probabilityLabel_.setTooltip("Chance of a note being generated");
    probabilitySlider_.setTextValueSuffix("%");
    probabilitySlider_.setTooltip("Chance of a note being generated");
    probabilityAttachment_.reset (new SliderAttachment (*apvts, "probability", probabilitySlider_));

    addAndMakeVisible(probabilityLabel_);
    addAndMakeVisible(probabilitySlider_);

    // --------- Gate ------------
    gateLabel_.setText ("Gate %", juce::dontSendNotification);
    gateLabel_.setTooltip("How long the note will be on as a proportion of the speed.");
    gateSlider_.setTextValueSuffix("%");
    gateSlider_.setTooltip("How long the note will be on as a proportion of the speed.");
    gateAttachment_.reset (new SliderAttachment (*apvts, "gate", gateSlider_));

    gateBox_.addAndMakeVisible(gateLabel_);
    gateBox_.addAndMakeVisible(gateSlider_);

    // --------- Gate Range ------------
    gateRangeLabel_.setText ("Range", juce::dontSendNotification);
    gateRangeLabel_.setTooltip("Range in variance (+/-) for the gate");
    gateRangeSlider_.setTextValueSuffix("%");
    gateRangeSlider_.setTooltip("Range in variance (+/-) for the gate");
    gateRangeAttachment_.reset (new SliderAttachment (*apvts, "gate_range", gateRangeSlider_));

    gateRangeBox_.addAndMakeVisible(gateRangeLabel_);
    gateRangeBox_.addAndMakeVisible(gateRangeSlider_);

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

    // --------- Gate Group ------------
    gateGroup_.addAndMakeVisible(gateBox_);
    gateGroup_.addAndMakeVisible(gateRangeBox_);
    gateGroup_.setText("Gate");

    addAndMakeVisible(gateGroup_);

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
void PropertyComponent::update_speed_type(SpeedType sp) {

    DBGLOG("PropertyComponent::update_speed_type called = ", sp)

    speedComponent_.setActiveIndex(sp);

    speedType_.refresh();

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

    juce::Array<Track> column_layout   = { Track (Fr (10)), Track(Fr(50)) };
    juce::Array<Track> sub_group_layout = { Track (Fr (1)), Track(Fr(1)) };

    grid.alignItems = juce::Grid::AlignItems::center;
    grid.justifyContent = juce::Grid::JustifyContent::start;
    grid.justifyItems = juce::Grid::JustifyItems::start;
    
    grid.templateColumns = column_layout;


    //----------------------------------------

    grid.templateRows.add(Track (Fr (10)));
    grid.items.add(GridItem(probabilityLabel_));
    grid.items.add(GridItem(probabilitySlider_));

    speedBox_.layoutTemplate = column_layout;

    grid.templateRows.add(Track (Fr (13)));
    grid.items.add(GridItem(speedBox_).withArea(GridItem::Span(1), GridItem::Span(2)));

    gateBox_.layoutTemplate = column_layout;
    gateRangeBox_.layoutTemplate = column_layout;
    gateGroup_.layoutTemplate = sub_group_layout;

    grid.templateRows.add(Track (Fr (23)));
    grid.items.add(GridItem(gateGroup_).withArea(GridItem::Span(1), GridItem::Span(2)));

    veloBox_.layoutTemplate = column_layout;
    veloRangeBox_.layoutTemplate = column_layout;
    veloGroup_.layoutTemplate = sub_group_layout;

    grid.templateRows.add(Track (Fr (23)));
    grid.items.add(GridItem(veloGroup_).withArea(GridItem::Span(1), GridItem::Span(2)));

    advanceBox_.layoutTemplate = column_layout;
    delayBox_.layoutTemplate = column_layout;
    timingGroup_.layoutTemplate = sub_group_layout;

    grid.templateRows.add(Track (Fr (23)));
    grid.items.add(GridItem(timingGroup_).withArea(GridItem::Span(1), GridItem::Span(2)));

    grid.performLayout (getLocalBounds());
    
}

