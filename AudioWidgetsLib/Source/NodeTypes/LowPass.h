#pragma once

#ifndef LOWPASSNODE_H_INCLUDED
#define LOWPASSNODE_H_INCLUDED

#include "..\NodeModel.h"

class LowPassNode : public NodeModel
{
public:
    LowPassNode () = default;
    LowPassNode (int id, float positionX, float positionY)
        : NodeModel (id, positionX, positionY)
    {
    }

    ~LowPassNode () = default;

    void process (InputBufArray audioIn,
                  OutputBufArray audioOut,
                  const uint32_t numSamples) override
    {
        NodeModel::process (audioIn, audioOut, numSamples);

        //audioOut[0] = audioIn[0];

        for (unsigned int i = 1; i < numSamples; ++i)
        {
            //audioOut[i] = audioIn[i] + audioIn[i - 1];
        }
    }

    const char* const getName () const override
    {
        return "LowPass";
    }

    unsigned int getNumInputChannels () const override
    {
        return 2;
    }

    unsigned int getNumOutputChannels () const override
    {
        return 2;
    }

    bool acceptsMidi () const override
    {
        return false;
    }

private:
};

#endif
