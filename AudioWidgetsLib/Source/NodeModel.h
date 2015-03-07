#pragma once

#ifndef NODEMODEL_H_INCLUDED
#define NODEMODEL_H_INCLUDED

#include <vector>
#include <string>
#include <tuple>

#include "AudioBuffer.h"

class NodeID
{
public:
    NodeID (const int id) : nodeNum (id)
    {
    }

    NodeID (const NodeID& other) : NodeID (other.nodeNum)
    {
    }

    inline void operator= (const NodeID& rhs)
    {
        this->nodeNum = rhs.nodeNum;
    }

    inline bool operator< (const NodeID& rhs)
    {
        return this->nodeNum < rhs.getNumber ();
    }

    friend bool operator< (const NodeID& lhs, const NodeID& rhs)
    {
        return lhs.getNumber () < rhs.getNumber ();
    }

    inline bool operator== (const NodeID& rhs)
    {
        return nodeNum == rhs.nodeNum;
    }

    friend bool operator== (const NodeID& lhs, const NodeID& rhs)
    {
        return lhs.nodeNum == rhs.nodeNum;
    }

    inline int getNumber () const
    {
        return nodeNum;
    }

private:
    int nodeNum;
};

class NodeModel
{
public:
    NodeModel () : id (-1) {}
    explicit NodeModel (const int _id) : id (_id) {}
    NodeModel (const int _id, float positionX, float positionY)
        : id (_id),
        position (positionX, positionY)
    {
    }

    friend bool operator==(const NodeModel& lhs, const NodeModel& rhs)
    {
        if (lhs.getID ().getNumber () == rhs.getID ().getNumber ())
            return true;

        return false;
    }

    friend bool operator!=(const NodeModel& lhs, const NodeModel& rhs)
    {
        if (lhs.getID ().getNumber () == rhs.getID ().getNumber ())
            return false;

        return true;
    }

    virtual ~NodeModel () {};

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

    void setID (NodeID id) { this->id = id; }
    const NodeID& getID () const { return id; }

    // TODO use the CRTP method instead of a virual call
    virtual void process (const AudioBuffer<float>* const /*audioIn*/,
                          AudioBuffer<float>* const audioOut,
                          const unsigned int numSamples)
    {
        // AudioIn can be null so overriding nodes must check if it applies or not
        assert (audioOut != nullptr);
        assert (numSamples > 0);
        assert (audioOut->getBufferSize () > 0 && 
                audioOut->getBufferSize () <= numSamples);
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
    NodeID id;
};

#endif
