#pragma once

#ifndef CONNECTOR_H_INCLUDED
#define CONNECTOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

namespace Ananke {

class GraphComponent;
class Graph;

class Connector : public Component
{
public:
	Connector () = default;
	Connector (Graph* graph, GraphComponent* graphComponent);
	Connector (Graph* graph, GraphComponent* graphComponent, 
		int srcNodeComponent, int srcChannel, int dstNodeComponent, int dstChannel);
	~Connector () = default;

	void setSourceNodeID (int id) { sourceFilterID = id; }
	int getSourceNodeID () const noexcept { return sourceFilterID; }

	void setSourceChannel (int index) { sourceFilterChannel = index; }
	int getSourceChannel () const noexcept { return sourceFilterChannel; }

	void setDestNodeID (int id) { destFilterID = id; }
	int getDestNodeID () const noexcept { return destFilterID; }

	void setDestChannel (int index) { destFilterChannel = index; }
	int getDestChannel () const noexcept { return destFilterChannel; }

	void setInput (const int sourceFilterID, const int sourceFilterChannel);
	void setOutput (const int destFilterID, const int destFilterChannel);

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

	// Drag one end of the connector and update its position in the filter
	void mouseDrag (const MouseEvent& e);

	// If dragging previously, check if the position is near a pin, if not,
	// disconnect this connector (delete it). if it is near, check if it can connect
	// and act accordingly
	void mouseUp (const MouseEvent& e);

	void update ();

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
	int sourceFilterID = 0;
	int destFilterID = 0;
	int sourceFilterChannel = 0;
	int destFilterChannel = 0;

	Graph* graph = nullptr;
	GraphComponent* graphComponent = nullptr;
	bool dragging = false;

	JUCE_LEAK_DETECTOR (Connector);
};

}

#endif
