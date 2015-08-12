#pragma once

#ifndef NODEMODEL_H_INCLUDED
#define NODEMODEL_H_INCLUDED

#include <vector>
#include <string>
#include <tuple>
#include <cstdint>

#include "AudioBuffer.h"

class NodeModel
{
public:
    NodeModel () = default;
    virtual ~NodeModel () {};
    NodeModel (int id) : id (id) {}
    NodeModel (int id, float positionX, float positionY)
        :   position (positionX, positionY),
            id (id)
    {
    }

    friend bool operator==(const NodeModel& lhs, const NodeModel& rhs)
    {
        if (lhs.getID () == rhs.getID ())
            return true;

        return false;
    }

    friend bool operator!=(const NodeModel& lhs, const NodeModel& rhs)
    {
        if (lhs.getID () == rhs.getID ())
            return false;

        return true;
    }

    virtual const char* const getName () const
    {
        return "Node";
    }

    virtual unsigned int getNumInputChannels () const
    {
        return 0;
    }

    virtual unsigned int getNumOutputChannels () const
    {
        return 0;
    }

    virtual bool acceptsMidi () const
    {
        return true;
    }

    virtual bool producesMidi () const
    {
        return false;
    }

    void setID (uint32_t _id) { this->id = _id; }
    uint32_t getID () const { return id; }

    virtual void process (const AudioBuffer<float>* const /*audioIn*/,
                          AudioBuffer<float>& audioOut,
                          const unsigned int numSamples)
    {
        // AudioIn can be null so overriding nodes must check if it applies or not
        assert (numSamples > 0);
        assert (audioOut.getBufferSize () > 0 && 
                audioOut.getBufferSize () >= numSamples);
    }

    using PointType = std::tuple <float, float>;

    void setPosition (PointType point)
    {
        position = point;
    }

    const PointType& getPosition () const
    {
        return position;
    }

    // An empty instance of this class
    const static NodeModel Empty;

private:
    // TODO move position data into NodeController, model shouldn't care about visuals
    PointType position {0.f, 0.f};
    uint32_t id {0};
};

#endif
