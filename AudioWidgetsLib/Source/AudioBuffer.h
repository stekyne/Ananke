#pragma once

#ifndef AUDIOBUFFER_H_INCLUDED
#define AUDIOBUFFER_H_INCLUDED

#include <cassert>

template <typename SampleType>
class AudioBuffer
{
public:
    AudioBuffer () = delete;
    AudioBuffer (const unsigned int numSamples)
        : numSamples (numSamples)
    {
        buffer = new SampleType[numSamples];
        memset (buffer, 0, sizeof (SampleType) * numSamples);
    }

    AudioBuffer (const unsigned int numSamples, const int id)
        : AudioBuffer (numSamples)
    {
        this->id = id;
    }

    AudioBuffer (const SampleType* inputBuffer, int numSamples)
        : numSamples (numSamples)
    {
        deleteOnDestruction = false;
        buffer = const_cast<SampleType*> (inputBuffer);
    }
    
    ~AudioBuffer () 
    {        
        assert (buffer != nullptr);
        if (buffer != nullptr && deleteOnDestruction)
        {
            delete[] buffer;
            buffer = nullptr;
        }
    }

    int getSamplesCount () const
    {
        return numSamples;
    }

    void setID (unsigned int _id)
    {
        this->id = _id;
    }

    int getID () const
    {
        return id;
    }

    SampleType& operator[] (const unsigned int index)
    {
        assert (index >= 0 && index < numSamples);
        return buffer[index];
    }

    const SampleType& operator[] (const unsigned int index) const
    {
        assert (index >= 0 && index < numSamples);
        return buffer[index];
    }

    void setBufferFree ()
    {
        isBufferFree = true;
    }

    bool isFree () const 
    { 
        return isBufferFree; 
    }

    unsigned getBufferSize () const
    {
        return numSamples;
    }

private:
    unsigned int numSamples {0};
    int id {-1};
    SampleType* buffer {nullptr};
    bool isBufferFree {false}, deleteOnDestruction {true};
};

#endif
