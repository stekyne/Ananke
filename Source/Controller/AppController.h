#pragma once

#ifndef APPCONTROLLER_H_INCLUDED
#define APPCONTROLLER_H_INCLUDED

#include "../AudioManager.h"
#include "../View/WidgetWrapper.h"
#include "../AudioWidgetsLib/Source/GraphModel.h"
#include "../AudioWidgetsLib/Source/Nodes.h"

#include "../JuceLibraryCode/JuceHeader.h"

class AppController :   public AudioManager
{
public:
    AppController ();
    ~AppController ();

    void loadWidgets ();

    std::shared_ptr<GraphModel> getGraphModel () const
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