cmake_minimum_required(VERSION 3.16)
project(Starp VERSION 0.1.0)

#First, we'll add the CMake folder, in case we'll need to find_package later:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

#Compile commands, useful for some IDEs like VS-Code
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

#Minimum MacOS target, set globally

if (${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0 CACHE STRING "Minimum OS X deployment version" FORCE)

    #code signing to run on an iOS device:
#    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer" CACHE STRING "" FORCE)
#    set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "XXXXXXXXXX" CACHE STRING "" FORCE)
else()
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11" CACHE STRING "Minimum OS X deployment version" FORCE)
endif()

option(UniversalBinary "Build universal binary for mac" OFF)

if (UniversalBinary)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE INTERNAL "")
endif()


if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    #static linking in Windows
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options("$<$<CONFIG:Release>:/O2>")
endif()

add_compile_definitions("$<$<CONFIG:Release>:STARP_DEBUG=0>")

#We 'require' that we need juce. If JUCE isn't found, it will revert to what's in
#CMake/Findjuce.cmake, where you can see how JUCE is brought in/configured
find_package(juce REQUIRED)

add_subdirectory(external)

add_subdirectory(Source)

#optionally, we're also adding the unit tests:
option(BUILD_UNIT_TESTS "Build unit tests" ON)

if (BUILD_UNIT_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif()
