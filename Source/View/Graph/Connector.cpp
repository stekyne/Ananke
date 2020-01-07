#include "Connector.h"
#include "NodeComponent.h"
#include "GraphComponent.h"
#include "../Source/Graph/Graph.h"
#include "../Source/Graph/Connection.h"

namespace Ananke {

Connector::Connector (GraphComponent* graphComponent) :
	graphComponent (graphComponent)
{
}

Connector::Connector (GraphComponent* graphComponent, int srcNodeComponent, int srcChannel,
	int dstNodeComponent, int dstChannel) :
	graphComponent (graphComponent),
	sourceNodeID (srcNodeComponent),
	sourceChannel (srcChannel),
	destNodeID (dstNodeComponent),
	destChannel (dstChannel)
{
}

Connector::Connector (GraphComponent* graphComponent, const Connection& connection) :
	graphComponent (graphComponent),
	sourceNodeID (connection.sourceNode),
	sourceChannel (connection.sourceChannel),
	destNodeID (connection.destNode),
	destChannel (connection.destChannel)
{

}

void Connector::setInput (const int sourceFilterID_, const int sourceFilterChannel_)
{
	if (sourceNodeID != sourceFilterID_ || sourceChannel != sourceFilterChannel_)
	{
		sourceNodeID = sourceFilterID_;
		sourceChannel = sourceFilterChannel_;
		update ();
	}
}

void Connector::setOutput (const int destFilterID_, const int destFilterChannel_)
{
	if (destNodeID != destFilterID_ || destChannel != destFilterChannel_)
	{
		destNodeID = destFilterID_;
		destChannel = destFilterChannel_;
		update ();
	}
}

void Connector::dragStart (int x, int y)
{
	lastx1 = (float)x;
	lasty1 = (float)y;
	resizeToFit ();
}

void Connector::dragEnd (int x, int y)
{
	lastx2 = (float)x;
	lasty2 = (float)y;
	resizeToFit ();
}

void Connector::paint (Graphics& g)
{
	g.setColour (Colours::whitesmoke);
	g.fillPath (linePath);
}

bool Connector::hitTest (int x, int y)
{
	if (hitPath.contains ((float)x, (float)y))
	{
		double distanceFromStart, distanceFromEnd;
		getDistancesFromEnds (x, y, distanceFromStart, distanceFromEnd);

		// avoid clicking the connector when over a pin
		return distanceFromStart > 7.0 && distanceFromEnd > 7.0;
	}

	return false;
}

void Connector::getPoints (float& x1, float& y1, float& x2, float& y2) const
{
	jassert (graphComponent != nullptr);

	x1 = lastx1;
	y1 = lasty1;
	x2 = lastx2;
	y2 = lasty2;

	if (graphComponent != nullptr)
	{
		const auto srcFilterComp = graphComponent->getComponentForFilter (sourceNodeID);

		if (srcFilterComp != nullptr)
			srcFilterComp->getPinPos (sourceChannel, false, x1, y1);

		const auto dstFilterComp = graphComponent->getComponentForFilter (destNodeID);

		if (dstFilterComp != nullptr)
			dstFilterComp->getPinPos (destChannel, true, x2, y2);
	}
}

void Connector::resized ()
{
	float x1, y1, x2, y2;
	getPoints (x1, y1, x2, y2);

	lastx1 = x1;
	lasty1 = y1;
	lastx2 = x2;
	lasty2 = y2;

	x1 -= getX ();
	y1 -= getY ();
	x2 -= getX ();
	y2 -= getY ();

	linePath.clear ();
	linePath.startNewSubPath (x1, y1);
	linePath.cubicTo (x1, y1 + (y2 - y1) * 0.33f,
		x2, y1 + (y2 - y1) * 0.66f,
		x2, y2);

	PathStrokeType wideStroke (8.0f);
	wideStroke.createStrokedPath (hitPath, linePath);

	PathStrokeType stroke (2.5f);
	stroke.createStrokedPath (linePath, linePath);

	const float arrowW = 5.0f;
	const float arrowL = 4.0f;

	Path arrow;
	arrow.addTriangle (-arrowL, arrowW, -arrowL, -arrowW, arrowL, 0.0f);

	arrow.applyTransform (AffineTransform::rotation (float_Pi * 0.5f - (float)atan2 (x2 - x1, y2 - y1))
		.translated ((x1 + x2) * 0.5f, (y1 + y2) * 0.5f));

	linePath.addPath (arrow);
	linePath.setUsingNonZeroWinding (true);
}

void Connector::update ()
{
	float x1, y1, x2, y2;
	getPoints (x1, y1, x2, y2);

	if (lastx1 != x1 || lasty1 != y1 ||
		lastx2 != x2 || lasty2 != y2)
	{
		resizeToFit ();
	}
}

void Connector::resizeToFit ()
{
	float x1, y1, x2, y2;
	getPoints (x1, y1, x2, y2);

	const Rectangle<int> newBounds ((int)jmin (x1, x2) - 4,
		(int)jmin (y1, y2) - 4,
		(int)fabsf (x1 - x2) + 8,
		(int)fabsf (y1 - y2) + 8);

	if (newBounds != getBounds ())
		setBounds (newBounds);
	else
		resized ();

	repaint ();
}

void Connector::mouseDown (const MouseEvent& /*e*/)
{
	dragging = false;
}

void Connector::mouseDrag (const MouseEvent& e)
{
	jassert (graphComponent != nullptr);

	if (!dragging && !e.mouseWasClicked ())
	{
		dragging = true;

		graphComponent->getGraph()->removeConnection (Ananke::Connection (sourceNodeID, sourceChannel,
			destNodeID, destChannel));

		double distanceFromStart, distanceFromEnd;
		getDistancesFromEnds (e.x, e.y, distanceFromStart, distanceFromEnd);
		const bool isNearerSource = (distanceFromStart < distanceFromEnd);

		graphComponent->beginConnector (isNearerSource ? 0 : sourceNodeID,
			sourceChannel,
			isNearerSource ? destNodeID : 0,
			destChannel,
			e);
	}
	else if (dragging)
	{
		graphComponent->dragConnector (e);
	}
}

void Connector::mouseUp (const MouseEvent& e)
{
	jassert (graphComponent != nullptr);

	if (dragging)
	{
		graphComponent->endConnector (e);
	}
}

}
