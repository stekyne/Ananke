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
            AudioBufferManager audioBufferManager (50);

            auto& bufferEntry = audioBufferManager.getFreeBuffer ();
            Assert::AreEqual (1, bufferEntry.getID (), L"Buffer ID did not match expected value");
            Assert::AreEqual (1, audioBufferManager.getBufferCount (), L"Buffer count didn't match expected");

            auto& bufferEntry2 = audioBufferManager.getFreeBuffer ();
            Assert::AreEqual (2, bufferEntry2.getID (), L"Buffer2 ID did not match expected value");
            Assert::AreEqual (2, audioBufferManager.getBufferCount (), L"Buffer2 count didn't match expected");
        }

        TEST_METHOD (AudioBufferManager_markBufferAsFree)
        {
            AudioBufferManager audioBufferManager (50);

            auto& bufferEntry = audioBufferManager.getFreeBuffer ();
            audioBufferManager.markBufferAsFree (bufferEntry);
            
            Assert::AreEqual (1, audioBufferManager.getFreeBufferCount (), L"Free buffer count wasn't as expected");
        }

        TEST_METHOD (AudioBufferManager_getBufferForID)
        {
            AudioBufferManager audioBufferManager (50);

            auto& bufferEntry = audioBufferManager.getFreeBuffer ();
            auto& buffer = audioBufferManager.getBufferFromID (bufferEntry);

            Assert::AreEqual (1, buffer.getID (), L"ID did not match expected");
        }

        TEST_METHOD (AudioBufferManager_associatedBuffers)
        {
            AudioBufferManager audioBufferManager (50);
            
            GainNode node (1, 0.0, 0.0);

            const auto& freeBuffer = audioBufferManager.getFreeBuffer ();
            audioBufferManager.associateBufferWithNode (freeBuffer, node.getID ());
            const auto& buffer = audioBufferManager.getAssociatedBufferForNodeId (node.getID ());
            
            Assert::AreEqual (1, buffer.getID (), L"Node ID was not as expected");
        }

    private:

    };
}
