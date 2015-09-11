#pragma once

#ifndef AUDIOBUFFER_H_INCLUDED
#define AUDIOBUFFER_H_INCLUDED

#include <cassert>
#include "DSP.h"

template <typename SampleType = DSP::SampleType>
class AudioBuffer
{
public:
    AudioBuffer () = default;
    AudioBuffer (const AudioBuffer& other) = delete;

    AudioBuffer (const unsigned int numSamples, 
                 const unsigned int numChannels, const int id)
        :   numSamples (numSamples),
            numChannels (numChannels),
            id (id)
    {
        buffer = new SampleType*[numChannels];

        for (unsigned int i = 0; i < numChannels; ++i)
        {
            buffer[i] = new SampleType[numSamples];
        }
    }

    ~AudioBuffer () 
    {        
        if (buffer == nullptr || !deleteOnDestruction) 
            return;

        for (unsigned int i = 0; i < numChannels; ++i)
        {
            delete[] buffer[i];
        }

        delete[] buffer;
    }

    uint32_t getSamplesCount () const
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

    void setBufferFree ()
    {
        isBufferFree = true;
    }

    bool isFree () const 
    { 
        return isBufferFree; 
    }

    uint32_t getBufferSize () const
    {
        return numSamples;
    }

    uint32_t getChannelCount () const
    {
        return numChannels;
    }

    SampleType* const getChannel (const unsigned int channel)
    {
        assert (channel >= 0 && channel < numChannels);
        return buffer[channel];
    }

    const SampleType* const getChannel (const unsigned int channel) const
    {
        assert (channel >= 0 && channel < numChannels);
        return buffer[channel];
    }

    const SampleType* const getChannel (const unsigned int channel) const
    {
        assert (channel >= 0 && channel < numChannels);
        return buffer[channel];
    }

    static AudioBuffer<> Empty;

private:
    uint32_t numSamples {0}, numChannels {0};
    int id {-1};
    SampleType** buffer {nullptr};
    bool isBufferFree {false}, deleteOnDestruction {true};
    uint32_t referenceCount {0};
};

AudioBuffer<float> AudioBuffer<float>::Empty;

#endif
