#pragma once

#ifndef GAINNODE_H_INCLUDED
#define GAINNODE_H_INCLUDED

#include "..\NodeModel.h"

class GainNode : public NodeModel
{
public:
    GainNode () = default;
    GainNode (int id)
        : NodeModel (id)
    {
    }

    ~GainNode () = default;

    void process (const AudioBuffer<float>* const audioIn,
                  AudioBuffer<float>* const audioOut,
                  const unsigned int numSamples) override
    {
        NodeModel::process (audioIn, audioOut, numSamples);
        assert (audioIn != nullptr);
        assert (audioIn->getBufferSize () > 0 && 
                audioIn->getBufferSize () <= numSamples);

        const auto& inBuf = *audioIn;
        auto& outBuf = *audioOut;

        for (unsigned int i = 0; i < numSamples; ++i)
        {
            outBuf[i] = inBuf[i] * gain;
        }
    }

    void setGain (float gain)
    {
        this->gain = gain;
    }

    float getGain () const
    {
        return gain;
    }

private:
    float gain {1.f};
};

#endif