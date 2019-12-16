#pragma once

#ifndef SAWOSCNODE_H_INCLUDED
#define SAWOSCNODE_H_INCLUDED

#include "..\Node.h"

namespace Ananke
{

class SawOSCNode : public Node
{
public:
    SawOSCNode () = delete;
    SawOSCNode (int id) :
        Node (id)
    {
    }

    ~SawOSCNode () = default;

    void process (const BufferArray& audioIn, const BufferArray& audioOut, const int numSamples) override
    {
        Node::process (audioIn, audioOut, numSamples);

        assert (audioIn.size () == getNumInputChannels ());
        assert (audioOut.size () == getNumOutputChannels ());

        const float sr = 44100.f;
        increm = 2 * juce::float_Pi * freq / sr;

        auto& leftChan = *audioOut[0];
        auto& rightChan = *audioOut[1];

        for (auto i = 0; i < numSamples; ++i)
        {
            leftChan[i] = rightChan[i] = sinf (phasor);
            phasor += increm;
        }
    }

    std::string getName () const override
    {
        return "Saw OSC";
    }

    int getNumInputChannels () const override
    {
        return 0;
    }

    int getNumOutputChannels () const override
    {
        return 2;
    }

    void setFrequency (float _freq)
    {
        this->freq = _freq;
    }

    float getFrequency () const
    {
        return freq;
    }

private:
    float phasor = 0.f;
    float freq = 220.f;
    float increm = 0.f;
};

}

#endif
