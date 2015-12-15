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

    void process (InputBufArray audioIn,
                  OutputBufArray audioOut,
                  const uint32_t numSamples) override
    {
        NodeModel::process (audioIn, audioOut, numSamples);

        assert (audioIn.size () == getNumInputChannels ());
        assert (audioOut.size () == getNumOutputChannels ());

        for (auto chan = 0u; chan < audioOut.size (); ++chan)
        {
            auto& input  = *audioIn[chan];
            auto& output = *audioOut[chan];

            for (auto i = 0u; i < numSamples; ++i)
            {
                output[i] = input[i] * gain;
            }
        }
    }

    const char* const getName () const override
    {
        return "Gain";
    }

    unsigned int getNumInputChannels () const override
    {
        return 2;
    }

    unsigned int getNumOutputChannels () const override
    {
        return 2;
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
    float gain {0.5f};
};

#endif
