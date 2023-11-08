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

#include "BoxComponent.hpp"

#include "../Starp.hpp"

void BoxComponent::setTemplate(juce::Array<juce::Grid::TrackInfo> &tmpl) {
    layoutTemplate.clearQuick();
    layoutTemplate.addArray(tmpl);

    DBGLOG("layouttemplate = ", layoutTemplate.size())
}


void BoxComponent::paint(juce::Graphics& g) {
    DBGLOG("BoxComponent::paint = ", draw_border_)
    if (draw_border_) {
        getLookAndFeel().drawGroupComponentOutline (g, getWidth(), getHeight(),
                                            getText(), getTextLabelPosition(), *this);
    } else {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    }
}

void BoxComponent::add(juce::Component &c, int inset_x, int inset_y) {
    addAndMakeVisible(c);
    insets.add({inset_x, inset_y});
    
}


void BoxComponent::resized() {

    DBGLOG("BoxComponent::resized called")
    
    juce::Grid grid;
 
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using GridItem = juce::GridItem;

    grid.alignItems = juce::Grid::AlignItems::center;
    grid.justifyContent = juce::Grid::JustifyContent::start;
    grid.justifyItems = juce::Grid::JustifyItems::start;

    if (orient_ == Horizontal) { 
        grid.templateColumns.addArray(layoutTemplate);
        grid.templateRows.add(Track (Fr (1)));

    } else {
        grid.templateRows.addArray(layoutTemplate);
        grid.templateColumns.add(Track (Fr (1)));
    }

    int index = 0;
    for(auto *c : getChildren()) {
        auto margin = juce::GridItem::Margin::Margin();
        if (index < insets.size()) {
            auto inset = insets[index];
            margin.left = margin.right = float(inset.first);
            margin.top = margin.bottom = float(inset.second);
        }
        grid.items.add(GridItem(c).withMargin(margin));
    }

    auto bounds = getLocalBounds();
    if (draw_border_) {
        bounds = bounds.reduced(5);
        bounds.removeFromTop(10);
    }
    grid.performLayout (bounds);

}