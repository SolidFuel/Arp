#pragma once

#if !defined(STARP_DEBUG)
 #define STARP_DEBUG 0
#endif

#include <string>

template<typename ...Args>
std::string concat(Args&&... args) {
    std::stringstream ss;

    (ss << ... << args);

    return ss.str();

}


extern juce::FileLogger *dbgout;

#if STARP_DEBUG
    #define DBGLOG(...) dbgout->logMessage(concat(__VA_ARGS__));
#else
    #define DBGLOG(...)
#endif