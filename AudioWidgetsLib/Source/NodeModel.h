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
    enum IDs
    {
        InputNodeID = 0x10001,
        OutputNodeID = 0x20002,
        MidiInNodeID = 0x30003,
        MidiOutNodeID = 0x40004
    };

public:
    NodeModel () = default;
    virtual ~NodeModel () {};
    NodeModel (int _id) : id (_id) {}
    NodeModel (int _id, int _parentId) : id (_id), parentId (_parentId) {}
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

    virtual uint32_t getNumInputChannels () const
    {
        return 0;
    }

    virtual uint32_t getNumOutputChannels () const
    {
        return 0;
    }

    virtual bool acceptsMidi () const
    {
        return false;
    }

    virtual bool producesMidi () const
    {
        return false;
    }

    void setID (uint32_t _id) { this->id = _id; }
    uint32_t getID () const { return id; }

    void setParentID (uint32_t _pId) { this->parentId = _pId; }
    uint32_t getParentID () const { return parentId; }

    // Process function for filter type nodes
    virtual void process (InputBufArray buffersIn,
                          OutputBufArray buffersOut,
                          const uint32_t numSamples)
    {
        assert (numSamples > 0);
        //assert (buffersIn.size () == <number of channels we expect>)
        //assert (buffersOut.size () == <number of channels we expect>)
        
        for (auto& channel : buffersIn)
        {
            for (int i = 0; i < channel->getSamplesCount (); ++i)
            {
                
            }
        }
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
    static NodeModel Empty;

private:
    // TODO move position data into NodeController, model shouldn't care about visuals
    PointType position {0.f, 0.f};
    int id {0}, parentId {-1};
};

#endif
