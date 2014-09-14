#pragma once

#ifndef AUDIOBUFFER_H_INCLUDED
#define AUDIOBUFFER_H_INCLUDED

#include <cassert>

template <typename T>
class AudioBuffer
{
public:
    AudioBuffer ()
    {
    }

    AudioBuffer (const unsigned int numSamples)
        : numSamples (numSamples)
    {
        buffer = new T[numSamples];
        memset (buffer, 0, sizeof (T) * numSamples);
    }

    AudioBuffer (const unsigned int numSamples, const int id)
        : AudioBuffer (numSamples)
    {
        this->id = id;
    }

    AudioBuffer (const AudioBuffer& other)
    {

    }
    
    ~AudioBuffer () 
    {        
        assert (buffer != nullptr);
        if (buffer != nullptr)
        {
            delete[] buffer;
            buffer = nullptr;
        }
    }

    int getSamplesCount () const
    {
        return numSamples;
    }

    int getID () const
    {
        return id;
    }

    T& operator[] (const unsigned int index)
    {
        assert (index >= 0 && index < numSamples);
        return buffer[index];
    }

private:
    unsigned int numSamples {0};
    int id {-1};
    T* buffer {nullptr};
};

#endif
