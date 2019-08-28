#pragma once

#include "..\..\src\Nodes.h"

using namespace APG;

struct InputOnlyNode : public Node
{
    InputOnlyNode (uint32_t node) : Node (node) {}
    ~InputOnlyNode () {}

    uint32_t getNumInputChannels () const override
    {
        return 2;
    }

    uint32_t getNumOutputChannels () const override
    {
        return 0;
    }

    bool acceptsMidi () const override
    {
        return false;
    }

    bool producesMidi () const override
    {
        return false;
    }
};

struct OutputOnlyNode : public Node
{
    OutputOnlyNode (uint32_t node) : Node (node) {}
    ~OutputOnlyNode () {}

    uint32_t getNumInputChannels () const override
    {
        return 0;
    }

    uint32_t getNumOutputChannels () const override
    {
        return 2;
    }

    bool acceptsMidi () const override
    {
        return false;
    }

    bool producesMidi () const override
    {
        return false;
    }
};

struct InputOutputNode : public Node
{
    InputOutputNode (uint32_t node) : Node (node) {}
    ~InputOutputNode () {}

    uint32_t getNumInputChannels () const override
    {
        return 2;
    }

    uint32_t getNumOutputChannels () const override
    {
        return 2;
    }

    bool acceptsMidi () const override
    {
        return false;
    }

    bool producesMidi () const override
    {
        return false;
    }
};
