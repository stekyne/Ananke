#pragma once

#ifndef SAWOSCNODE_H_INCLUDED
#define SAWOSCNODE_H_INCLUDED

#include "..\NodeModel.h"

class SawOSCNode : public NodeModel
{
public:
    SawOSCNode () = default;
    SawOSCNode (int id)
        : NodeModel (id)
    {
    }

    ~SawOSCNode () = default;

    void process (const AudioBuffer<float>* const audioIn,
                  AudioBuffer<float>* const audioOut,
                  const unsigned int numSamples) override
    {
        NodeModel::process (audioIn, audioOut, numSamples);
        assert (audioIn == nullptr);

        for (unsigned int i = 0; i < numSamples; ++i)
        {

        }
    }

private:
    float phasor {0.f}, freq {440.f};
};

#endif
