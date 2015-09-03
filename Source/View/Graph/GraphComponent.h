#pragma once

#ifndef GRAPHCOMPONENT_H_INCLUDED
#define GRAPHCOMPONENT_H_INCLUDED

#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"

#include "../AudioWidgetsLib/Source/Connection.h"

class Node;
class Connector;
class Pin;
class GraphModel;

class GraphComponent : public Component
{
public:
    GraphComponent (std::shared_ptr<GraphModel> graph, 
                    int numAudioInputs, int numAudioOutputs);
    ~GraphComponent ();

    void resized ();
    void paint (Graphics& g);

    Node* getComponentForFilter (const uint32 filterID) const;

    Connector* getComponentForConnection (const Connection& connection) const;

    /** Create new dragging connector if needed or else take hold of the one
    that was clicked on */
    void beginConnector (const uint32 sourceFilterID, const int sourceFilterChannel,
                         const uint32 destFilterID, const int destFilterChannel,
                         const MouseEvent& e);

    /** Check for any pins that might be close to the mouse position */
    void dragConnector (const MouseEvent& e);

    /** Check if connector is over a pin and add if the connection is valid */
    void endConnector (const MouseEvent& e);

    /** Check if a pin exists at this location in the graph */
    Pin* findPin (const int x, const int y) const;

    /** Update the components on the graph */
    void updateGraph ();

private:
    std::shared_ptr<GraphModel> graph;
    Connector* draggingConnector {nullptr};
    //std::vector<Pin*> inputPins, outputPins;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphComponent);
};

#endif
