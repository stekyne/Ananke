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

class ProcessNodeOp : public GraphOp
{
public:
    ProcessNodeOp (AudioBuffer<DSP::SampleType>& audioIn,
                   AudioBuffer<DSP::SampleType>& audioOut,
                   NodeModel& node)
        :   audioIn (audioIn), 
            audioOut (audioOut), 
            node (node)
    {
    }

    ProcessNodeOp& operator= (const ProcessNodeOp&) = delete;

    virtual void perform (const int blockSize) override
    {
        node.process (audioIn, audioOut, blockSize);
    }

    const char* const getName () override
    {
        return node.getName ();
    }

private:
    AudioBuffer<DSP::SampleType>& audioIn;
    AudioBuffer<DSP::SampleType>& audioOut;
    NodeModel& node;
};

#endif
