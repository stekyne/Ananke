#pragma once

#include "Controller\AppController.h"
#include "MainComponent.h"
#include "CommandIDs.h"

MainContentComponent::MainContentComponent (AppController& appController) :
    appController (appController),
    graph (appController.getGraphModel ()),
    midiKeyboard (midiKeyboardState, MidiKeyboardComponent::horizontalKeyboard),
    menuBar (this)
{
    commandManager.registerAllCommandsForTarget (this);
    setSize (800, 600);
    addAndMakeVisible (graph);
    addAndMakeVisible (midiKeyboard);
    addAndMakeVisible (menuBar);
    setVisible (true);
}

MainContentComponent::~MainContentComponent ()
{
}

void MainContentComponent::paint (Graphics&)
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
        menu.addCommandItem (&commandManager, Ananke::menuFileOption, "Open File");
        menu.addCommandItem (&commandManager, Ananke::menuExitOption);
        break;

    case 2:
        menu.addCommandItem (&commandManager, Ananke::menuSettingsOption);
        break;

    case 3:
        menu.addCommandItem (&commandManager, Ananke::menuHelpOption);
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

    case Ananke::menuSettingsOption:
        break;
    
    case Ananke::menuHelpOption:
        break;
    }
}

StringArray MainContentComponent::getMenuBarNames ()
{
    return StringArray ({ 
        "File", 
		"Edit", 
		"Settings", 
		"Help", 
    });
}

ApplicationCommandTarget* MainContentComponent::getNextCommandTarget ()
{
    return findFirstTargetParentComponent ();
}

void MainContentComponent::getAllCommands (Array<CommandID>& commands)
{
    const CommandID ids[] = {
        Ananke::menuFileOption,
        Ananke::menuExitOption,
        Ananke::menuSettingsOption,
        Ananke::menuHelpOption
    };

    commands.addArray (ids, numElementsInArray (ids));
}

void MainContentComponent::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
{
    const String category ("General");

    switch (commandID)
    {
    case Ananke::menuFileOption:
        result.setInfo ("File", "Create a new synth graph", category, 0);
        result.defaultKeypresses.add (KeyPress ('n', ModifierKeys::commandModifier, 0));
        break;

    case Ananke::menuExitOption:
        result.setInfo ("Exit", "Exit out of application", category, 0);
        result.defaultKeypresses.add (KeyPress ('w', ModifierKeys::commandModifier, 0));
        break;

    case Ananke::menuSettingsOption:
        result.setInfo ("Settings", "Open settings window", category, 0);
        result.defaultKeypresses.add (KeyPress ('s', ModifierKeys::commandModifier, 0));
        break;

    case Ananke::menuHelpOption:
        result.setInfo ("Help", "Open help file", category, 0);
        result.defaultKeypresses.add (KeyPress ('h', ModifierKeys::commandModifier, 0));
        break;
    }
}

bool MainContentComponent::perform (const InvocationInfo& info)
{
    switch (info.commandID)
    {
    case Ananke::menuFileOption:
        break;

    case Ananke::menuExitOption:
        break;

    case Ananke::menuSettingsOption: 
    {
        auto selector = appController.getSelector ();

        DialogWindow::LaunchOptions o;
        o.content.setNonOwned (selector.get ());
        o.dialogTitle = "Audio Settings";
        o.componentToCentreAround = this;
        o.dialogBackgroundColour = Colours::azure;
        o.escapeKeyTriggersCloseButton = true;
        o.useNativeTitleBar = false;
        o.resizable = false;

        o.runModal ();
        return true;
    }
    case Ananke::menuHelpOption:
        break;
    }

    return false;
}