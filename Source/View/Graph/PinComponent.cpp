#include "PinComponent.h"
#include "GraphComponent.h"

namespace Ananke {

PinComponent::PinComponent (Type type, int nodeId, int index, GraphComponent* graph) :
	pinType (type),
	ParentNodeID (nodeId),
	ChannelIndex (index),
	IsInput (type == AudioInput || type == MidiInput),
	graph (graph)
{
	setSize (5, 5);
}

void PinComponent::paint (Graphics& g)
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

void PinComponent::mouseDown (const MouseEvent& e)
{
	jassert (graph != nullptr);

	graph->beginConnector (IsInput ? 0 : ParentNodeID,
		ChannelIndex,
		IsInput ? ParentNodeID : 0,
		ChannelIndex, e);
}

void PinComponent::mouseDrag (const MouseEvent& e)
{
	jassert (graph != nullptr);
	
	graph->dragConnector (e);
}

void PinComponent::mouseUp (const MouseEvent& e)
{
	jassert (graph != nullptr);

	graph->endConnector (e);
}

}
