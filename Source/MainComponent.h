#pragma once

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <memory>

#include "View\Graph\GraphComponent.h"

class AppController;

class MainContentComponent   :  public Component,
                                public MenuBarModel,
                                public ApplicationCommandTarget
{
public:
    MainContentComponent (AppController& appController);
    ~MainContentComponent ();

    void paint (Graphics&) override;
    void resized () override;

private:
    // Menu Bar Model
    virtual PopupMenu getMenuForIndex (int topLevelMenuIndex, const String& menuName) override;
    virtual void menuItemSelected (int menuItemID, int topLevelMenuIndex) override;
    StringArray getMenuBarNames () override;

    // Application Command Target
    ApplicationCommandTarget* getNextCommandTarget () override;
    void getAllCommands (Array<CommandID>& commands) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform (const InvocationInfo& info) override;

private:
    AppController& appController;
    GraphComponent graph;
    ApplicationCommandManager commandManager;
    MidiKeyboardState midiKeyboardState;
    MidiKeyboardComponent midiKeyboard;
    MenuBarComponent menuBar;
 
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif
