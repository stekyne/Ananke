#pragma once

#ifndef APPCONTROLLER_H_INCLUDED
#define APPCONTROLLER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "WidgetWrapper.h"
#include "../AudioWidgetsLib/Source/GraphModel.h"
#include "../AudioWidgetsLib/Source/Nodes.h"

class AppController
{
public:
    AppController ();
    ~AppController ();

    void loadWidgets ();

    std::shared_ptr<GraphModel> const getGraphModel () const
    {
        return graphModel;
    }

private:
    void loadTestData ();

private:
    //juce::ValueTree valueTree;
    OwnedArray<WidgetWrapper> widgets;
    std::shared_ptr<GraphModel> graphModel;
};

#endif