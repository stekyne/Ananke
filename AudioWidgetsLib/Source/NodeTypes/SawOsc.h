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

    void process (const AudioBuffer<float>* const audioIn,
                  AudioBuffer<float>* const audioOut,
                  const unsigned int numSamples) override
    {
        NodeModel::process (audioIn, audioOut, numSamples);
        assert (audioIn == nullptr);

        auto& outBuf = *audioOut;

        float sr = 44100.f;
        increm = freq / sr;

        for (unsigned int i = 0; i < numSamples; ++i)
        {
            phasor += increm;

            if (phasor > 1.f)
                phasor = -1.f;

            outBuf[i] = phasor;
        }
    }

    const char* const getName () const
    {
        return "SawNode";
    }

    unsigned int getNumInputChannels () const
    {
        return 0;
    }

    unsigned int getNumOutputChannels () const
    {
        return 2;
    }

    bool acceptsMidi () const
    {
        return true;
    }

    void setFrequency (float freq)
    {
        this->freq = freq;
    }

    float getFrequency () const
    {
        return freq;
    }

private:
    float phasor {0.f}, freq {440.f}, increm {0.f};
};

#endif
