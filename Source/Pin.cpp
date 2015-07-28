#include "Pin.h"
#include "GraphComponent.h"

Pin::Pin (Type type, uint32 filterID, int index)
    :   pin_type (type),
        filterID (filterID),
        index (index),
        isInput (type == AudioInput || type == MidiInput)
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
    getGraph ()->beginConnector (isInput ? 0 : filterID,
                                 index,
                                 isInput ? filterID : 0,
                                 index,
                                 e);
}

void Pin::mouseDrag (const MouseEvent &e)
{
    getGraph ()->dragConnector (e);
}

void Pin::mouseUp (const MouseEvent &e)
{
    getGraph ()->endConnector (e);
}
