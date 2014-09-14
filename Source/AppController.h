#pragma once

#ifndef APPCONTROLLER_H_INCLUDED
#define APPCONTROLLER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "WidgetWrapper.h"

class AppController
{
public:
    AppController ();
    ~AppController ();

    // Used to load the last opened project on initial launch
    void loadWidgets ();

private:
    //juce::ValueTree valueTree;
    OwnedArray<WidgetWrapper> widgets;
};

#endif