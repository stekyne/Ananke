#include "stdafx.h"
#include "CppUnitTest.h"

#include "AudioBufferManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AudioWidgetsTests
{
    TEST_CLASS (TestAudioBufferManager)
    {
    public:
        TEST_METHOD (AudioBufferManager_getFreeBuffer)
        {
            AudioBufferManager<float> audioBufferManager (50);

            auto& bufferEntry = audioBufferManager.getFreeBuffer ();
            Assert::AreEqual (1, bufferEntry.getID (), L"Buffer ID did not match expected value");
            Assert::AreEqual (1, audioBufferManager.getBufferCount (), L"Buffer count didn't match expected");

            auto& bufferEntry2 = audioBufferManager.getFreeBuffer ();
            Assert::AreEqual (2, bufferEntry2.getID (), L"Buffer2 ID did not match expected value");
            Assert::AreEqual (2, audioBufferManager.getBufferCount (), L"Buffer2 count didn't match expected");
        }

        TEST_METHOD (AudioBufferManager_markBufferAsFree)
        {
            AudioBufferManager<float> audioBufferManager (50);

            auto& bufferEntry = audioBufferManager.getFreeBuffer ();
            audioBufferManager.markBufferAsFree (bufferEntry);
            
            Assert::AreEqual (1, audioBufferManager.getFreeBufferCount (), L"Free buffer count wasn't as expected");
        }

        TEST_METHOD (AudioBufferManager_getBufferForID)
        {
            AudioBufferManager<float> audioBufferManager (50);

            auto& bufferEntry = audioBufferManager.getFreeBuffer ();
            auto buffer = audioBufferManager.getBufferFromID (bufferEntry);

            Assert::AreEqual (1, buffer->getID (), L"ID did not match expected");
        }

    private:

    };
}
