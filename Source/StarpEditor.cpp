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


constexpr int WIDTH = 600;
constexpr int HEADER_HEIGHT = 50;
constexpr int MAIN_HEIGHT = 300;
constexpr int HEIGHT = HEADER_HEIGHT + MAIN_HEIGHT;
constexpr int MARGIN = 5;

//==============================================================================
StarpEditor::StarpEditor (StarpProcessor& p)
    : AudioProcessorEditor (&p), proc_ (p), main_component(p.getParameters()) {

    addAndMakeVisible(header_component);
    addAndMakeVisible(main_component);

    setSize(WIDTH, HEIGHT);
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

    box.items.add(FlexItem(float(WIDTH), float(HEADER_HEIGHT), header_component));

    box.items.add(FlexItem(float(WIDTH-(MARGIN*2)), float(MAIN_HEIGHT), main_component)
            .withMargin(FlexItem::Margin(0, MARGIN, 0, MARGIN)));


    box.performLayout (juce::Rectangle(0, 0, getWidth(), getHeight()));


    
    }
