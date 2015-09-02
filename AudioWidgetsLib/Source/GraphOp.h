#pragma once

#ifndef GRAPHOP_H_INCLUDED
#define GRAPHOP_H_INCLUDED

#include <cassert>
#include "AudioBuffer.h"

// Interface for a graph operation
struct GraphOp
{
    virtual ~GraphOp () {};
    virtual void perform (const int blockSize) = 0;
    virtual const char* const getName () = 0;
};

class FilterNodeOp : public GraphOp
{
public:
    FilterNodeOp (const AudioBuffer<DSP::SampleType>& audioIn,
                  AudioBuffer<DSP::SampleType>& audioOut,
                  NodeModel& node)
        :   audioIn (audioIn), 
            audioOut (audioOut), 
            node (node)
    {
    }

    FilterNodeOp& operator= (const FilterNodeOp&) = delete;

    virtual void perform (const int blockSize) override
    {
        node.process (audioIn, audioOut, blockSize);
    }

    const char* const getName () override
    {
        return node.getName ();
    }

private:
    const AudioBuffer<DSP::SampleType>& audioIn;
    AudioBuffer<DSP::SampleType>& audioOut;
    NodeModel& node;
};

class GeneratorNode : public GraphOp
{
public:
    GeneratorNode (AudioBuffer<>& audioOut, NodeModel& node)
        :   audioOut (audioOut),
            node (node)
    {
    }

    GeneratorNode& operator= (const GeneratorNode&) = delete;

    virtual void perform (const int blockSize) override
    {
        node.process (audioOut, blockSize);
    }

    const char* const getName () override
    {
        return node.getName ();
    }

private:
    AudioBuffer<>& audioOut;
    NodeModel& node;
};

#endif
