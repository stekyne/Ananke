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
	deleteAllChildren ();
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
	for (int i = getNumChildComponents (); --i >= 0;)
	{
		auto const fc = dynamic_cast<NodeComponent*> (getChildComponent (i));

		if (fc != nullptr && fc->id == filterID)
			return fc;
	}

	return nullptr;
}

Connector* GraphComponent::getComponentForConnection (const Connection& connection) const
{
	for (int i = getNumChildComponents (); --i >= 0;)
	{
		Connector* const c = dynamic_cast<Connector*>(getChildComponent (i));

		if (c == nullptr)
			continue;

		if (c->sourceFilterID == connection.sourceNode &&
			c->destFilterID == connection.destNode &&
			c->sourceFilterChannel == connection.sourceChannel &&
			c->destFilterChannel == connection.destChannel)
		{
			return c;
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
	{
		delete draggingConnector;
	}

	draggingConnector = dynamic_cast<Connector*> (e.originalComponent);

	/** User didnt click on a connector so create a new one */
	if (draggingConnector == nullptr)
	{
		draggingConnector = new Connector (graph,
			sourceFilterID, sourceFilterChannel,
			destFilterID, destFilterChannel);
	}

	/** Set inital position of the connector */
	draggingConnector->setInput (sourceFilterID, sourceFilterChannel);
	draggingConnector->setOutput (destFilterID, destFilterChannel);

	addAndMakeVisible (draggingConnector);
	draggingConnector->toFront (false);

	dragConnector (e);
}

void GraphComponent::dragConnector (const MouseEvent& e)
{
	const MouseEvent e2 (e.getEventRelativeTo (this));

	if (draggingConnector != nullptr)
	{
		//draggingConnector->setTooltip( String::empty );

		int x = e2.x;
		int y = e2.y;

		auto const pin = findPin (x, y);

		if (pin != nullptr)
		{
			uint32 srcFilter = draggingConnector->sourceFilterID;
			int	srcChannel = draggingConnector->sourceFilterChannel;

			uint32 dstFilter = draggingConnector->destFilterID;
			int	dstChannel = draggingConnector->destFilterChannel;

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

		if (draggingConnector->sourceFilterID == 0)
			draggingConnector->dragStart (x, y);
		else
			draggingConnector->dragEnd (x, y);
	}
}

void GraphComponent::endConnector (const MouseEvent& e)
{
	if (draggingConnector == nullptr)
		return;

	//draggingConnector->setTooltip( String::empty );

	const MouseEvent e2 (e.getEventRelativeTo (this));

	uint32 srcFilter = draggingConnector->sourceFilterID;
	int	srcChannel = draggingConnector->sourceFilterChannel;

	uint32 dstFilter = draggingConnector->destFilterID;
	int dstChannel = draggingConnector->destFilterChannel;

	deleteAndZero (draggingConnector);

	Pin* const pin = findPin (e2.x, e2.y);

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
			DBG ("Connection is successful: " + String (srcFilter) +
				" to " + String (dstFilter));
			updateGraph ();
		}
		else
		{
			DBG ("Connection unsuccessful: " + String (srcFilter) +
				" to " + String (dstFilter));
		}
	}
}

Pin* GraphComponent::findPin (const int x, const int y) const
{
	for (int i = getNumChildComponents (); --i >= 0;)
	{
		auto const fc = dynamic_cast<NodeComponent*>(getChildComponent (i));

		if (fc != nullptr)
		{
			auto const pin = dynamic_cast<Pin*> (
				fc->getComponentAt (x - fc->getX (),
					y - fc->getY ()));

			if (pin != nullptr)
				return pin;
		}
	}

	return nullptr;
}

void GraphComponent::updateGraph ()
{
	for (int i = getNumChildComponents (); --i >= 0;)
	{
		auto const fc = dynamic_cast<NodeComponent*> (getChildComponent (i));

		if (fc != nullptr)
			fc->update ();
	}

	for (int i = getNumChildComponents (); --i >= 0;)
	{
		auto const cc = dynamic_cast<Connector*> (getChildComponent (i));

		if (cc != nullptr && cc != draggingConnector)
		{
			const Connection testConnection (cc->sourceFilterID,
				cc->sourceFilterChannel,
				cc->destFilterID,
				cc->destFilterChannel);

			if (graph.connectionExists (testConnection) == false)
			{
				delete cc;
			}
			else
			{
				cc->update ();
			}
		}
	}

	for (auto& connection : graph.getConnections ())
	{
		if (getComponentForConnection (connection) == nullptr)
		{
			auto const conn = new Connector (graph);

			addAndMakeVisible (conn);

			conn->setInput (connection.sourceNode, connection.sourceChannel);
			conn->setOutput (connection.destNode, connection.destChannel);
		}
	}

	for (auto& node : graph.getNodes ())
	{
		const auto id = node->getID ();

		if (getComponentForFilter (id) == 0)
		{
			NodeComponent* const newNode = new NodeComponent (graph, id);
			addAndMakeVisible (newNode);
			newNode->update ();
		}
	}
}

}
