#pragma once

#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"

class GraphModel;
class GraphComponent;
class Pin;

class Node : public Component
{
public:
    Node () = delete;
    Node (std::shared_ptr<GraphModel> graph, uint32 id_);
    ~Node ();

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
    GraphComponent* getGraph () const;

private:
    ComponentDragger dragger;
    std::vector<Pin*> inputs, outputs;
    Pin *midiIn {nullptr}, *midiOut {nullptr};
    std::shared_ptr<GraphModel> graph;
    Font font;
    int numIns {0}, numOuts {0}, numInputs {0}, numOutputs {0};
    bool acceptsMidi {false}, producesMidi {false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Node);
};

#endif
