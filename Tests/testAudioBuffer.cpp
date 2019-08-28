#include "..\src\AudioBuffer.h"

#include "catch.hpp"

namespace AudioProcessingGraphTests
{
    using namespace APG;

    TEST_CASE ("Audio buffer usage")
    {
        const auto BlockSize = 32;
        const auto Id = 1u;
        const auto ChannelNum = 0u;

        auto externalBuffer = new float[BlockSize];
        memset (externalBuffer, 0, sizeof (float) * BlockSize);

        // A buffer 32 samples in size that corresponds to ID: 1, Channel: 0
        AudioBuffer<float> buffer (BlockSize, {Id, ChannelNum});
        
        SECTION ("Buffer size should match constructor")
        {
            REQUIRE (buffer.getSize() == BlockSize);
        }

        SECTION ("ID should match constructor")
        {
            REQUIRE (buffer.getID () == AudioBufferID (Id, ChannelNum));
        }

        SECTION ("Buffer should display freeness correctly")
        {
            AudioBuffer<float> freeBuffer (32, AudioBufferID::Empty);
            REQUIRE (freeBuffer.isFree ());

            freeBuffer.setID (AudioBufferID (2, 1));
            REQUIRE_FALSE (freeBuffer.isFree ());

            freeBuffer.setID (AudioBufferID::Empty);
            REQUIRE (freeBuffer.isFree ());
        }

        SECTION ("Buffer writing and reading indexes should match and be valid")
        {
            AudioBuffer<int> intBuf (10, AudioBufferID (2, 0));
            intBuf[5] = 5;
            REQUIRE (intBuf[5] == 5);

            intBuf[9] = 9;
            REQUIRE (intBuf[9] == 9);
        }

        SECTION ("Increase buffer size")
        {
            const auto newBufferSize = 64;
            buffer.setSize (newBufferSize);
            buffer[newBufferSize - 1] = 0.5f;
            REQUIRE (buffer.getSize () == newBufferSize);
        }

        SECTION ("Decreasing buffer size should not reallocate")
        {
            const auto dataPointer = buffer.getData ();
            buffer.setSize (buffer.getSize () - 1);
            REQUIRE (buffer.getData () == dataPointer);
        }

        SECTION ("Buffer can be supplied with data buffer to use")
        {
            buffer.setBufferToUse (externalBuffer, BlockSize);
            buffer[3] = 1.5f;
            REQUIRE ((externalBuffer[3] > 1.f && 
                      externalBuffer[3] < 2.f));
        }

        SECTION ("Copy data from external buffer to AudioBuffer")
        {
            externalBuffer[5] = 1.5;
            buffer.copyDataFrom (externalBuffer, BlockSize);
            REQUIRE ((buffer[5] > 1.f && 
                      buffer[5] < 2.f));
        }

        SECTION ("Copy data from AudioBuffer to external buffer")
        {
            buffer[5] = 1.5;
            buffer.copyDataTo (externalBuffer, BlockSize);
            REQUIRE ((externalBuffer[5] > 1.f &&
                      externalBuffer[5] < 2.f));
        }

        SECTION ("Setting an ID to a buffer negates it's free-ness")
        {
            AudioBuffer<float> freeBuffer (32, AudioBufferID::Empty);
            REQUIRE (freeBuffer.isFree ());

            freeBuffer.setID (AudioBufferID (2, 1));
            REQUIRE_FALSE (freeBuffer.isFree ());

            freeBuffer.setID (AudioBufferID::Empty);
            REQUIRE (freeBuffer.isFree ());
        }
    }
}