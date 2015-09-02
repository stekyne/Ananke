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

    void process (const AudioBuffer<float>& audioIn,
                  AudioBuffer<float>& audioOut,
                  const unsigned int numSamples) override
    {
        NodeModel::process (audioIn, audioOut, numSamples);

        assert (audioIn.getBufferSize () > 0 &&
                audioIn.getBufferSize () >= numSamples);

        //audioOut[0] = audioIn[0];

        for (unsigned int i = 1; i < numSamples; ++i)
        {
            //audioOut[i] = audioIn[i] + audioIn[i - 1];
        }
    }

    const char* const getName () const
    {
        return "LowPass";
    }

    unsigned int getNumInputChannels () const
    {
        return 2;
    }

    unsigned int getNumOutputChannels () const
    {
        return 2;
    }

    bool acceptsMidi () const
    {
        return false;
    }

private:
};

#endif
