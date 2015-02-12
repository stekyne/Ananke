#pragma once

#include "AppController.h"

AppController::AppController ()
    : graphModel (new GraphModel)
    //: valueTree (juce::Identifier ("audioWidgets"))
{
    loadWidgets ();
}

AppController::~AppController ()
{

}

void AppController::loadWidgets ()
{

}