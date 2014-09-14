#pragma once

#include "MainComponent.h"
#include "AppController.h"

MainContentComponent::MainContentComponent (std::shared_ptr<AppController> _appController)
    : appController (_appController)
{
    setSize (800, 600);
}

MainContentComponent::~MainContentComponent ()
{
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xffeeddff));
}

void MainContentComponent::resized ()
{
    
}
