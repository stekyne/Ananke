#include "Pin.h"
#include "GraphComponent.h"

Pin::Pin (Type type_, uint32 filterID_, int index_, bool isInput_)
    : pin_type (type_),
    filterID (filterID_),
    isInput (isInput_),
    index (index_)
{
    setSize (10, 10);
}

Pin::~Pin ()
{
}

void Pin::paint (Graphics& g)
{
    switch (pin_type)
    {
    case AudioInput:
        g.fillAll (Colours::green);
        break;
    case AudioOutput:
        g.fillAll (Colours::yellow);
        break;
    case MidiInput:
        g.fillAll (Colours::red);
        break;
    case MidiOutput:
        g.fillAll (Colours::white);
        break;

    case NumberOfTypes:
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
