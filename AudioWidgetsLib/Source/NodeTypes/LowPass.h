#pragma once

#ifndef LOWPASSNODE_H_INCLUDED
#define LOWPASSNODE_H_INCLUDED

#include "..\NodeModel.h"

class LowPassNode : public NodeModel
{
public:
    LowPassNode () = default;
    LowPassNode (int id)
        : NodeModel (id)
    {
    }

    ~LowPassNode () = default;

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

        outBuf[0] = inBuf[0];

        for (unsigned int i = 1; i < numSamples; ++i)
        {
            outBuf[i] = inBuf[i] + inBuf[i - 1];
        }
    }

private:
};

#endif
