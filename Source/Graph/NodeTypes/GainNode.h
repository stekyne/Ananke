#pragma once

#ifndef GAINNODE_H_INCLUDED
#define GAINNODE_H_INCLUDED

#include "..\Node.h"

namespace APG 
{

class GainNode : public Node
{
public:
    GainNode () = delete;
    GainNode (int id) :
        Node (id)
    {
    }

    ~GainNode () = default;

    void process (const ReadOnlyBufferArray& audioIn, const BufferArray& audioOut, const int numSamples) override
    {
        Node::process (audioIn, audioOut, numSamples);

        assert (audioIn.size () == getNumInputChannels ());
        assert (audioOut.size () == getNumOutputChannels ());
        assert (audioIn.size () == audioOut.size ());

        for (auto chan = 0u; chan < audioOut.size (); ++chan)
        {
            auto& input = *audioIn[chan];
            auto& output = *audioOut[chan];

            for (auto i = 0; i < output.getSize (); ++i)
            {
                output[i] = input[i] * gain;
            }
        }
    }

    std::string getName () const override
    {
        return "Gain";
    }

    int getNumInputChannels () const override
    {
        return 2;
    }

    int getNumOutputChannels () const override
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

}

#endif
