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

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class BoxComponent : public juce::GroupComponent {


public :
    enum Orientation { Horizontal, Vertical };

    BoxComponent() {};

    BoxComponent(Orientation o) : orient_(o) {};

    BoxComponent(Orientation o, bool drawBorder) : 
        orient_(o), draw_border_(drawBorder) {};

    juce::Array<juce::Grid::TrackInfo> layoutTemplate;

    void add(juce::Component &c, int inset_x = 0, int inset_y = 0);

    void resized() override ;

    void setTemplate(juce::Array<juce::Grid::TrackInfo> &tmpl);
    void setOrientation(Orientation o) {
        orient_ = o;
    }

    void setDrawBorder(bool d) {
        if (d != draw_border_) {
            draw_border_ = d;
            repaint();
        }
    }

    void paint(juce::Graphics& g) override;


private :
    Orientation orient_ = Vertical;
    bool draw_border_ = false;

    juce::Array<std::pair<int, int>> insets;
};
