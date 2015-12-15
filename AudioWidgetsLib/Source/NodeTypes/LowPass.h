#pragma once

#ifndef LOWPASSNODE_H_INCLUDED
#define LOWPASSNODE_H_INCLUDED

#include "..\NodeModel.h"
#include "..\DSP.h"

class LowPassNode : public NodeModel
{
public:
    LowPassNode () = default;
    LowPassNode (int id, float positionX, float positionY) :
        NodeModel (id, positionX, positionY)
    {
        float cutoff = 2 * 500.f / 44100.f;
        double res = pow (10, 0.05 * -0.0);
        double k = 0.5 * res * sin (3.14159265359 * cutoff);
        double c1 = 0.5 * (1 - k) / (1 + k);
        double c2 = (0.5 + c1) * cos (3.14159265359 * cutoff);
        double c3 = (0.5 + c1 - c2) * 0.25;

        lpLeft.mA0 = 2 * c3;
        lpLeft.mA1 = 2 * 2 * c3;
        lpLeft.mA2 = 2 * c3;
        lpLeft.mB1 = 2 * -c2;
        lpLeft.mB2 = 2 * c1;

        lpRight.mA0 = 2 * c3;
        lpRight.mA1 = 2 * 2 * c3;
        lpRight.mA2 = 2 * c3;
        lpRight.mB1 = 2 * -c2;
        lpRight.mB2 = 2 * c1;
    }

    ~LowPassNode () = default;

    void process (InputBufArray audioIn,
                  OutputBufArray audioOut,
                  const uint32_t numSamples) override
    {
        NodeModel::process (audioIn, audioOut, numSamples);

        assert (audioIn.size () == getNumInputChannels ());
        assert (audioOut.size () == getNumOutputChannels ());

        for (unsigned int chan = 0; chan < audioIn.size (); ++chan)
        {
            auto& input  = *audioIn[chan];
            auto& output = *audioOut[chan];

            FilterState& state = (chan == 0) ? lpLeft : lpRight;

            for (unsigned int i = 1; i < numSamples; ++i)
            {
                output[i] = (float)((state.mA0 * input[i]) + (state.mA1 * state.mX1) + 
                                    (state.mA2 * state.mX2) - (state.mB1 * state.mY1) - 
                                    (state.mB2 * state.mY2));

                state.mX2 = state.mX1;
                state.mX1 = input[i];
                state.mY2 = state.mY1;
                state.mY1 = output[i];
            }
        }
    }

    const char* const getName () const override
    {
        return "LP Filter";
    }

    unsigned int getNumInputChannels () const override
    {
        return 2;
    }

    unsigned int getNumOutputChannels () const override
    {
        return 2;
    }

private:
    struct FilterState
    {
        float mX1 {0};
        float mX2 {0};
        float mY1 {0};
        float mY2 {0};
        double mA0 {0};
        double mA1 {0};
        double mA2 {0};
        double mB1 {0};
        double mB2 {0};
    };

    FilterState lpLeft, lpRight;
};

#endif
