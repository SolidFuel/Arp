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

#include "AlgoChoiceComponent.hpp"
#include "../ParamData.hpp"
#include "../Starp.hpp"


AlgoChoiceComponent::AlgoChoiceComponent() : 
        ChoicePropertyComponent("Algorithm") {

    DBGLOG("Creating Choice Component")
    choices.addArray(AlgorithmChoices);
    refresh();
    DBGLOG("We have ", choices.size(), " choices")

    listener_.onChange = [this](juce::Value &){ refresh(); };
}

void AlgoChoiceComponent::setValue(juce::Value &ptr) {
    DBGLOG("Setting value_ptr_");
    value_ptr_.referTo(ptr);
    value_ptr_.addListener(&listener_);
}

void AlgoChoiceComponent::addListener(juce::Value::Listener *l) {
    value_ptr_.addListener(l);
}

void AlgoChoiceComponent::setIndex(int newIndex) {
    DBGLOG("SETINDEX called = ", newIndex);
     value_ptr_.setValue(newIndex);
}

int AlgoChoiceComponent::getIndex()	const {
    DBGLOG("GETINDEX called = ", int(value_ptr_.getValue()));
    return int(value_ptr_.getValue());
}
