#pragma once

#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <memory>
#include <tuple>
#include "../JuceLibraryCode/JuceHeader.h"

namespace Ananke {

class Graph;
class GraphComponent;
class PinComponent;

class NodeComponent : public Component
{
public:
	NodeComponent () = default;
	NodeComponent (GraphComponent* graphComponent, int id);
	~NodeComponent () = default;

	std::tuple<float, float> getPinPos (const int index, const bool isInput);

	void paint (Graphics& g);
	void resized ();
	void update ();

	void mouseDown (const MouseEvent& e);
	void mouseDrag (const MouseEvent& e);
	void mouseUp (const MouseEvent& e);

	const int id;

private:
	friend class GraphComponent;
	GraphComponent* graphComp = nullptr;

private:
	ComponentDragger dragger;
	std::vector<std::unique_ptr<PinComponent>> inputs;
	std::vector<std::unique_ptr<PinComponent>> outputs;
	std::unique_ptr<PinComponent> midiIn;
	std::unique_ptr<PinComponent> midiOut;
	int numIns = 0;
	int numOuts = 0;
	int numInputs = 0;
	int numOutputs = 0;
	bool acceptsMidi = false;
	bool producesMidi = false;
	Font font;

	JUCE_LEAK_DETECTOR (NodeComponent);
};

}

#endif
