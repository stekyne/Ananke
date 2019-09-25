#pragma once

#ifndef AUDIOBUFFER_H_INCLUDED
#define AUDIOBUFFER_H_INCLUDED

#include <cassert>
#include <vector>
#include <string>

#include "DSP.h"

namespace Ananke {

struct AudioBufferID
{
    AudioBufferID (const int id, const int channelNumber) :
        id (id),
        channelNumber (channelNumber)
    {
    }

    // TODO probably not the right way to express this
    AudioBufferID (std::vector<AudioBufferID>&& bufferIDs) :
        bufferIDs (bufferIDs),
        isComposite (true)
    {
    }

    auto getID () const { return id; }
    auto getChanelNumber () const { return channelNumber; }

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

    std::string toString () const
    {
        std::string name;
        name += "(";
        name += std::to_string (id);
        name += ":";
        name += std::to_string (channelNumber);
        name += ")";
        return name;
    }

    const static AudioBufferID Empty;

private:
    int id {0};
    int channelNumber {0};
    std::vector<AudioBufferID> bufferIDs;
    bool isComposite {false};
};

template <typename SampleType = DSP::SampleType>
class AudioBuffer
{
public:
    // Can't support 0 sized buffer so delete default constructor
    AudioBuffer () = delete;
    AudioBuffer (const AudioBuffer<SampleType>& other) = delete;
    AudioBuffer (AudioBuffer<SampleType>&& other) = delete;
    AudioBuffer& operator= (const AudioBuffer<SampleType>& other) = delete;
    AudioBuffer& operator= (AudioBuffer<SampleType>&& other) = delete;

    AudioBuffer (int numSamples, AudioBufferID id) :
        numSamples (numSamples),
        id (id),
        isBufferFree (id == AudioBufferID::Empty ? true : false)
    {
        assert (numSamples != 0);
        buffer = new SampleType[numSamples];
    }

    AudioBuffer (SampleType* buffer, int numSamples, AudioBufferID id) :
        buffer (buffer),
        numSamples (numSamples),
        id (id),
        isBufferFree (id == AudioBufferID::Empty ? true : false)
    {
        assert (numSamples != 0);
    }

    ~AudioBuffer () 
    {        
        if (shouldDelete == false)
            return;

        if (buffer == nullptr) 
            return;

        delete[] buffer;
        buffer = nullptr;
    }

    void clear ()
    {
        assert (buffer != nullptr);
        memset (buffer, 0, sizeof (SampleType) * numSamples);
    }

    int getSize () const
    {
        return numSamples;
    }

    void setID (const AudioBufferID& _id)
    {
        this->id = _id;
        isBufferFree = (id == AudioBufferID::Empty) ? true : false;
    }

    AudioBufferID getID () const
    {
        return id;
    }

    bool isFree () const 
    { 
        return isBufferFree; 
    }

    void setSize (int newBufferSize)
    {
        // Don't reallocate for smaller sizes
        if (newBufferSize <= numSamples)
        {
            assert (buffer != nullptr);
            numSamples = newBufferSize;
            return;
        }

        if (buffer != nullptr)
        {
            delete[] buffer;
            buffer = nullptr;
        }

        buffer = new SampleType[newBufferSize];
        memset (buffer, 0, sizeof (SampleType) * newBufferSize);
        numSamples = newBufferSize;
    }

    SampleType& operator[] (const int index)
    {
        assert (index < numSamples);
        return buffer[index];
    }

    const SampleType& operator[] (const int index) const
    {
        assert (index < numSamples);
        return buffer[index];
    }

    SampleType* const getData () const
    {
        return buffer;
    }

    void copyDataTo (DSP::SampleType* copyToBuffer, int _numSamples)
    {
        assert (this->numSamples == _numSamples);
        memcpy (copyToBuffer, buffer, numSamples * sizeof (float));
    }

    // Copies a single channel worth of audio into this buffer
    // The buffers must match in length (number of samples)
    void copyDataFrom (const DSP::SampleType* copyFromBuffer, int _numSamples)
    {
        assert (this->numSamples == _numSamples);
        assert (isReadOnly == false);
        memcpy (buffer, copyFromBuffer, _numSamples * sizeof (float));
    }

    // Add the passed in buffers signal to this buffers signal
    void addDataFrom (const AudioBuffer<DSP::SampleType>* otherBuffer)
    {
        assert (this->numSamples == otherBuffer->getSize ());
        assert (otherBuffer->buffer != nullptr);
        assert (isReadOnly == false);

        for (auto i = 0; i < numSamples; ++i)
        {
            this->buffer[i] += (*otherBuffer)[i];
        }
    }

    // Set the buffer to use in this class. This assumes that no data will 
    // be leaked by calling it. Therefore don't call it on an instance 
    // where it has allocated memory
    // Returns previously held buffer pointer incase it needs deletion
    SampleType* setBufferToUse (SampleType* newBuffer, int _numSamples)
    {
        assert (newBuffer != nullptr);
        auto oldBuffer = buffer;

        this->buffer = newBuffer;
        this->numSamples = _numSamples;
        this->shouldDelete = false;

        return oldBuffer;
    }

    SampleType* setBufferToUse (const SampleType* newBuffer, int _numSamples)
    {
        assert (newBuffer != nullptr);

        auto oldBuffer = buffer;

        // TODO need a better solution, awful
        this->buffer = const_cast<SampleType*> (newBuffer);
        this->numSamples = _numSamples;
        this->isReadOnly = true;
        this->shouldDelete = false;

        return oldBuffer;
    }

    static AudioBuffer<DSP::SampleType> Empty;

private:
    SampleType* buffer {nullptr};
    int numSamples {0};
    AudioBufferID id {0, 0};
    bool isBufferFree {true};
    bool isReadOnly {false};
    bool shouldDelete {true};
};

AudioBuffer<DSP::SampleType> AudioBuffer<DSP::SampleType>::Empty (1, {0,0});

using ReadOnlyBufferArray = std::vector<const AudioBuffer<DSP::SampleType>*>;
using BufferArray = std::vector<AudioBuffer<DSP::SampleType>*>;

}

#endif
