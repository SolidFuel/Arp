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

#include "AlgorithmComponent.hpp"
#include "../Starp.hpp"

AlgorithmComponent::AlgorithmComponent (ProcessorParameters *params) : params_(params),
    randomComponent_(&params->random_parameters),
    linearComponent_(&params->linear_parameters),
    spiralComponent_(&params->spiral_parameters) {

    DBGLOG("AlgorithmComponent constructor called");


    algoComponent_.setValue(params->algorithm_index);
    addAndMakeVisible(algoComponent_);
    algoComponent_.refresh();
    algoComponent_.addListener(this);
    
    addChildComponent(randomComponent_);
    addChildComponent(linearComponent_);
    addChildComponent(spiralComponent_);
    
    DBGLOG("Setting up AlgorithmComponent CHECK 1");

    // Make sure we are in synch with the current value
    valueChanged(params->algorithm_index);

}

void AlgorithmComponent::valueChanged(juce::Value &) { 
    auto algo = params_->get_algo_index();

    DBGLOG("AlgorithmComponent::valueChanged = ", algo)

    randomComponent_.setVisible(algo == Algorithm::Random);
    linearComponent_.setVisible(algo == Algorithm::Linear);
    spiralComponent_.setVisible(algo == Algorithm::Spiral);

    resized();

}

//==============================================================================
void AlgorithmComponent::paint (juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::PropertyComponent::backgroundColourId));

}


//==============================================================================
void AlgorithmComponent::resized() {

    DBGLOG("AlgorithmComponent::resized called")
    
    juce::Grid grid;
 
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using GridItem = juce::GridItem;

    grid.alignItems = juce::Grid::AlignItems::center;
    grid.justifyContent = juce::Grid::JustifyContent::start;
    grid.justifyItems = juce::Grid::JustifyItems::start;
    grid.templateColumns = { Track (Fr (1)) };

    //----------------------------------------
    // Algorithm Choice
    grid.templateRows.add(Track (Fr (1)));
    grid.items.add(GridItem(algoComponent_).withMargin({5, 30, 5, 30}));

    DBGLOG("AlgorithmComponent Algorithm Choice DONE");

    //----------------------------------------
    // Algorithm Specific Options

    auto algo = params_->get_algo_index();
    DBGLOG("algo = ", algo)
    juce::Component *optionComponent = nullptr;
    switch (algo) {
        case Algorithm::Random :
            optionComponent = &randomComponent_;
            break;
        case Algorithm::Linear :
            optionComponent = &linearComponent_;
            break;
        case Algorithm::Spiral :
            optionComponent = &spiralComponent_;
            break;
        default :
            jassertfalse;
    }

    jassert(optionComponent != nullptr);

    grid.templateRows.add(Track (Fr (1)));
    grid.items.add(GridItem(*optionComponent).withMargin({0, 5, 0, 5}));

    DBGLOG("AlgorithmComponent Algorithm Options DONE");

    grid.performLayout (getLocalBounds());
}

