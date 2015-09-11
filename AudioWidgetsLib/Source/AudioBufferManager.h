#pragma once

#ifndef AUDIOBUFFERMANAGER_H_INCLUDED
#define AUDIOBUFFERMANAGER_H_INCLUDED

#include <map>
#include <memory>
#include "AudioBuffer.h"
#include "DSP.h"

struct AudioBufferID
{
    AudioBufferID (const int id)
        : id (id)
    {
    }

    int getID () const { return id; }

    friend bool operator< (const AudioBufferID& lhs, const AudioBufferID& rhs)
    {
        return lhs.getID () < rhs.getID ();
    }

    friend bool operator== (const AudioBufferID& lhs, const AudioBufferID& rhs)
    {
        return lhs.id == rhs.id;
    }

    friend bool operator!= (const AudioBufferID& lhs, const AudioBufferID& rhs)
    {
        return !(lhs == rhs);
    }

    const static AudioBufferID Empty;

private:
    int id {-1};
};

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

    // Get an unused pre-allocated buffer ID
    const AudioBufferID getFreeBuffer (int numChannels = 1)
    {
        for (auto& elem : buffers)
        {
            if (elem.second->isFree () == true)
                return elem.first;
        }

        // No free buffers so allocate more
        return createBuffer (numChannels);
    }

    // Mark a currently in use buffer as free to use
    void markBufferAsFree (AudioBufferID buffer)
    {
        for (auto& elem : buffers)
        {
            if (elem.first == buffer)
            {
                elem.second->setBufferFree ();
                ++numberFreeBuffers;
                return;
            }
        }
    }

    // Associate buffer with a particular node
    void associateBufferWithNode (AudioBufferID buffer, int nodeID)
    {
        for (auto& elem : buffers)
        {
            if (elem.first == buffer)
            {
                elem.second->setID (nodeID);
                return;
            }
        }
    }

    // Return a buffer from the presented ID
    AudioBuffer<>& getBufferFromID (AudioBufferID bufferId)
    {
        assert (buffers.size () != 0);
        
        if (bufferId == AudioBufferID::Empty)
            return AudioBuffer<>::Empty;

        // Check if the key is in the map, return empty buffer if not found
        if (buffers.find (bufferId) == buffers.end ())
            return AudioBuffer<>::Empty;

        return *buffers[bufferId].get();
    }

    // Find the buffer that is associated with a particular node ID
    const AudioBufferID getAssociatedBufferForNodeId (const int id)
    {
        for (auto& elem : buffers)
        {
            if (elem.second->getID () == id)
            {
                return elem.first;
            }
        }

        return AudioBufferID::Empty;
    }

    int getBufferCount () const { return numberBuffers; }
    int getFreeBufferCount () const { return numberFreeBuffers; }

private:
    AudioBufferID createBuffer (unsigned int numChannels)
    {
        AudioBufferID newBuffer (++ids);
        buffers[newBuffer] = 
            std::make_unique<AudioBuffer<DSP::SampleType>> (blockSize, numChannels, newBuffer.getID ());
        ++numberBuffers;
        return newBuffer;
    }

private:
    std::map<AudioBufferID, std::unique_ptr<AudioBuffer<DSP::SampleType>>> buffers;
    uint32_t ids {0};
    uint32_t blockSize {50};
    uint32_t numberBuffers {0};
    uint32_t numberFreeBuffers {0};
};

#endif
