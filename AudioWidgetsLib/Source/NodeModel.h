#pragma once

#ifndef NODEMODEL_H_INCLUDED
#define NODEMODEL_H_INCLUDED

#include <vector>
#include <string>

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
    
    NodeModel (const NodeModel& other)
        :   id (other.id),
            dependentNodes (other.dependentNodes)
    {
    }

    NodeModel& operator= (const NodeModel& other)
    {
        this->id = NodeID (other.getID ());
        this->dependentNodes = other.dependentNodes;
        return *this;
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

    void addDependentNode (const NodeID id)
    {
        dependentNodes.push_back (id);
    }

    void removeDependentNode (const NodeID id)
    {
        auto iter = dependentNodes.cbegin ();

        while (iter != dependentNodes.cend ())
        {
            if (iter->getNumber () == id.getNumber ())
            {
                dependentNodes.erase (iter);
                break;
            }
            ++iter;
        }
    }

    void clearDependentNodes ()
    {
        dependentNodes.clear ();
    }

    int dependentNodeCount () const
    {
        return dependentNodes.size ();
    }

    const std::vector<NodeID>& getDependentNodes () const
    {
        return dependentNodes;
    }

    inline const NodeID& getID () const { return id; }

    // TODO use the CRTP method instead of a virual call
    virtual void process (const AudioBuffer<float>* const audioIn,
                          AudioBuffer<float>* const audioOut,
                          const unsigned int numSamples)
    {
        // AudioIn can be null so overriding nodes must check if it applies or not
        assert (audioOut != nullptr);
        assert (numSamples > 0);
        assert (audioOut->getBufferSize () > 0 && 
                audioOut->getBufferSize () <= numSamples);
    }

    const static NodeModel Empty;

private:
    std::vector<NodeID> dependentNodes;
    NodeID id;
};

#endif
