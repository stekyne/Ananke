#pragma once

#ifndef GRAPHCOMPONENT_H_INCLUDED
#define GRAPHCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../Source/Graph/Graph.h"

namespace Ananke {

class Connector;
class NodeComponent;
class Pin;

class GraphComponent : public Component, public Graph::Listener
{
public:
	GraphComponent (Graph& graph);
	~GraphComponent ();

	void resized ();
	void paint (Graphics& g);

	NodeComponent* getComponentForFilter (const int filterID) const;
	Connector* getComponentForConnection (const Connection& connection) const;

	void beginConnector (const int sourceFilterID, const int sourceFilterChannel,
		const int destFilterID, const int destFilterChannel, const MouseEvent& e);
	void dragConnector (const MouseEvent& e);
	void endConnector (const MouseEvent& e);
	Pin* findPin (const int x, const int y) const;

	void updateGraph ();
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
