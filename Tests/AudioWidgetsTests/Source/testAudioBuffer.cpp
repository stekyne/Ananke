#include "stdafx.h"
#include "CppUnitTest.h"

#include "GraphModel.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AudioWidgetsTests
{
    TEST_CLASS (TestAudioBuffer)
    {
    public:
        TEST_METHOD (AudioBuffer_Constructor)
        {
            AudioBuffer<float> buffer (256);

            Assert::AreEqual (256, buffer.getSamplesCount (), 
                              L"Sample count not as expected");
        }

        TEST_METHOD (AudioBuffer_IndexOperator)
        {
            AudioBuffer<int> buffer (256);

            for (int i = 0; i < buffer.getSamplesCount (); i++)
            {
                buffer[i] = i;
            }

            Assert::AreEqual (true, (buffer[4] == 4 && buffer[250] == 250),
                              L"Buffer contained values that were not expected");
        }

    private:

    };
}
