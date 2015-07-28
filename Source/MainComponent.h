#pragma once

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <memory>

#include "GraphComponent.h"

class AppController;

class MainContentComponent   :  public Component,
                                public MenuBarModel
{
public:
    MainContentComponent (std::shared_ptr<AppController> appController);
    ~MainContentComponent ();

    void paint (Graphics&);
    void resized ();

private:
    // Menu Bar Model
    virtual PopupMenu getMenuForIndex (int topLevelMenuIndex,
                                       const String& menuName) override;

    virtual void menuItemSelected (int menuItemID,
                                   int topLevelMenuIndex) override;

    StringArray getMenuBarNames () override;

private:
    std::shared_ptr<AppController> appController;
    GraphComponent graph;
    ApplicationCommandManager commandManager;
    MidiKeyboardState midiKeyboardState;
    MidiKeyboardComponent midiKeyboard;
    MenuBarComponent menuBar;
 
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif
