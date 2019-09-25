#pragma once

#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <memory>
#include "../Source/AudioProcessingGraph.h"
#include "../JuceLibraryCode/JuceHeader.h"

class Graph;
class GraphComponent;
class Pin;

class NodeComponent : public Component
{
public:
    NodeComponent () = delete;
    NodeComponent (Ananke::Graph& graph, int id);
    ~NodeComponent ();

    void getPinPos (const int index, const bool isInput, float& x, float& y);

    void paint (Graphics& g);
    void resized ();
    void update ();

    void mouseDown (const MouseEvent &e);
    void mouseDrag (const MouseEvent &e);
    void mouseUp (const MouseEvent &e);

    const uint32 id;

private:
    friend class GraphComponent;
    GraphComponent* NodeComponent::getGraph () const;
    mutable GraphComponent* graphComp {nullptr};

private:
    ComponentDragger dragger;
    std::vector<Pin*> inputs, outputs;
    Font font;
    Pin* midiIn {nullptr};
    Pin* midiOut {nullptr};
    Ananke::Graph& graph;
    int numIns {0}, numOuts {0}, numInputs {0}, numOutputs {0};
    bool acceptsMidi {false}, producesMidi {false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeComponent);
};

#endif
