#pragma once

#ifndef AUDIOBUFFER_H_INCLUDED
#define AUDIOBUFFER_H_INCLUDED

#include <cassert>
#include <vector>
#include "DSP.h"

struct AudioBufferID
{
    AudioBufferID (const uint32_t id, const uint32_t channelNumber) :
        id (id),
        channelNumber (channelNumber)
    {
    }

    uint32_t getID () const { return id; }
    uint32_t getChanelNumber () const { return channelNumber; }

    friend bool operator< (const AudioBufferID& lhs, const AudioBufferID& rhs)
    {
        return lhs.getID () < rhs.getID () &&
            lhs.getChanelNumber () < rhs.getChanelNumber ();
    }

    friend bool operator== (const AudioBufferID& lhs, const AudioBufferID& rhs)
    {
        return lhs.id == rhs.id &&
            lhs.channelNumber == rhs.channelNumber;
    }

    friend bool operator!= (const AudioBufferID& lhs, const AudioBufferID& rhs)
    {
        return !(lhs == rhs);
    }

    const static AudioBufferID Empty;

private:
    uint32_t id {0}, channelNumber {0};
};

template <typename SampleType = DSP::SampleType>
class AudioBuffer
{
public:
    AudioBuffer () = default;
    AudioBuffer (const AudioBuffer& other) = default;
    AudioBuffer (AudioBuffer&& other) = default;
    AudioBuffer& operator= (const AudioBuffer& other) = default;
    AudioBuffer& operator= (AudioBuffer&& other) = default;

    AudioBuffer (const uint32_t numSamples, AudioBufferID id) :
        numSamples (numSamples),
        id (id)
    {
        buffer = new SampleType[numSamples];
    }

    ~AudioBuffer () 
    {        
        if (buffer == nullptr) 
            return;

        delete[] buffer;
    }

    uint32_t getSamplesCount () const
    {
        return numSamples;
    }

    void setID (AudioBufferID _id)
    {
        id = _id;
    }

    AudioBufferID getID () const
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

    void setBufferSize (uint32_t bufferSize)
    {
        if (buffer != nullptr)
        {
            delete[] buffer;
            buffer = nullptr;
        }

        buffer = new SampleType[bufferSize];
        memset (buffer, 0, sizeof (SampleType) * bufferSize);
        numSamples = bufferSize;
    }

    uint32_t getBufferSize () const
    {
        return numSamples;
    }

    SampleType& operator[] (const uint32_t index)
    {
        assert (index < numSamples);
        return buffer[index];
    }

    const SampleType& operator[] (const uint32_t index) const
    {
        assert (index < numSamples);
        return buffer[index];
    }

    SampleType* const getData () const
    {
        return buffer;
    }

    static AudioBuffer<DSP::SampleType> Empty;

private:
    SampleType* buffer {nullptr};
    uint32_t numSamples {0};
    AudioBufferID id {0, 0};
    bool isBufferFree {false};
    uint32_t referenceCount {0};
};

AudioBuffer<DSP::SampleType> AudioBuffer<DSP::SampleType>::Empty (0, {0,0});

// TODO move to more appropriate place
using InputBufArray = std::vector<AudioBuffer<DSP::SampleType>*>;
using OutputBufArray = std::vector<AudioBuffer<DSP::SampleType>*>;

#endif
