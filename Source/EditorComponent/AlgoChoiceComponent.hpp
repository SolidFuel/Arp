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

#include "../ParamData.hpp"
#include "../ValueListener.hpp"
#include <juce_gui_basics/juce_gui_basics.h>

class AlgoChoiceComponent : public juce::ChoicePropertyComponent {

public :
    AlgoChoiceComponent();

    
    virtual void setIndex(int newIndex)	override;
    virtual int getIndex()	const override;

    void setValue(juce::Value &ptr);

    // Be sure you have called setValue() before calling this.
    void addListener(juce::Value::Listener *l);

private :    
    juce::Value value_ptr_;
    ValueListener listener_;


};