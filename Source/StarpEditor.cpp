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
    : AudioProcessorEditor (&p), proc_ (p), main_component(p.getParameters()) {

    addAndMakeVisible(header_component);
    addAndMakeVisible(main_component);

    setSize(600, 300);
}

StarpEditor::~StarpEditor() {
    
}

//==============================================================================
void StarpEditor::paint (juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void StarpEditor::resized() {

    using FlexItem = juce::FlexItem;
    juce::FlexBox box;

    box.flexDirection = juce::FlexBox::Direction::column;
    box.alignContent = juce::FlexBox::AlignContent::center;

    box.items.add(FlexItem(float(getWidth()), 50.0f, header_component).withMinWidth(10.0f));
    box.items.add(FlexItem(float(getWidth()), 250.0f, main_component).withMinWidth(10.0f));


    box.performLayout (juce::Rectangle(0, 0, getWidth(), getHeight()));


    
    }
