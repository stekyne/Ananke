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
        increm = freq / sr;

        for (auto chan = 0u; chan < audioOut.size (); ++chan)
        {
            auto& output = *audioOut[chan];

            for (auto i = 0; i < output.getSize (); ++i)
            {
                phasor += increm;

                if (phasor > 1.f)
                    phasor = -1.f;

                output[i] = phasor;
            }
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
    float phasor {0.f};
    float freq {220.f};
    float increm {0.f};
};

}

#endif
