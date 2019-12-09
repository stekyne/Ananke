#pragma once

#ifndef AUDIOBUFFERMANAGER_H_INCLUDED
#define AUDIOBUFFERMANAGER_H_INCLUDED

#include <vector>
#include <memory>
#include "AudioBuffer.h"
#include "DSP.h"

namespace Ananke {

class AudioBufferManager
{
public:
    AudioBufferManager () = delete;
    ~AudioBufferManager () = default;

    explicit AudioBufferManager (int bufferSize) :
        bufferSize (bufferSize)
    {
    }

    // Set the number of samples per block allocated
    void setBufferSize (int bufferSize_)
    {
        // Re-allocate memory if new block size is greater
        for (auto& buffer : buffers)
            buffer->setSize (bufferSize_);

        this->bufferSize = bufferSize_;
    }

    void allocateBuffers (int numberBuffers)
    {
        for (auto i = 0; i < numberBuffers; ++i)
            createBuffer ();
    }

    // Get an unused pre-allocated buffer pointer. Do not delete!
    // Buffer will be assigned an ID to keep track of it
    AudioBuffer<DSP::SampleType>* registerFreeBufferWithID (AudioBufferID newId)
    {
        assert (bufferSize != 0);
        assert (newId != AudioBufferID::Empty);

		// Find a free buffer and use that
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

    // Return the buffer using the registered ID
    AudioBuffer<DSP::SampleType>* getBufferFromID (AudioBufferID bufferId)
    {
        if (bufferId == AudioBufferID::Empty || buffers.size () == 0)
			return nullptr;

        // Check if the key is in the map, return empty buffer if not found
        for (auto& elem : buffers)
            if (elem->getID () == bufferId)
                return elem.get ();

        return nullptr;
    }

    void freeAllBuffers ()
    {
        for (auto& buffer : buffers)
            buffer->setID (AudioBufferID::Empty);
    }

    // Free's all buffers associated with node ID
    void markFreeByID (int id)
    {
        for (const auto& buffer : buffers)
            if (buffer->getID ().getID () == id)
                buffer->setID (AudioBufferID::Empty);
    }

    auto getBufferCount () const { return buffers.size (); }
    auto getBufferSize () const { return bufferSize; }

    auto getFreeBufferCount () const 
    { 
        int numberFreeBuffers = 0;
        
        for (const auto& buffer : buffers)
            if (buffer->isFree ())
                ++numberFreeBuffers;

        return numberFreeBuffers; 
    }

private:
    AudioBuffer<DSP::SampleType>* createBuffer ()
    {
        assert (bufferSize != 0);
        auto buffer = std::make_unique<AudioBuffer<DSP::SampleType>> (bufferSize, AudioBufferID::Empty);
        auto bufferPtr = buffer.get ();
        buffers.push_back (std::move (buffer));
        return bufferPtr;
    }

private:    
    using BufferPtr = std::unique_ptr<AudioBuffer<DSP::SampleType>>;

    std::vector<BufferPtr> buffers;
    int ids = 0;
    int bufferSize = 0;
};

}

#endif
