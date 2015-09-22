#include "stdafx.h"
#include "CppUnitTest.h"

#include "AudioBuffer.h"
#include "Nodes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AudioWidgetsTests
{
    TEST_CLASS (TestNodes)
    {
    public:
        TEST_METHOD (GainNode_Test)
        {
            GainNode gainNode;

            //AudioBuffer<float> audioIn (1, 1);
            //audioIn.getChannel (0)[0] = 1.f;
            //AudioBuffer<float> audioOut (1, 2);
            //
            //gainNode.setGain (0.5);
            //gainNode.process (audioIn, audioOut, audioIn.getBufferSize ());

            //Assert::AreEqual (true, audioIn.getChannel(0)[0] == 1.f, L"Input node didn't contain expected value");
            //Assert::AreEqual (true, audioOut.getChannel(0)[0] == 0.5f, L"Output node didn't contain expected value");
        }

        TEST_METHOD (LowPass_Test)
        {
            LowPassNode lowPassNode;

            //AudioBuffer<float> audioIn (1, 1);
            //audioIn[0] = 1.f;
            //AudioBuffer<float> audioOut (1, 1);

            //lowPassNode.process (audioIn, audioOut, audioIn.getBufferSize ());

            //Assert::AreEqual (true, audioOut[0] == audioIn[0], L"Output node didn't contain expected value");
        }

        TEST_METHOD (SawOSC_Test)
        {
            //SawOSCNode sawOscNode;
            //sawOscNode.setFrequency (440.f);

            //AudioBuffer<float> audioOut (50, 1);

            //sawOscNode.process (AudioBuffer<float>(), audioOut, 50);
        }

    private:

    };
}
