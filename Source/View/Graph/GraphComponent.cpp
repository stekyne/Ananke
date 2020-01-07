#include "GraphComponent.h"
#include "NodeComponent.h"
#include "Connector.h"
#include "Pin.h"
#include "../Source/Graph/Connection.h"

namespace Ananke {

GraphComponent::GraphComponent (Graph& graph) :
	graph (graph)
{
	for (const auto& connection : graph.getConnections ())
		connectors.push_back (std::make_unique<Connector> (this, connection));

	for (const auto& node : graph.getNodes ())
		nodes.push_back (std::make_unique<NodeComponent> (this, node->getID ()));

	graph.addListener (this);
	setOpaque (true);
	setSize (600, 400);
}

GraphComponent::~GraphComponent ()
{
}

void GraphComponent::resized ()
{
	updateGraph ();
}

void GraphComponent::paint (Graphics& g)
{
	g.fillAll (Colours::darkgrey);
}

NodeComponent* GraphComponent::getComponentForFilter (const int filterID) const
{
	for (auto& node : nodes)
	{
		if (node->id == filterID)
			return node.get ();
	}

	return nullptr;
}

void GraphComponent::beginConnector (const int sourceFilterID, const int sourceFilterChannel,
	const int destFilterID, const int destFilterChannel, const MouseEvent& e)
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
		draggingConnector = new Connector (this,
			sourceFilterID, sourceFilterChannel,
			destFilterID, destFilterChannel);
	}

	// Set inital position of the connector
	draggingConnector->setInput (sourceFilterID, sourceFilterChannel);
	draggingConnector->setOutput (destFilterID, destFilterChannel);

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
		auto srcFilter = draggingConnector->getSourceNodeID ();
		auto srcChannel = draggingConnector->getSourceChannel ();

		auto dstFilter = draggingConnector->getDestNodeID ();
		auto dstChannel = draggingConnector->getDestChannel ();

		if (srcFilter == 0 && !pin->IsInput)
		{
			srcFilter = pin->FilterID;
			srcChannel = pin->Index;
		}
		else if (dstFilter == 0 && pin->IsInput)
		{
			dstFilter = pin->FilterID;
			dstChannel = pin->Index;
		}

		if (graph.canConnect (Connection (srcFilter, srcChannel, dstFilter, dstChannel)))
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

	auto srcFilter = draggingConnector->getSourceNodeID ();
	auto srcChannel = draggingConnector->getSourceChannel ();

	auto dstFilter = draggingConnector->getDestNodeID ();
	auto dstChannel = draggingConnector->getDestChannel ();

	delete draggingConnector;
	draggingConnector = nullptr;

	auto const pin = findPin (e2.x, e2.y);

	if (pin != nullptr)
	{
		if (srcFilter == 0)
		{
			if (pin->IsInput)
				return;

			srcFilter = pin->FilterID;
			srcChannel = pin->Index;
		}
		else
		{
			if (!pin->IsInput)
				return;

			dstFilter = pin->FilterID;
			dstChannel = pin->Index;
		}

		if (graph.addConnection (Connection (srcFilter, srcChannel, dstFilter, dstChannel)))
		{
			DBG ("Connection is successful: " + String (srcFilter) + " to " + String (dstFilter));
			updateGraph ();
		}
		else
		{
			DBG ("Connection unsuccessful: " + String (srcFilter) + " to " + String (dstFilter));
		}
	}
}

Pin* GraphComponent::findPin (const int x, const int y) const
{
	for (auto& node : nodes)
	{
		auto const pin = dynamic_cast<Pin*> (node->getComponentAt (x - node->getX (), y - node->getY ()));

		if (pin != nullptr)
			return pin;
	}

	return nullptr;
}

void GraphComponent::updateGraph ()
{
	for (auto& node : nodes)
		node->update ();

	for (auto& connector : connectors)
	{
		if (connector.get() == draggingConnector)
			continue;

		connector->update ();
	}
}

void GraphComponent::newNodeAdded (const Node& newNode) 
{
	nodes.push_back (std::make_unique<NodeComponent> (this, newNode.getID ()));

	updateGraph ();
}

void GraphComponent::nodeRemoved (const Node& removedNode)
{
	nodes.erase (
		std::remove_if (std::begin (nodes), std::end (nodes), [&](const auto& item) { 
			return item->id == removedNode.getID ();
		}),
		std::end (nodes)
	);

	updateGraph ();
}

void GraphComponent::newConnectionAdded (const Connection& newConnection)
{
	connectors.push_back (std::make_unique<Connector> (this, newConnection.sourceNode, newConnection.sourceChannel, 
		newConnection.destNode, newConnection.destChannel));

	updateGraph ();
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

	updateGraph ();
}

}
