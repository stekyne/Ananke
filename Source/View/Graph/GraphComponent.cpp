#include "GraphComponent.h"
#include "NodeComponent.h"
#include "Connector.h"
#include "Pin.h"
#include "../Source/Graph/Connection.h"

namespace Ananke {

GraphComponent::GraphComponent (Graph& graph) :
	graph (graph)
{
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

Connector* GraphComponent::getComponentForConnection (const Connection& connection) const
{
	for (auto& connector : connectors)
	{
		if (connector->getSourceNodeID () == connection.sourceNode &&
			connector->getDestNodeID () == connection.destNode &&
			connector->getSourceChannel () == connection.sourceChannel &&
			connector->getDestChannel () == connection.destChannel)
		{
			return connector.get ();
		}
	}

	return nullptr;
}

void GraphComponent::beginConnector (const int sourceFilterID, const int sourceFilterChannel,
	const int destFilterID, const int destFilterChannel, const MouseEvent& e)
{
	/** Delete previous connector if it exists, get a pointer to the connector
	if the user clicked on one */
	if (draggingConnector != nullptr)
		draggingConnector.reset ();

	const auto connectorComponent = dynamic_cast<Connector*> (e.originalComponent);

	/** User didnt click on a connector so create a new one */
	if (connectorComponent == nullptr)
	{
		draggingConnector = std::make_unique <Connector> (&graph,
			sourceFilterID, sourceFilterChannel,
			destFilterID, destFilterChannel);
	}

	/** Set inital position of the connector */
	draggingConnector->setInput (sourceFilterID, sourceFilterChannel);
	draggingConnector->setOutput (destFilterID, destFilterChannel);

	addAndMakeVisible (draggingConnector.get ());
	draggingConnector->toFront (false);

	dragConnector (e);
}

void GraphComponent::dragConnector (const MouseEvent& e)
{
	const MouseEvent e2 (e.getEventRelativeTo (this));

	if (draggingConnector == nullptr)
		return;

	//draggingConnector->setTooltip( String::empty );

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
	{
		node->update ();
	}

	for (auto& connector : connectors)
	{
		if (connector.get() == draggingConnector.get ())
			continue;

		const Connection testConnection (
			connector->getSourceNodeID (),
			connector->getSourceChannel (),
			connector->getDestNodeID (),
			connector->getDestChannel ());

		if (graph.connectionExists (testConnection))
		{
			connector->update ();
		}
		else
		{
			connector = nullptr;
		}
	}

	connectors.erase (std::remove_if (connectors.begin (), connectors.end (), 
	[&] (const auto& item) {
		return item == nullptr;
	}), connectors.end ());

	for (auto& connection : graph.getConnections ())
	{
		if (getComponentForConnection (connection) == nullptr)
		{
			auto conn = std::make_unique<Connector> (&graph);
			conn->setInput (connection.sourceNode, connection.sourceChannel);
			conn->setOutput (connection.destNode, connection.destChannel);
			addAndMakeVisible (conn.get ());
			connectors.push_back (std::move (conn));
		}
	}

	for (auto& node : graph.getNodes ())
	{
		if (getComponentForFilter (node->getID ()) == nullptr)
		{
			auto newNode = std::make_unique<NodeComponent> (&graph, node->getID ());
			newNode->update ();
			addAndMakeVisible (newNode.get ());
			nodes.push_back (std::move (newNode));
		}
	}
}

}
