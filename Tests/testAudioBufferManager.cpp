#include "..\src\AudioBufferManager.h"

#include "catch.hpp"

namespace AudioProcessingGraphTests
{
    using namespace APG;

    TEST_CASE ("Audio buffer manager usage")
    {
        const auto BufferSize = 32;
        const auto BufferCount = 3;

        AudioBufferManager audioBufferManager (64);
        REQUIRE (audioBufferManager.getBufferCount () == 0);
        REQUIRE (audioBufferManager.getFreeBufferCount () == 0);

        audioBufferManager.allocateBuffers (BufferCount);
        REQUIRE (audioBufferManager.getBufferCount () == BufferCount);
        REQUIRE (audioBufferManager.getFreeBufferCount () == BufferCount);

        // Get a free buffer from the pool
        auto buffer1 = audioBufferManager.getFreeBuffer (AudioBufferID (1, 0));
        auto buffer2 = audioBufferManager.getFreeBuffer (AudioBufferID (2, 0));
        auto buffer2_1 = audioBufferManager.getFreeBuffer (AudioBufferID (2, 1));
        REQUIRE (buffer1 != nullptr);
        REQUIRE (buffer2 != nullptr);
        REQUIRE (buffer2_1 != nullptr);
        REQUIRE ((buffer1 != buffer2 && buffer1 != buffer2_1));
        REQUIRE (audioBufferManager.getFreeBufferCount () == 0);

        SECTION ("Buffer size change should take effect")
        {
            audioBufferManager.setBufferSize (BufferSize);
            REQUIRE (audioBufferManager.getBufferSize () == BufferSize);

            // Buffer1 should be changed to the new buffer size
            REQUIRE (buffer1->getSize () == BufferSize);
        }

        SECTION ("Get buffer based on node ID associated output")
        {
            auto buffer = 
                audioBufferManager.getBufferFromID (AudioBufferID (1, 0));
            
            // 'buffer1' and 'buffer' should be pointing to same buffer
            REQUIRE (buffer1 == buffer);
        }

        SECTION ("Free a buffer by an ID number")
        {
            const auto freeBufferCount = audioBufferManager.getFreeBufferCount ();
            audioBufferManager.markFreeByID (2);
            REQUIRE (audioBufferManager.getFreeBufferCount () == 2);
        }

        SECTION ("Free all buffers")
        {
            audioBufferManager.freeAllBuffers ();
            REQUIRE (audioBufferManager.getBufferCount () == 
                     audioBufferManager.getFreeBufferCount ());
        }
    }
}
