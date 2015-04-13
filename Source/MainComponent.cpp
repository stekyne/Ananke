#pragma once

#include "MainComponent.h"
#include "AppController.h"

MainContentComponent::MainContentComponent (
    std::shared_ptr<AppController> _appController)
    :   appController (_appController),
        graph (appController->getGraphModel ()),
        midiKeyboard (midiKeyboardState, MidiKeyboardComponent::horizontalKeyboard)
{
    setSize (800, 600);
    addAndMakeVisible (graph);
    addAndMakeVisible (midiKeyboard);
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

    graph.setBounds (0, 0, getWidth (), getHeight () - keyboardHeight);
    midiKeyboard.setBounds (0, getHeight () - keyboardHeight, getWidth (), keyboardHeight);
}
