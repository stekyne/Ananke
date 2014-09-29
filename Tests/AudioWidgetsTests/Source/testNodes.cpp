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

            AudioBuffer<float> audioIn (1);
            audioIn[0] = 1.f;
            AudioBuffer<float> audioOut (1);
            
            gainNode.setGain (0.5);
            gainNode.process (&audioIn, &audioOut, audioIn.getBufferSize ());

            Assert::AreEqual (true, audioIn[0] == 1.f, L"Input node didn't contain expected value");
            Assert::AreEqual (true, audioOut[0] == 0.5f, L"Output node didn't contain expected value");
        }

        TEST_METHOD (LowPass_Test)
        {
            LowPassNode lowPassNode;

            AudioBuffer<float> audioIn (1);
            audioIn[0] = 1.f;
            AudioBuffer<float> audioOut (1);

            lowPassNode.process (&audioIn, &audioOut, audioIn.getBufferSize ());

            Assert::AreEqual (true, audioOut[0] == audioIn[0], L"Output node didn't contain expected value");
        }

        TEST_METHOD (SawOSC_Test)
        {
            SawOSCNode sawOscNode;

            AudioBuffer<float> audioOut (50);

            sawOscNode.process (nullptr, &audioOut, 50);
        }

    private:

    };
}
