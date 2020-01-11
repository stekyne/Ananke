#include "GraphComponent.h"
#include "NodeComponent.h"
#include "Connector.h"
#include "PinComponent.h"
#include "../Source/Graph/Connection.h"

namespace Ananke {

GraphComponent::GraphComponent (Graph& graph) :
	graph (graph)
{
	for (const auto& connection : graph.getConnections ())
	{
		connectors.push_back (std::make_unique<Connector> (this, connection));
		addAndMakeVisible (connectors.back ().get ());
	}

	for (const auto& node : graph.getNodes ())
	{
		nodes.push_back (std::make_unique<NodeComponent> (this, node->getID ()));
		addAndMakeVisible (nodes.back ().get ());
	}

	graph.addListener (this);
	setOpaque (true);
	setSize (600, 400);
}

GraphComponent::~GraphComponent ()
{
}

void GraphComponent::resized ()
{
	redrawSubComponents ();
}

void GraphComponent::paint (Graphics& g)
{
	g.fillAll (Colours::darkgrey);
}

NodeComponent* GraphComponent::getComponentForNode (const int nodeId) const
{
	for (auto& node : nodes)
	{
		if (node->id == nodeId)
			return node.get ();
	}

	return nullptr;
}

void GraphComponent::beginConnector (const int sourceNodeId, const int sourceNodeChannel,
	const int destNodeId, const int destNodeChannel, const MouseEvent& e)
{
	// Delete previous connector if it exists, get a pointer to the connector
	// if the user clicked on one
	if (draggingConnector)
	{
		delete draggingConnector;
		draggingConnector = nullptr;
	}

	draggingConnector = dynamic_cast<Connector*> (e.originalComponent);

	// User didnt click on a connector so create a new one
	if (draggingConnector == nullptr)
	{
		draggingConnector = new Connector (this, Connection ( 
			sourceNodeId, sourceNodeChannel,
			destNodeId, destNodeChannel));
	}

	// Set inital position of the connector
	jassert (draggingConnector != nullptr);
	draggingConnector->setInput (sourceNodeId, sourceNodeChannel);
	draggingConnector->setOutput (destNodeId, destNodeChannel);

	addAndMakeVisible (draggingConnector);
	draggingConnector->toFront (false);

	dragConnector (e);
}

void GraphComponent::dragConnector (const MouseEvent& e)
{
	if (draggingConnector == nullptr)
		return;

	const MouseEvent e2 (e.getEventRelativeTo (this));
	//draggingConnector->setTooltip (String::empty);

	int x = e2.x;
	int y = e2.y;

	auto const pin = findPin (x, y);

	if (pin != nullptr)
	{
		auto sourceNode = draggingConnector->getSourceNodeID ();
		auto sourceChannel = draggingConnector->getSourceChannel ();

		auto destNode = draggingConnector->getDestNodeID ();
		auto destChannel = draggingConnector->getDestChannel ();

		if (sourceNode == 0 && !pin->IsInput)
		{
			sourceNode = pin->ParentNodeID;
			sourceChannel = pin->ChannelIndex;
		}
		else if (destNode == 0 && pin->IsInput)
		{
			destNode = pin->ParentNodeID;
			destChannel = pin->ChannelIndex;
		}

		if (graph.canConnect (Connection (sourceNode, sourceChannel, destNode, destChannel)))
		{
			x = pin->getParentComponent ()->getX () +
				pin->getX () + pin->getWidth () / 2;

			y = pin->getParentComponent ()->getY () +
				pin->getY () + pin->getHeight () / 2;

			//draggingConnector->setTooltip (pin->getTooltip());
		}
	}

	if (draggingConnector->getSourceNodeID () == 0)
		draggingConnector->dragStart (x, y);
	else
		draggingConnector->dragEnd (x, y);
}

void GraphComponent::endConnector (const MouseEvent& e)
{
	if (draggingConnector == nullptr)
		return;

	//draggingConnector->setTooltip( String::empty );

	const MouseEvent e2 (e.getEventRelativeTo (this));

	auto sourceNode = draggingConnector->getSourceNodeID ();
	auto sourceChannel = draggingConnector->getSourceChannel ();

	auto destNode = draggingConnector->getDestNodeID ();
	auto destChannel = draggingConnector->getDestChannel ();

	delete draggingConnector;
	draggingConnector = nullptr;

	const auto pin = findPin (e2.x, e2.y);

	if (pin != nullptr)
	{
		if (sourceNode == 0)
		{
			if (pin->IsInput)
				return;

			sourceNode = pin->ParentNodeID;
			sourceChannel = pin->ChannelIndex;
		}
		else
		{
			if (!pin->IsInput)
				return;

			destNode = pin->ParentNodeID;
			destChannel = pin->ChannelIndex;
		}

		if (graph.addConnection (Connection (sourceNode, sourceChannel, destNode, destChannel)))
		{
			DBG ("Connection is successful: " + String (sourceNode) + " to " + String (destNode));
			// TODO remove explict redraw
			redrawSubComponents ();
		}
		else
		{
			DBG ("Connection unsuccessful: " + String (sourceNode) + " to " + String (destNode));
		}
	}
}

PinComponent* GraphComponent::findPin (const int x, const int y) const
{
	for (auto& node : nodes)
	{
		// TODO refactor this
		auto const pin = dynamic_cast<PinComponent*> (node->getComponentAt (x - node->getX (), y - node->getY ()));

		if (pin != nullptr)
			return pin;
	}

	return nullptr;
}

void GraphComponent::redrawSubComponents ()
{
	for (auto& node : nodes)
		node->update ();

	for (auto& connector : connectors)
	{
		if (connector.get() == draggingConnector)
			continue;

		connector->updateBoundsAndRepaint ();
	}
}

void GraphComponent::newNodeAdded (const Node& newNode) 
{
	nodes.push_back (std::make_unique<NodeComponent> (this, newNode.getID ()));
	addAndMakeVisible (nodes.back ().get ());

	redrawSubComponents ();
	DBG ("GraphComp: New node added: " << newNode.getID () << " / " << newNode.getName ());
}

void GraphComponent::nodeRemoved (const Node& removedNode)
{
	nodes.erase (
		std::remove_if (std::begin (nodes), std::end (nodes), [&](const auto& item) { 
			return item->id == removedNode.getID ();
		}),
		std::end (nodes)
	);

	redrawSubComponents ();
	DBG ("GraphComp: Node removed: " << removedNode.getID () << " / " << removedNode.getName ());
}

void GraphComponent::newConnectionAdded (const Connection& newConnection)
{
	connectors.push_back (std::make_unique<Connector> (this, Connection (
		newConnection.sourceNode, newConnection.sourceChannel, 
		newConnection.destNode, newConnection.destChannel)));
	
	addAndMakeVisible (connectors.back ().get ());
	redrawSubComponents ();
}

void GraphComponent::connectionRemoved (const Connection& removedConnection)
{
	connectors.erase (
		std::remove_if (std::begin (connectors), std::end (connectors), [&](const auto& item) {
			return item->getSourceNodeID () == removedConnection.sourceNode && 
				item->getDestNodeID () == removedConnection.destNode &&
				item->getSourceChannel () == removedConnection.sourceChannel &&
				item->getDestChannel () == removedConnection.destChannel;
		}),
		std::end (connectors)
	);

	redrawSubComponents ();
}

}
