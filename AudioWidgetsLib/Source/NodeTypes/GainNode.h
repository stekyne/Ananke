#pragma once

#ifndef GAINNODE_H_INCLUDED
#define GAINNODE_H_INCLUDED

#include "..\NodeModel.h"

class GainNode : public NodeModel
{
public:
    GainNode () = default;
    GainNode (int id, float positionX, float positionY)
        : NodeModel (id, positionX, positionY)
    {
    }

    ~GainNode () = default;

    void process (const AudioBuffer<float>& audioIn,
                  AudioBuffer<float>& audioOut,
                  const unsigned int numSamples) override
    {
        NodeModel::process (audioIn, audioOut, numSamples);

        assert (audioIn.getBufferSize () > 0 && 
                audioIn.getBufferSize () >= numSamples);

        for (unsigned int i = 0; i < numSamples; ++i)
        {
            //audioOut[i] = audioIn[i] * gain;
        }
    }

    const char* const getName () const override
    {
        return "GainNode";
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

    void setGain (float _gain)
    {
        this->gain = _gain;
    }

    float getGain () const
    {
        return gain;
    }

private:
    float gain {1.f};
};

#endif
