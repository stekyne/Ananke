#pragma once

#ifndef PIN_H_INCLUDED
#define PIN_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

namespace Ananke {

class GraphComponent;

class PinComponent : public Component
{
public:
	enum Type
	{
		AudioInput = 0,
		AudioOutput,
		MidiInput,
		MidiOutput,

		NumberOfTypes
	};

	PinComponent () = default;
	PinComponent (Type type, int nodeId, int index, GraphComponent* graph);
	~PinComponent () = default;

	void paint (Graphics& g);

	void mouseDown (const MouseEvent& e);
	void mouseDrag (const MouseEvent& e);
	void mouseUp (const MouseEvent& e);

	const uint32 ParentNodeID;
	const int ChannelIndex;
	const bool IsInput;

	static const int MidiNum = 0x1000;

private:
	friend class NodeComponent;

	Type pinType;
	GraphComponent* graph = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PinComponent);
};

}

#endif
