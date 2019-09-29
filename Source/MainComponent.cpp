#pragma once

#include "Controller\AppController.h"
#include "MainComponent.h"
#include "CommandIDs.h"

namespace Ananke {

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
		getWidth () - 2, keyboardHeight - 2);
	menuBar.setBounds (0, 0, getWidth (), 20);
}

PopupMenu MainContentComponent::getMenuForIndex (int topLevelMenuIndex,
	const String& /*menuName*/)
{
	PopupMenu menu;

	switch (topLevelMenuIndex)
	{
	case 0:
		menu.addCommandItem (&commandManager, CommandIDs::menuFileOption, "Open File");
		menu.addCommandItem (&commandManager, CommandIDs::menuExitOption);
		break;

	case 2:
		menu.addCommandItem (&commandManager, CommandIDs::menuSettingsOption);
		break;

	case 3:
		menu.addCommandItem (&commandManager, CommandIDs::menuHelpOption);
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
	case CommandIDs::menuFileOption:
		break;

	case CommandIDs::menuExitOption:
		break;

	case CommandIDs::menuSettingsOption:
		break;

	case CommandIDs::menuHelpOption:
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
		CommandIDs::menuFileOption,
		CommandIDs::menuExitOption,
		CommandIDs::menuSettingsOption,
		CommandIDs::menuHelpOption
	};

	commands.addArray (ids, numElementsInArray (ids));
}

void MainContentComponent::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
{
	const String category ("General");

	switch (commandID)
	{
	case CommandIDs::menuFileOption:
		result.setInfo ("File", "Create a new synth graph", category, 0);
		result.defaultKeypresses.add (KeyPress ('n', ModifierKeys::commandModifier, 0));
		break;

	case CommandIDs::menuExitOption:
		result.setInfo ("Exit", "Exit out of application", category, 0);
		result.defaultKeypresses.add (KeyPress ('w', ModifierKeys::commandModifier, 0));
		break;

	case CommandIDs::menuSettingsOption:
		result.setInfo ("Settings", "Open settings window", category, 0);
		result.defaultKeypresses.add (KeyPress ('s', ModifierKeys::commandModifier, 0));
		break;

	case CommandIDs::menuHelpOption:
		result.setInfo ("Help", "Open help file", category, 0);
		result.defaultKeypresses.add (KeyPress ('h', ModifierKeys::commandModifier, 0));
		break;
	}
}

bool MainContentComponent::perform (const InvocationInfo& info)
{
	switch (info.commandID)
	{
	case CommandIDs::menuFileOption:
		break;

	case CommandIDs::menuExitOption:
		break;

	case CommandIDs::menuSettingsOption:
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
	case CommandIDs::menuHelpOption:
		break;
	}

	return false;
}

}
