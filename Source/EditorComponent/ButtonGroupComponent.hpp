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

class ButtonGroupComponent : public juce::GroupComponent {

public :
    enum Orientation { Horizontal, Vertical };

    ButtonGroupComponent() {};

    ButtonGroupComponent(int groupId, Orientation o = Horizontal, bool border = false) : 
        orient_(o), group_id_(groupId), draw_border_(border) {};

    void add(juce::TextButton *btn) {
        juce::GroupComponent::addAndMakeVisible(btn);
        buttons.add(btn);
        btn->setRadioGroupId(group_id_);
        btn->setClickingTogglesState(true);
    }

    void add(juce::TextButton &btn) {
        juce::GroupComponent::addAndMakeVisible(btn);
        buttons.add(&btn);
        btn.setRadioGroupId(group_id_);
        btn.setClickingTogglesState(true);
    }


    void resized() override ;

    void setOrientation(Orientation o) {
        if (o != orient_) {
            orient_ = o;
            repaint();
        }
    }

    void setGroupId(int groupId) {
        if (groupId != group_id_) {
            group_id_ = groupId;
            for (auto *b : buttons ) {
                b->setRadioGroupId(group_id_);
            }
        }
    }

    void setDrawBorder(bool d) {
        if (d != draw_border_) {
            draw_border_ = d;
            repaint();
        }
    }

    void paint(juce::Graphics& g) override;


private :
    bool draw_border_ = false;

    Orientation orient_ = Horizontal;

    int group_id_ = 1001;

    juce::Array<juce::TextButton *> buttons;

};
