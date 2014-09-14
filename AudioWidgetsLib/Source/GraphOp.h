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
};

class ProcessNodeOp : public GraphOp
{
public:
    ProcessNodeOp (const AudioBufferManager::AudioBufferID audioIn,
                   const AudioBufferManager::AudioBufferID audioOut,
                   const unsigned int numSamples, NodeModel& node,
                   AudioBufferManager audioBufferManager)
                   : audioIn (audioIn), audioOut (audioOut),
                   numSamples (numSamples), node (node),
                   audioBufferManager (audioBufferManager)
    {
        assert (numSamples != 0);
    }

    virtual void perform ()
    {
        if (audioIn == AudioBufferManager::AudioBufferID::Empty)
        {
            auto& outputBuffer = audioBufferManager.getBufferFromID (audioOut);
            node.process (outputBuffer.get (), numSamples);
        }
        else
        {
            auto& inputBuffer = audioBufferManager.getBufferFromID (audioIn);
            auto& outputBuffer = audioBufferManager.getBufferFromID (audioOut);
            node.process (inputBuffer.get (), outputBuffer.get (), numSamples);
        }
    }

private:
    AudioBufferManager& audioBufferManager;
    NodeModel& node;
    const AudioBufferManager::AudioBufferID audioIn;
    const AudioBufferManager::AudioBufferID audioOut;
    const unsigned int numSamples;
};

#endif