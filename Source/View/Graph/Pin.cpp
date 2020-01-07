#include "Pin.h"
#include "GraphComponent.h"

namespace Ananke {

Pin::Pin (Type type, int filterID, int index, GraphComponent* graph) :
	pinType (type),
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
	switch (pinType)
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

void Pin::mouseDown (const MouseEvent& e)
{
	jassert (graph != nullptr);

	graph->beginConnector (IsInput ? 0 : FilterID,
		Index,
		IsInput ? FilterID : 0,
		Index,
		e);
}

void Pin::mouseDrag (const MouseEvent& e)
{
	jassert (graph != nullptr);

	graph->dragConnector (e);
}

void Pin::mouseUp (const MouseEvent& e)
{
	jassert (graph != nullptr);

	graph->endConnector (e);
}

}
