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

#include "AlgoChoiceComponent.hpp"
#include "../ParamData.hpp"
#include "../Starp.hpp"


AlgoChoiceComponent::AlgoChoiceComponent() : 
        ChoicePropertyComponent("Algorithm") {

    DBGLOG("Creating Choice Component")
    choices.addArray(AlgorithmChoices);
    listener_.onChange = [this](juce::Value &){ refresh(); };

    for (int i=0; i< AlgorithmIndexes.size(); ++i) {
        auto k = int(AlgorithmIndexes[i]);
        DBGLOG("   insert ", k, " => ", i)
        value_map_.insert({k, i});
    }
    refresh();
    DBGLOG("Creating Choice Component DONE")
}

void AlgoChoiceComponent::setValue(juce::Value &ptr) {
    DBGLOG("Setting value_ptr_");
    value_ptr_.referTo(ptr);
    DBGLOG("Setting value_ptr_ adding Listener");
    value_ptr_.addListener(&listener_);
    DBGLOG("Setting value_ptr_ DONE");
}

void AlgoChoiceComponent::refresh() {
    DBGLOG("AlgoChoiceComponent::refresh called")
    juce::ChoicePropertyComponent::refresh();
}

void AlgoChoiceComponent::addListener(juce::Value::Listener *l) {
    DBGLOG("AlgoChoiceComponent::addListener called")
    value_ptr_.addListener(l);
}

void AlgoChoiceComponent::setIndex(int newIndex) {
    DBGLOG("SETINDEX called = ", newIndex );
    auto v = int(AlgorithmIndexes[newIndex]);
    DBGLOG("     => ", v);
    value_ptr_.setValue(v);
}

int AlgoChoiceComponent::getIndex()	const {
    auto v = int(value_ptr_.getValue());
    DBGLOG("GETINDEX called = ", v);
    auto i = value_map_.at(v);
    DBGLOG("    => ", i);
    return i;
}
