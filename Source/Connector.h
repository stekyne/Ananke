#pragma once

#ifndef CONNECTOR_H_INCLUDED
#define CONNECTOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class GraphComponent;
class GraphModel;

class Connector : public Component
{
public:
    Connector () = delete;
    Connector (GraphModel& graph);
    Connector (GraphModel& graph,
               uint32 srcNode, int srcChannel,
               uint32 dstNode, int dstChannel);
    ~Connector () = default;

    void setInput (const uint32 sourceFilterID_, const int sourceFilterChannel_);
    void setOutput (const uint32 destFilterID_, const int destFilterChannel_);

    void dragStart (int x, int y);
    void dragEnd (int x, int y);

    void getPoints (float& x1, float& y1, float& x2, float& y2) const;

    void paint (Graphics& g);

    bool hitTest (int x, int y);

    void resized ();
    void resizeToFit ();

    /** When a user clicks within the component, it checks if it hits the path
    (visual part) and begins dragging the connector if so */
    void mouseDown (const MouseEvent& e);

    /** Drag one end of the connector and update its position in the filter */
    void mouseDrag (const MouseEvent& e);

    /** If dragging previously, check if the position is near a pin, if not,
    disconnect this connector (delete it). if it is near, check if it can connect
    and act accordingly */
    void mouseUp (const MouseEvent& e);

    void update ();

    /** Connection IDs and channels for this object */
    uint32	sourceFilterID, destFilterID;
    int		sourceFilterChannel, destFilterChannel;

private:
    GraphComponent* getGraph () const
    {
        GraphComponent* graphComponent =
            findParentComponentOfClass<GraphComponent> ();

        if (graphComponent == nullptr)
        {
            DBG ("Could not find graph component for pin");
        }

        return graphComponent;
    }

    void getDistancesFromEnds (int x, int y,
                               double& distanceFromStart,
                               double& distanceFromEnd) const
    {
        float x1, y1, x2, y2;
        getPoints (x1, y1, x2, y2);

        distanceFromStart = juce_hypot (x - (x1 - getX ()), y - (y1 - getY ()));
        distanceFromEnd = juce_hypot (x - (x2 - getX ()), y - (y2 - getY ()));
    }

private:
    /** Visual path object to represent connector */
    Path linePath, hitPath;

    /** Position of the connection on the graph */
    float lastx1, lasty1, lastx2, lasty2;

    GraphModel& graph;
    bool dragging;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Connector);
};

#endif
