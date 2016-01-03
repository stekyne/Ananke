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

    Pin () = delete;
    Pin (Type type, uint32 filterID, int index, 
         GraphComponent& graph);
    ~Pin ();

    void paint (Graphics& g);

    void mouseDown (const MouseEvent& e);
    void mouseDrag (const MouseEvent& e);
    void mouseUp (const MouseEvent& e);

    const uint32 FilterID;
    const int Index;
    const bool IsInput;

    enum { midi_num = 0x1000 };

private:
    friend class Node;

    int	channelNo;
    Type pin_type;
    GraphComponent& graph;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pin);
};

#endif
