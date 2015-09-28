#include "stdafx.h"
#include "CppUnitTest.h"

#include "AudioBufferManager.h"
#include "AudioBuffer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AudioWidgetsTests
{
    TEST_CLASS (TestAudioBuffer)
    {
    public:
        TEST_METHOD (AudioBuffer_Constructor)
        {
            /*AudioBuffer<float> buffer (256, 1);

            Assert::AreEqual (256u, buffer.getSamplesCount (), 
                              L"Sample count not as expected");*/
        }

    private:

    };
}
