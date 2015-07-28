#pragma once

#include "MainComponent.h"
#include "AppController.h"
#include "CommandIDs.h"

MainContentComponent::MainContentComponent (
    std::shared_ptr<AppController> appController)
    :   appController (appController),
        graph (appController->getGraphModel (), 2, 2),
        midiKeyboard (midiKeyboardState, MidiKeyboardComponent::horizontalKeyboard),
        menuBar (this)
{
    setSize (800, 600);
    addAndMakeVisible (graph);
    addAndMakeVisible (midiKeyboard);
    addAndMakeVisible (menuBar);
    setVisible (true);
}

MainContentComponent::~MainContentComponent ()
{
}

void MainContentComponent::paint (Graphics& /*g*/)
{

}

void MainContentComponent::resized ()
{
    const int keyboardHeight = 40;

    graph.setBounds (0, 0, getWidth (), getHeight () - keyboardHeight - 2);
    midiKeyboard.setBounds (2, getHeight () - keyboardHeight, 
                            getWidth () - 2 , keyboardHeight - 2);
    menuBar.setBounds (0, 0, getWidth (), 20);
}

PopupMenu MainContentComponent::getMenuForIndex (int topLevelMenuIndex,
                                                const String& /*menuName*/)
{
    PopupMenu menu;

    switch (topLevelMenuIndex)
    {
    case 0:
        menu.addCommandItem (&commandManager, Ananke::menuFileOption);
        break;
    case 1:
        menu.addCommandItem (&commandManager, Ananke::menuExitOption);
        break;

    default:
        break;
    }

    return menu;
}

void MainContentComponent::menuItemSelected (int menuItemID, int /*topLevelMenuIndex*/)
{
    switch (menuItemID)
    {
    case Ananke::menuFileOption:
        break;

    case Ananke::menuExitOption:
        break;

    default:
        break;
    }
}

StringArray MainContentComponent::getMenuBarNames ()
{
    const char* const names[] = { "File", "Edit", "Modules", "Help", nullptr };
    return StringArray (names);
}