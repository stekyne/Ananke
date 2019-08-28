#pragma once

#ifndef AUDIOBUFFERMANAGER_H_INCLUDED
#define AUDIOBUFFERMANAGER_H_INCLUDED

#include <vector>
#include <memory>
#include "AudioBuffer.h"
#include "DSP.h"

namespace APG {

class AudioBufferManager
{
public:
    AudioBufferManager () = delete;
    ~AudioBufferManager () = default;

    explicit AudioBufferManager (uint32_t bufferSize) :
        bufferSize (bufferSize)
    {
    }

    // Set the number of samples per block allocated
    void setBufferSize (uint32_t _bufferSize)
    {
        // Re-allocate memory if new block size is greater
        for (auto& buffer : buffers)
        {
            buffer->setSize (_bufferSize);
        }

        this->bufferSize = _bufferSize;
    }

    void allocateBuffers (uint32_t numberBuffers)
    {
        for (auto i = 0u; i < numberBuffers; ++i)
        {
            createBuffer ();
        }
    }

    // Get an unused pre-allocated buffer pointer. Do not delete!
    // Buffer will be assigned an ID to keep track of it
    auto getFreeBuffer (AudioBufferID newId)
    {
        assert (bufferSize != 0);
        assert (newId != AudioBufferID::Empty);

        for (auto& elem : buffers)
        {
            if (elem->isFree () == true)
            {
                auto newBuffer = elem.get ();
                newBuffer->setID (newId);
                return newBuffer;
            }
        }

        // No free buffers so allocate more
        auto newBuffer = createBuffer ();
        newBuffer->setID (newId);
        return newBuffer;
    }

    // Return a buffer from the presented ID
    auto getBufferFromID (AudioBufferID bufferId)
    {
        if (bufferId == AudioBufferID::Empty ||
            buffers.size () == 0)
        {
            return &AudioBuffer<>::Empty;
        }

        // Check if the key is in the map, return empty buffer if not found
        for (auto& elem : buffers)
        {
            if (elem->getID () == bufferId)
                return elem.get ();
        }

        return &AudioBuffer<DSP::SampleType>::Empty;
    }

    void freeAllBuffers ()
    {
        for (auto& buffer : buffers)
        {
            // This will also mark the buffer as free
            buffer->setID (AudioBufferID::Empty);
        }
    }

    // Free's all buffers associated with node ID
    void markFreeByID (uint32_t id)
    {
        for (const auto& buffer : buffers)
        {
            if (buffer->getID ().getID () == id)
                buffer->setID (AudioBufferID::Empty);
        }
    }

    auto getBufferCount () const { return buffers.size (); }
    auto getBufferSize () const { return bufferSize; }

    auto getFreeBufferCount () const 
    { 
        uint32_t numberFreeBuffers = 0;
        
        for (const auto& buffer : buffers)
        {
            if (buffer->isFree ())
                ++numberFreeBuffers;
        }

        return numberFreeBuffers; 
    }

private:
    AudioBuffer<DSP::SampleType>* createBuffer ()
    {
        assert (bufferSize != 0);

        auto buffer = 
            std::make_unique<AudioBuffer<DSP::SampleType>> (bufferSize, AudioBufferID::Empty);
        
        auto bufferPtr = buffer.get ();
        buffers.push_back (std::move (buffer));
        return bufferPtr;
    }

private:    
    using BufferPtr = std::unique_ptr<AudioBuffer<DSP::SampleType>>;

    std::vector<BufferPtr> buffers;
    uint32_t ids {0};
    uint32_t bufferSize {0};
};

}

#endif
