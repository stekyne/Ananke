#pragma once

#ifndef GRAPHOP_H_INCLUDED
#define GRAPHOP_H_INCLUDED

#include <cassert>
#include "AudioBufferManager.h"

// Interface for a graph operation
struct GraphOp
{
    virtual ~GraphOp () {};
    virtual void perform () = 0;
    virtual const char* const getName () = 0;
};

class ProcessNodeOp : public GraphOp
{
public:
    ProcessNodeOp (const AudioBufferID audioIn,
                   const AudioBufferID audioOut,
                   const unsigned int numSamples, 
                   NodeModel& node,
                   AudioBufferManager& audioBufferManager)
        : audioIn (audioIn), audioOut (audioOut),
          numSamples (numSamples), node (node),
          audioBufferManager (audioBufferManager)
    {
        assert (numSamples != 0);
    }

    ProcessNodeOp& operator= (const ProcessNodeOp&) = delete;

    virtual void perform ()
    {
        auto inputBuffer = audioBufferManager.getBufferFromID (audioIn);
        auto outputBuffer = audioBufferManager.getBufferFromID (audioOut);
        node.process (inputBuffer == nullptr ? nullptr : inputBuffer.get (), 
                      outputBuffer.get (), numSamples);
    }

    const char* const getName ()
    {
        return node.getName ();
    }

private:
    AudioBufferManager& audioBufferManager;
    NodeModel& node;
    const AudioBufferID audioIn, audioOut;
    const unsigned int numSamples;
};

#endif
