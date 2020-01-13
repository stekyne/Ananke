#pragma once

#ifndef CONNECTOR_H_INCLUDED
#define CONNECTOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

namespace Ananke {

class GraphComponent;
class Graph;
struct Connection;

class ConnectionComponent : public Component
{
public:
	ConnectionComponent () = default;
	ConnectionComponent (GraphComponent* graphComponent);
	ConnectionComponent (GraphComponent* graphComponent, const Connection& connection);
	~ConnectionComponent () = default;

	friend bool operator== (const ConnectionComponent& left, const ConnectionComponent& right)
	{
		return left.getSourceNodeId () == right.getSourceNodeId () &&
			left.getSourceChannel () == right.getSourceChannel () &&
			left.getDestNodeId () == right.getDestNodeId () &&
			left.getDestChannel () == right.getDestChannel ();
	}

	void setSourceNodeID (int id) { sourceNodeId = id; }
	int getSourceNodeId () const noexcept { return sourceNodeId; }

	void setSourceChannel (int index) { sourceChannel = index; }
	int getSourceChannel () const noexcept { return sourceChannel; }

	void setDestNodeID (int id) { destNodeId = id; }
	int getDestNodeId () const noexcept { return destNodeId; }

	void setDestChannel (int index) { destChannel = index; }
	int getDestChannel () const noexcept { return destChannel; }

	void setInput (const int sourceNodeId, const int sourceNodeChannel);
	void setOutput (const int destNodeId, const int destNodeChannel);

	void dragStart (int x, int y);
	void dragEnd (int x, int y);
	bool isCurrentlyDragging () const noexcept { return isDragging; }

	void getStartAndEndPoints (float& x1, float& y1, float& x2, float& y2) const;

	void paint (Graphics& g);

	bool hitTest (int x, int y);

	void resized () override;
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

	// Determine if the bounds have changed and repaint if necessary
	void updateBoundsAndRepaint ();

	// Tests if this connector represents the given connection object
	bool represents (const Connection& connection);

private:
	void getDistancesFromEnds (int x, int y, double& distanceFromStart, double& distanceFromEnd) const
	{
		float x1, y1, x2, y2;
		getStartAndEndPoints (x1, y1, x2, y2);

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
	int sourceNodeId = 0;
	int sourceChannel = 0;
	int destNodeId = 0;
	int destChannel = 0;

	GraphComponent* graphComponent = nullptr;
	bool isDragging = false;

	JUCE_LEAK_DETECTOR (ConnectionComponent);
};



}

#endif
