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
        NodeModel (type == InputType ? InputNodeID : OutputNodeID, positionX, positionY),
        type (_type)        
    {
    }

    ~ExternalNode () = default;

    void process (const AudioBuffer<float>& audioIn,
                  AudioBuffer<float>& audioOut,
                  const unsigned int _numSamples) override
    {
        assert (buffers != nullptr);
        assert (numSamples == _numSamples);

        if (type == NodeType::InputType)
        {
            assert (audioIn.getChannelCount () == numChannels);
            assert (audioIn.getSamplesCount () == _numSamples);

            for (auto i = 0u; i < audioIn.getChannelCount (); ++i)
            {
                auto* bufIn = buffers[i];
                auto* bufOut = audioOut.getChannel (i);
                memcpy (bufOut, bufIn, sizeof (float) * _numSamples);
            }
        }
        else
        {
            assert (audioOut.getChannelCount () == numChannels);
            assert (audioOut.getSamplesCount () == _numSamples);

            for (auto i = 0u; i < audioOut.getChannelCount (); ++i)
            {
                auto* bufIn = audioIn.getChannel (i);
                auto* bufOut = buffers[i];
                memcpy (bufOut, bufIn, sizeof (float) * _numSamples);
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
    uint32_t numChannels, numSamples;
    NodeType type;
};

#endif
