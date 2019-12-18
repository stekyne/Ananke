#pragma once

#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"

namespace Ananke {

class Graph;
class GraphComponent;
class Pin;

class NodeComponent : public Component
{
public:
	NodeComponent () = default;
	NodeComponent (Graph* graph, int id);
	~NodeComponent ();

	void getPinPos (const int index, const bool isInput, float& x, float& y);

	void paint (Graphics& g);
	void resized ();
	void update ();

	void mouseDown (const MouseEvent& e);
	void mouseDrag (const MouseEvent& e);
	void mouseUp (const MouseEvent& e);

	const int id;

private:
	friend class GraphComponent;
	GraphComponent* getGraph () const;
	mutable GraphComponent* graphComp{ nullptr };

private:
	ComponentDragger dragger;
	std::vector<Pin*> inputs;
	std::vector<Pin*> outputs;
	Font font;
	Pin* midiIn = nullptr;
	Pin* midiOut = nullptr;
	Graph* graph = nullptr;
	int numIns = 0;
	int numOuts = 0;
	int numInputs = 0;
	int numOutputs = 0;
	bool acceptsMidi = false;
	bool producesMidi = false;

	JUCE_LEAK_DETECTOR (NodeComponent);
};

}

#endif
