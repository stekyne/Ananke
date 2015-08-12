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

// TODO should we return shared pointers or normal pointers seeing as this class will 
// outlive the GraphModel and therefore never be point to invalid memory?
class AudioBufferManager
{
public:
    AudioBufferManager () = default;
    explicit AudioBufferManager (unsigned int blockSize) : blockSize (blockSize) {}
    ~AudioBufferManager () = default;

    // Set the number of samples per block allocated
    void setBlockSize (unsigned int _blockSize)
    {
        this->blockSize = _blockSize;
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
    std::shared_ptr<AudioBuffer<DSP::SampleType>> getBufferFromID (AudioBufferID bufferId)
    {
        assert (buffers.size () != 0);
        
        if (bufferId == AudioBufferID::Empty)
            return nullptr;

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
        buffers[newBuffer] = 
            std::make_shared<AudioBuffer<DSP::SampleType>> (blockSize, newBuffer.getID ());
        ++numberBuffers;
        return newBuffer;
    }

    std::map<AudioBufferID, std::shared_ptr<AudioBuffer<DSP::SampleType>>> buffers;
    unsigned int ids {0}, blockSize {0};
    unsigned int numberBuffers {0}, numberFreeBuffers {0};
};

#endif
