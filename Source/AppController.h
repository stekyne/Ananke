#pragma once

#ifndef APPCONTROLLER_H_INCLUDED
#define APPCONTROLLER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "WidgetWrapper.h"
#include "../AudioWidgetsLib/Source/GraphModel.h"

class AppController
{
public:
    AppController ();
    ~AppController ();

    // Used to load the last opened project on initial launch
    void loadWidgets ();

    std::shared_ptr<GraphModel> const getGraphModel () const
    {
        return graphModel;
    }

private:
    //juce::ValueTree valueTree;
    OwnedArray<WidgetWrapper> widgets;
    std::shared_ptr<GraphModel> graphModel;
};

#endif