#pragma once

#ifndef GRAPHCOMPONENT_H_INCLUDED
#define GRAPHCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../Source/Graph/Graph.h"

namespace Ananke {

class Connector;
class NodeComponent;
class PinComponent;

class GraphComponent : public Component, public Graph::Listener
{
public:
	GraphComponent (Graph& graph);
	~GraphComponent ();

	void resized ();
	void paint (Graphics& g);

	NodeComponent* getComponentForNode (const int nodeId) const;

	void beginConnector (const int sourceNodeId, const int sourceNodeChannel,
		const int destNodeId, const int destNodeChannel, const MouseEvent& e);
	void dragConnector (const MouseEvent& e);
	void endConnector (const MouseEvent& e);
	PinComponent* findPin (const int x, const int y) const;

	void redrawSubComponents ();
	Graph* getGraph () noexcept { return &graph; }

private:
	void newNodeAdded (const Node& newNode) override;
	void nodeRemoved (const Node& removedNode) override;
	void newConnectionAdded (const Connection& /*newConnection*/) override;
	void connectionRemoved (const Connection& /*removedConnection*/) override;

private:
	Graph& graph;
	std::vector<std::unique_ptr<NodeComponent>> nodes;
	std::vector<std::unique_ptr<Connector>> connectors;
	Connector* draggingConnector = nullptr;
	 
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphComponent);
};

#endif

}
