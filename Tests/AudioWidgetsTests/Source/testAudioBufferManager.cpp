#include "stdafx.h"
#include "CppUnitTest.h"

#include "AudioBufferManager.h"
#include "Nodes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AudioWidgetsTests
{
    TEST_CLASS (TestAudioBufferManager)
    {
    public:
        TEST_METHOD (AudioBufferManager_getFreeBuffer)
        {
            AudioBufferManager audioBufferManager;

            auto bufferEntry = audioBufferManager.getFreeBuffer ();
            Assert::AreEqual (AudioBufferID (1, 0).getID (), bufferEntry->getID ().getID (), L"Buffer ID did not match expected value");
            Assert::AreEqual (1u, audioBufferManager.getBufferCount (), L"Buffer count didn't match expected");

            auto bufferEntry2 = audioBufferManager.getFreeBuffer ();
            Assert::AreEqual (AudioBufferID (2, 0).getID (), bufferEntry2->getID ().getID (), L"Buffer2 ID did not match expected value");
            Assert::AreEqual (2u, audioBufferManager.getBufferCount (), L"Buffer2 count didn't match expected");
        }

        TEST_METHOD (AudioBufferManager_markBufferAsFree)
        {
            AudioBufferManager audioBufferManager;

            auto bufferEntry = audioBufferManager.getFreeBuffer ();
            audioBufferManager.markBufferAsFree (bufferEntry->getID ());
            
            Assert::AreEqual (1u, audioBufferManager.getFreeBufferCount (), L"Free buffer count wasn't as expected");
        }

        TEST_METHOD (AudioBufferManager_getBufferForID)
        {
            AudioBufferManager audioBufferManager;

            auto bufferEntry = audioBufferManager.getFreeBuffer ();
            Assert::AreEqual (true, bufferEntry != nullptr, L"Could not get free buffer");

            auto buffer = audioBufferManager.getBufferFromID (bufferEntry->getID ());
            Assert::AreEqual (AudioBufferID (1, 0).getID (), buffer->getID ().getID (), L"ID did not match expected");
        }

    private:

    };
}
