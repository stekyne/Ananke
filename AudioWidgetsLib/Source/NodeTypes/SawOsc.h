#pragma once

#ifndef SAWOSCNODE_H_INCLUDED
#define SAWOSCNODE_H_INCLUDED

#include "..\NodeModel.h"

class SawOSCNode : public NodeModel
{
public:
    SawOSCNode () = default;
    SawOSCNode (int id, float positionX, float positionY)
        : NodeModel (id, positionX, positionY)
    {
    }

    ~SawOSCNode () = default;

    void process (InputBufArray audioIn,
                  OutputBufArray audioOut,
                  const uint32_t numSamples) override
    {
        NodeModel::process (audioIn, audioOut, numSamples);
        
        assert (audioIn.size () == getNumInputChannels ());
        assert (audioOut.size () == getNumOutputChannels ());

        float sr = 44100.f;
        increm = freq / sr;

        for (auto chan = 0u; chan < audioOut.size (); ++chan)
        {
            auto& output = *audioOut[chan];

            for (auto i = 0u; i < numSamples; ++i)
            {
                phasor += increm;

                if (phasor > 1.f)
                    phasor = -1.f;

                output[i] = phasor;
            }
        }
    }

    const char* const getName () const override
    {
        return "Saw OSC";
    }

    unsigned int getNumInputChannels () const override
    {
        return 0;
    }

    unsigned int getNumOutputChannels () const override
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
    float phasor {0.f}, freq {220.f}, increm {0.f};
};

#endif
