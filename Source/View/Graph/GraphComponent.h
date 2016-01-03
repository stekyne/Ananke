#pragma once

#ifndef GRAPHCOMPONENT_H_INCLUDED
#define GRAPHCOMPONENT_H_INCLUDED

#include <memory>
#include "../AudioWidgetsLib/Source/Connection.h"
#include "../JuceLibraryCode/JuceHeader.h"

class Connector;
class Node;
class Pin;
class GraphModel;

class GraphComponent : public Component
{
public:
    GraphComponent (GraphModel& graph);
    ~GraphComponent ();

    void resized ();
    void paint (Graphics& g);

    Node* getComponentForFilter (const uint32 filterID) const;

    Connector* getComponentForConnection (const Connection& connection) const;

    void beginConnector (const uint32 sourceFilterID, const int sourceFilterChannel,
                         const uint32 destFilterID, const int destFilterChannel,
                         const MouseEvent& e);
    void dragConnector (const MouseEvent& e);
    void endConnector (const MouseEvent& e);
    Pin* findPin (const int x, const int y) const;
    
    void updateGraph ();

private:
    GraphModel& graph;
    Connector* draggingConnector {nullptr};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphComponent);
};

#endif
