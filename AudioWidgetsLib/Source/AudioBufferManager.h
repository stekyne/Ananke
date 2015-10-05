#pragma once

#ifndef AUDIOBUFFERMANAGER_H_INCLUDED
#define AUDIOBUFFERMANAGER_H_INCLUDED

#include <vector>
#include <memory>
#include "AudioBuffer.h"
#include "DSP.h"

// outlive the GraphModel and therefore never be point to invalid memory?
class AudioBufferManager
{
public:
    AudioBufferManager () = default;
    explicit AudioBufferManager (unsigned int blockSize) : 
        blockSize (blockSize) {}
    ~AudioBufferManager () = default;

    // Set the number of samples per block allocated
    void setBlockSize (unsigned int _blockSize)
    {
        this->blockSize = _blockSize;
        // TODO reallocate blocks
    }

    // Get an unused pre-allocated buffer pointer. Do not delete!
    AudioBuffer<DSP::SampleType>* getFreeBuffer ()
    {
        for (auto& elem : buffers)
        {
            if (elem->isFree () == true)
                return elem.get ();
        }

        // No free buffers so allocate more
        return createBuffer ();
    }

    // Mark a currently in use buffer as free to use
    void markBufferAsFree (AudioBufferID buffer)
    {
        for (auto& elem : buffers)
        {
            if (elem->getID () == buffer)
            {
                elem->setBufferFree ();
                ++numberFreeBuffers;
                // TODO store this free buffer for next use?
                return;
            }
        }
    }

    // Return a buffer from the presented ID
    AudioBuffer<DSP::SampleType>* getBufferFromID (AudioBufferID bufferId)
    {
        assert (buffers.size () != 0);
        
        if (bufferId == AudioBufferID::Empty)
            return &AudioBuffer<>::Empty;

        // Check if the key is in the map, return empty buffer if not found
        for (auto& elem : buffers)
        {
            if (elem->getID () == bufferId)
                return elem.get ();
        }

        return &AudioBuffer<DSP::SampleType>::Empty;
    }

    // Find the buffer that is associated with a particular node ID
    AudioBuffer<DSP::SampleType>* getAssociatedBufferForNodeOutput (const uint32_t id, const uint32_t channel)
    {
        for (auto& elem : buffers)
        {
            if (elem->getID () == AudioBufferID (id, channel))
            {
                return elem.get ();
            }
        }

        return &AudioBuffer<DSP::SampleType>::Empty;
    }

    void resetAllBuffers ()
    {
        // TODO mark all as free
    }

    auto getBufferCount () const { return numberBuffers; }
    auto getFreeBufferCount () const { return numberFreeBuffers; }

private:
    AudioBuffer<DSP::SampleType>* createBuffer ()
    {
        auto buffer = 
            std::make_unique<AudioBuffer<DSP::SampleType>> (blockSize, AudioBufferID(++ids, 0));
        auto bufferPtr = buffer.get ();
        buffers.push_back (std::move (buffer));
        ++numberBuffers;
        return bufferPtr;
    }

private:
    std::vector<std::unique_ptr<AudioBuffer<DSP::SampleType>>> buffers;
    uint32_t ids {0};
    uint32_t blockSize {50};
    uint32_t numberBuffers {0};
    uint32_t numberFreeBuffers {0};
};

#endif
