#pragma once

#include "MainComponent.h"
#include "AppController.h"

MainContentComponent::MainContentComponent (
    std::shared_ptr<AppController> _appController)
    :   appController (_appController),
        graph (appController->getGraphModel ())
{
    setSize (800, 600);
    addAndMakeVisible (graph);
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
    graph.setBounds (0, 0, getWidth (), getHeight ());
}
