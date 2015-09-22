#pragma once

#ifndef GRAPHOP_H_INCLUDED
#define GRAPHOP_H_INCLUDED

#include <cassert>
#include "AudioBuffer.h"

// Interface for a graph operation
struct GraphOp
{
    virtual ~GraphOp () {};
    virtual void perform (const uint32_t blockSize) = 0;
    virtual const char* const getName () = 0;
};

class FilterNodeOp : public GraphOp
{
public:
    FilterNodeOp (InputBufArray& audioIn,
                  OutputBufArray& audioOut,
                  NodeModel& node) :
        audioIn (audioIn), 
        audioOut (audioOut), 
        node (node)
    {
    }

    FilterNodeOp& operator= (const FilterNodeOp&) = delete;

    virtual void perform (const uint32_t blockSize) override
    {
        node.process (audioIn, audioOut, blockSize);
    }

    const char* const getName () override
    {
        return node.getName ();
    }

private:
    InputBufArray& audioIn;
    OutputBufArray& audioOut;
    NodeModel& node;
};

class GeneratorNode : public GraphOp
{
public:
    GeneratorNode (OutputBufArray& audioOut, NodeModel& node) :
        audioOut (audioOut),
        node (node)
    {
    }

    GeneratorNode& operator= (const GeneratorNode&) = delete;

    virtual void perform (const uint32_t blockSize) override
    {
        node.process (InputBufArray(), audioOut, blockSize);
    }

    const char* const getName () override
    {
        return node.getName ();
    }

private:
    OutputBufArray& audioOut;
    NodeModel& node;
};

#endif
