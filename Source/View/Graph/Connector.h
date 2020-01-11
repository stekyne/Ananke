#pragma once

#ifndef CONNECTOR_H_INCLUDED
#define CONNECTOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

namespace Ananke {

class GraphComponent;
class Graph;
struct Connection;

class Connector : public Component
{
public:
	Connector () = default;
	Connector (GraphComponent* graphComponent);
	Connector (GraphComponent* graphComponent, const Connection& connection);
	~Connector () = default;

	void setSourceNodeID (int id) { sourceNodeID = id; }
	int getSourceNodeID () const noexcept { return sourceNodeID; }

	void setSourceChannel (int index) { sourceChannel = index; }
	int getSourceChannel () const noexcept { return sourceChannel; }

	void setDestNodeID (int id) { destNodeID = id; }
	int getDestNodeID () const noexcept { return destNodeID; }

	void setDestChannel (int index) { destChannel = index; }
	int getDestChannel () const noexcept { return destChannel; }

	void setInput (const int sourceNodeId, const int sourceNodeChannel);
	void setOutput (const int destNodeId, const int destNodeChannel);

	void dragStart (int x, int y);
	void dragEnd (int x, int y);

	void getPoints (float& x1, float& y1, float& x2, float& y2) const;

	void paint (Graphics& g);

	bool hitTest (int x, int y);

	void resized ();
	void resizeToFit ();

	// When a user clicks within the component, it checks if it hits the path
	// (visual part) and begins dragging the connector if so
	void mouseDown (const MouseEvent& e);

	// Drag one end of the connector and update its position in the graph
	void mouseDrag (const MouseEvent& e);

	// If dragging previously, check if the position is near a pin, if not,
	// disconnect this connector (delete it). if it is near, check if it can connect
	// and act accordingly
	void mouseUp (const MouseEvent& e);

	// 
	void updateBoundsAndRepaint ();

private:
	void getDistancesFromEnds (int x, int y, double& distanceFromStart, double& distanceFromEnd) const
	{
		float x1, y1, x2, y2;
		getPoints (x1, y1, x2, y2);

		distanceFromStart = juce_hypot (x - (x1 - getX ()), y - (y1 - getY ()));
		distanceFromEnd = juce_hypot (x - (x2 - getX ()), y - (y2 - getY ()));
	}

private:
	// Visual path object to represent connector
	Path linePath;
	Path hitPath;

	// Position of the connection on the graph
	float lastx1 = 0.f;
	float lasty1 = 0.f; 
	float lastx2 = 0.f;
	float lasty2 = 0.f;

	// Connection IDs and channels for this object
	int sourceNodeID = 0;
	int sourceChannel = 0;
	int destNodeID = 0;
	int destChannel = 0;

	GraphComponent* graphComponent = nullptr;
	bool dragging = false;

	JUCE_LEAK_DETECTOR (Connector);
};

}

#endif
