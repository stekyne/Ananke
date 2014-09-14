#pragma once

#ifndef AUDIOBUFFERMANAGER_H_INCLUDED
#define AUDIOBUFFERMANAGER_H_INCLUDED

#include <map>
#include <memory>
#include "AudioBuffer.h"

class AudioBufferManager
{
public:
    struct AudioBufferID
    {
        bool isFree {false};

        AudioBufferID (const int id, const bool isFree)
            : id (id), isFree (isFree)
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

public:
    AudioBufferManager ();
    explicit AudioBufferManager (unsigned int blockSize);
    ~AudioBufferManager ();

    void setBlockSize (unsigned int blockSize);
    const AudioBufferID getFreeBuffer ();
    void markBufferAsFree (AudioBufferID buffer);
    std::shared_ptr<AudioBuffer<float>> getBufferFromID (AudioBufferID bufferId);

    int getBufferCount () const { return numberBuffers; }
    int getFreeBufferCount () const { return numberFreeBuffers; }

private:
    AudioBufferID createBuffer ();
    std::map<AudioBufferID, std::shared_ptr<AudioBuffer<float>>> buffers;
    unsigned int ids {0}, blockSize {0};
    unsigned int numberBuffers {0}, numberFreeBuffers {0};
};

#endif
