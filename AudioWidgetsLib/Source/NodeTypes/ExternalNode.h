#pragma once

#ifndef EXTERNALNODE_H_INCLUDED
#define EXTERNALNODE_H_INCLUDED

#include <assert.h>
#include "..\NodeModel.h"

class ExternalNode : public NodeModel
{
public:
    enum NodeType
    {
        InputType,
        OutputType
    };

public:
    ExternalNode () = delete;
    ExternalNode (float positionX, float positionY, NodeType _type) : 
        NodeModel (_type == InputType ? InputNodeID : OutputNodeID,
                   positionX, positionY),
        type (_type)        
    {
    }

    ~ExternalNode () = default;

    void process (InputBufArray buffersIn,
                  OutputBufArray buffersOut,
                  const uint32_t _numSamples) override
    {
        assert (buffers != nullptr);
        assert (numSamples == _numSamples);

        if (type == NodeType::InputType)
        {
            assert (buffersOut.size () == numChannels);

            for (auto i = 0u; i < buffersIn.size (); ++i)
            {
                auto* bufIn = buffers[i];
                auto* bufOut = buffersOut[i];
                memcpy (bufOut->getData (), bufIn, sizeof (float) * _numSamples);
            }
        }
        else
        {
            assert (buffersIn.size () == numChannels);

            for (auto i = 0u; i < buffersOut.size (); ++i)
            {
                auto* bufIn = buffersIn[i];
                auto* bufOut = buffers[i];
                memcpy (bufOut, bufIn->getData(), sizeof (float) * _numSamples);
            }
        }
    }

    const char* const getName () const override
    {
        return type == InputType ? "InputNode" : "OutputNode";
    }

    unsigned int getNumInputChannels () const override
    {
        return type == InputType ? 0 : 2;
    }

    unsigned int getNumOutputChannels () const override
    {
        return type == InputType ? 2 : 0;
    }

    void setExternalBuffers (float** const _buffers, 
                             uint32_t _numChannels, uint32_t _numSamples)
    {
        this->buffers = _buffers;
        this->numChannels = _numChannels;
        this->numSamples = _numSamples;
    }

private:
    float** buffers {nullptr};
    uint32_t numChannels {0}, numSamples {0};
    NodeType type;
};

#endif
