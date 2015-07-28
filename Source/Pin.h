#pragma once

#ifndef PIN_H_INCLUDED
#define PIN_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class GraphComponent;

class Pin : public Component
{
public:
    enum Type
    {
        AudioInput = 0,
        AudioOutput,
        MidiInput,
        MidiOutput,

        NumberOfTypes
    };

    //=====================================================================
    Pin () = delete;
    explicit Pin (Type type, uint32 filterID, int index);
    ~Pin ();

    void paint (Graphics& g);

    /** Call begin dragging in graph */
    void mouseDown (const MouseEvent& e);

    /** Call connector dragging in graph */
    void mouseDrag (const MouseEvent& e);

    /** Call stop dragging in graph */
    void mouseUp (const MouseEvent& e);

    const uint32 filterID;
    const int index;
    const bool isInput;

    static enum { midi_num = 0x1000 };

private:
    GraphComponent* getGraph () const
    {
        GraphComponent* graphComponent =
            findParentComponentOfClass<GraphComponent> ();

        if (graphComponent == nullptr)
        {
            DBG ("Could not find graph component for pin");
        }

        return graphComponent;
    }

private:
    friend class Node;

    int	channelNo;
    Type pin_type;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pin);
};

#endif
