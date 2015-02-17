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
    Node (std::shared_ptr<GraphModel> graph,
          uint32 id_, String name_,
          int  inputNum, int  outputNum,
          bool hasMidiIn, bool hasMidiOut);
    ~Node ();

    void getPinPos (const int index, const bool isInput, float& x, float& y);

    void paint (Graphics& g);
    void resized ();
    void update ();

    void mouseDown (const MouseEvent &e);
    void mouseDrag (const MouseEvent &e);
    void mouseUp (const MouseEvent &e);

    const uint32 id;
    const String name;

    int numInputs, numOutputs;

private:
    friend class GraphComponent;
    GraphComponent* getGraph () const;

private:
    ComponentDragger dragger;
    std::vector<Pin*> inputs, outputs;
    std::shared_ptr <GraphModel> graph;
    Font font;
    int numIns, numOuts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Node);
};

#endif
