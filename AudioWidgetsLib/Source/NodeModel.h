#pragma once

#ifndef NODEMODEL_H_INCLUDED
#define NODEMODEL_H_INCLUDED

#include <vector>
#include <string>

#include "AudioBuffer.h"

class NodeModel
{
public:
    class ID
    {
    public:
        ID (const int id)  : nodeNum (id)
        {
        }

        ID (const ID& other) : ID (other.nodeNum)
        {
        }

        inline void operator= (const ID& rhs)
        {
            this->nodeNum = rhs.nodeNum;
        }

        inline bool operator< (const ID& rhs)
        {
            return this->nodeNum < rhs.getNumber ();
        }
        
        friend bool operator< (const ID& lhs, const ID& rhs)
        {
            return lhs.getNumber () < rhs.getNumber ();
        }

        inline bool operator== (const ID& rhs)
        {
            return nodeNum == rhs.nodeNum;
        }

        friend bool operator== (const ID& lhs, const ID& rhs)
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

public:
    NodeModel () : id (-1) {}
    explicit NodeModel (const int _id) : id (_id) {}
    NodeModel (const NodeModel& other);
    NodeModel& operator= (const NodeModel& other);
    friend bool operator==(const NodeModel& lhs, const NodeModel& rhs);
    friend bool operator!=(const NodeModel& lhs, const NodeModel& rhs);
    virtual ~NodeModel () {};

    void addDependentNode (const ID id);
    void removeDependentNode (const ID id);
    void clearDependentNodes ();
    int dependentNodeCount () const;
    const std::vector<ID>& getDependentNodes () const;

    inline const ID& getID () const { return id; }

    virtual void process (const AudioBuffer<float>* const audioIn,
                          AudioBuffer<float>* const audioOut,
                          const unsigned int numSamples) {}

    virtual void process (const AudioBuffer<float>* const audioOut,
                          const unsigned int numSamples) {}

    const static NodeModel Empty;

private:
    std::vector<ID> dependentNodes;
    ID id;
};

#endif
