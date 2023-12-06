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

#include <juce_core/juce_core.h>

#if !defined(SF_DEBUG)
 #define SF_DEBUG 1
#endif

#include <string>

template<typename ...Args>
std::string concat(Args&&... args) {
    std::stringstream ss;

    (ss << ... << args);

    return ss.str();

}



#if SF_DEBUG
    extern std::unique_ptr<juce::FileLogger> dbgout;
    #define DBGLOG(...) dbgout->logMessage(concat(__VA_ARGS__));
#else
    #define DBGLOG(...)
#endif