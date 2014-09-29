#pragma once

#ifndef AUDIOBUFFERMANAGER_H_INCLUDED
#define AUDIOBUFFERMANAGER_H_INCLUDED

#include <map>
#include <memory>
#include "AudioBuffer.h"

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

template <typename SampleType = float>
class AudioBufferManager
{
public:
    AudioBufferManager () {}
    explicit AudioBufferManager (unsigned int blockSize) : blockSize (blockSize) {}
    ~AudioBufferManager () {}

    // Set the number of samples per block allocated
    void setBlockSize (unsigned int blockSize)
    {
        this->blockSize = blockSize;
        // TODO reallocate blocks
    }

    // Get an unused pre-allocated buffer ID
    const AudioBufferID getFreeBuffer ()
    {
        for (auto& elem : buffers)
        {
            if (elem.second->isFree () == true)
                return elem.first;
        }

        // No free buffers so allocate more
        return createBuffer ();
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
    void associatedBufferWithNode (AudioBufferID buffer, int nodeID)
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
    std::shared_ptr<AudioBuffer<SampleType>> getBufferFromID (AudioBufferID bufferId)
    {
        assert (buffers.size () != 0);
        return buffers[bufferId];
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
    AudioBufferID createBuffer ()
    {
        AudioBufferID newBuffer (++ids);
        buffers[newBuffer] = std::make_shared<AudioBuffer<float>> (blockSize, newBuffer.getID ());
        ++numberBuffers;
        return newBuffer;
    }

    std::map<AudioBufferID, std::shared_ptr<AudioBuffer<SampleType>>> buffers;
    unsigned int ids {0}, blockSize {0};
    unsigned int numberBuffers {0}, numberFreeBuffers {0};
};

#endif
