/****
 * solidArp - Stable Random Arpeggiator Plugin 
 * Copyright (C) 2023 Solid Fuel
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

/*
 * A Component that manages several components that will overlay each other.
 * Only one child component will be shown at a time. Think of this as a 
 * tabbed interface without the tabs actually showing.
 * 
 * I suspect the template isn't actually needed and I could set ComponentType
 * to simply juce::Component and it would work. But this lets you check for problems
 * if you tighten the requirements.
*/
template <class ComponentType> class OverlayComponent : public juce::Component {

    juce::Array<ComponentType *> components;
    int active_index = -1;

    void setup_new_component(ComponentType * c) {
        components.add(c);
        if (active_index < 0 ) {
            c->setVisible(true);
            active_index = components.size()-1;
        } else {
            c->setVisible(false);
        }

    }

public :
    OverlayComponent() {}
    
    void add(ComponentType * c) {
        setup_new_component(c);
        addChildComponent(c);
    }

    void add(ComponentType &c) {
        setup_new_component(&c);
        addChildComponent(c);
    }

    void setActiveIndex(int new_index) {
        if (new_index < components.size() && new_index >= 0 && new_index != active_index) {
            active_index = new_index;
            refresh();
        }
    }

    void refresh() {

        for (int i = 0; i < components.size(); ++i ) {
            components[i]->setVisible(i == active_index);
        }

        resized();
    }

    void resized() override {
        components[active_index]->setBounds(getLocalBounds());
    }
};