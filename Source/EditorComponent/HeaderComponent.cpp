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

#include "HeaderComponent.hpp"

#include <version.hpp>

const std::string about_text = "      STARP    \n"
    "Version " + STARP_VERSION + " (" + GIT_HASH + ")\n"
    "Copyright (c) 2023 Mark Hollomon\n"
    "Licensed under GPL 3 (https://opensource.org/license/gpl-3-0/)\n"
    "Source code : https://github.com/mhhollomon/Starp\n"
    "\n"
    "* This program is free software: you can redistribute it and/or modify it\n"
    "* under the terms of the GNU General Public License as published by the \n"
    "* Free Software Foundation, either version 3 of the License, or (at your \n"
    "* option) any later version. This program is distributed in the hope that it \n"
    "* will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty\n"
    "* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the LICENSE file\n"
    "* in the root directory.\n"
    "\n"
    "\n"
    "JUCE Copyright (c) \n"
    "Used by permission under GPL 3\n"
    "Source Code : https://github.com/juce-framework/JUCE\n"
    "\n"
    "TinySHA1 Copyright (c) 2012-22 SAURAV MOHAPATRA mohaps@gmail.com\n"
    "Used by permission\n"
    "    Permission to use, copy, modify, and distribute this software for any purpose\n"
    "    with or without fee is hereby granted,\n"
    "    provided that the above copyright notice and this permission notice appear in all copies."
    ;

HeaderComponent::HeaderComponent() {
    nameLabel_.setText ("STARP", juce::dontSendNotification);
    nameLabel_.setFont(juce::Font(32.0f, juce::Font::bold));
    nameLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible (nameLabel_);

    menuButton_.setButtonText("menu");
    menuButton_.changeWidthToFitText();
    menuButton_.setTriggeredOnMouseDown(true);
    menuButton_.onClick = [this]() { showMenu_(); };

    addAndMakeVisible(menuButton_);

}

void HeaderComponent::paint (juce::Graphics& g) {
    g.fillAll (juce::Colour(0xff060660));

}

//==============================================================================
void HeaderComponent::showMenu_() {
    menuButton_.setEnabled(false);

    juce::PopupMenu menu;

    menu.addItem(1, "About");

    menu.showMenuAsync({}, [this](int r) { processMenu_(r); });

}

//==============================================================================
void HeaderComponent::showAboutBox_() {
    auto options = juce::DialogWindow::LaunchOptions();

    auto* te = new juce::TextEditor();

    te->setMultiLine(true);
    te->setText(about_text, false);
    te->setReadOnly(true);

    te->setSize(600, 400);

    options.dialogTitle = "ABOUT STARP";
    options.content = juce::OptionalScopedPointer<juce::Component>(te, true);

    options.launchAsync();

}

//==============================================================================
void HeaderComponent::processMenu_(int results) {
    switch (results) {
        case 1 :
            showAboutBox_();
            break;
        default :
            break;
    }
    menuButton_.setEnabled(true);
}

constexpr int MARGIN = 10;
//==============================================================================
void HeaderComponent::resized() {

    //const auto component_width = getWidth();
    const auto component_height = getHeight();

    const auto menu_height = component_height / 2.0f;
    menuButton_.changeWidthToFitText(int(menu_height));
    menuButton_.setTopLeftPosition({MARGIN, int(menu_height / 2.0f)});

    nameLabel_.setSize(200, component_height-MARGIN);
    nameLabel_.setCentreRelative(0.5f, 0.5f);
}