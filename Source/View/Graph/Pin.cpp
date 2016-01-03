#include "Pin.h"
#include "GraphComponent.h"

Pin::Pin (Type type, uint32 filterID, int index, 
          GraphComponent& graph) :
    pin_type (type),
    FilterID (filterID),
    Index (index),
    IsInput (type == AudioInput || type == MidiInput),
    graph (graph)
{
    setSize (5, 5);
}

Pin::~Pin ()
{
}

void Pin::paint (Graphics& g)
{
    switch (pin_type)
    {
    case AudioInput:
        g.fillAll (Colours::red);
        break;
    case AudioOutput:
        g.fillAll (Colours::blue);
        break;
    case MidiInput:
        g.fillAll (Colours::yellow);
        break;
    case MidiOutput:
        g.fillAll (Colours::green);
        break;

    default:
        g.fillAll (Colours::black);
        break;
    }
}

void Pin::mouseDown (const MouseEvent &e)
{
    graph.beginConnector (IsInput ? 0 : FilterID,
                          Index,
                          IsInput ? FilterID : 0,
                          Index,
                          e);
}

void Pin::mouseDrag (const MouseEvent &e)
{
    graph.dragConnector (e);
}

void Pin::mouseUp (const MouseEvent &e)
{
    graph.endConnector (e);
}
