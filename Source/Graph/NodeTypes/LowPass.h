#pragma once

#ifndef LOWPASSNODE_H_INCLUDED
#define LOWPASSNODE_H_INCLUDED

#include "..\Node.h"
#include "..\DSP.h"

namespace Ananke
{

class LowPassNode : public Node
{
public:
    LowPassNode () = delete;
    LowPassNode (int id) :
        Node (id)
    {
        const float cutoff = 2 * 500.f / 44100.f;
        const double res = pow (10, 0.05 * -0.0);
        const double k = 0.5 * res * sin (3.14159265359 * cutoff);
        const double c1 = 0.5 * (1 - k) / (1 + k);
        const double c2 = (0.5 + c1) * cos (3.14159265359 * cutoff);
        const double c3 = (0.5 + c1 - c2) * 0.25;

        lpRight.mA0 = lpLeft.mA0 = 2 * c3;
        lpRight.mA1 = lpLeft.mA1 = 2 * 2 * c3;
        lpRight.mA2 = lpLeft.mA2 = 2 * c3;
        lpRight.mB1 = lpLeft.mB1 = 2 * -c2;
        lpRight.mB2 = lpLeft.mB2 = 2 * c1;
    }

    ~LowPassNode () = default;

    void process (const ReadOnlyBufferArray& audioIn, const BufferArray& audioOut, const int numSamples) override
    {
        Node::process (audioIn, audioOut, numSamples);

        assert (audioIn.size () == getNumInputChannels ());
        assert (audioOut.size () == getNumOutputChannels ());
        assert (audioIn.size () == audioOut.size ());

        for (unsigned int chan = 0; chan < audioIn.size (); ++chan)
        {
            auto& input = *audioIn[chan];
            auto& output = *audioOut[chan];

            FilterState& state = (chan == 0) ? lpLeft : lpRight;

            for (auto i = 1; i < output.getSize (); ++i)
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

    std::string getName () const override
    {
        return "LP Filter";
    }

    int getNumInputChannels () const override
    {
        return 2;
    }

    int getNumOutputChannels () const override
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

}

#endif
