#pragma once

#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED

#include "Node.h"

namespace Ananke {

struct Connection
{
    Connection () = default;
    Connection (int sourceNode_, int sourceChannel_,
                int destNode_, int destChannel_) :
        sourceNode (sourceNode_),
        sourceChannel (sourceChannel_),
        destNode (destNode_),
        destChannel (destChannel_)
    {}

    Connection (const Node& node1, int node1Channel,
                const Node& node2, int node2Channel) :
        sourceNode (node1.getID ()), 
        sourceChannel (node1Channel),
        destNode (node2.getID ()), 
        destChannel (node2Channel)
    {}

    Connection (const Node* const node1, int node1Channel,
                const Node* const node2, int node2Channel) :
        sourceNode (node1->getID ()),
        sourceChannel (node1Channel),
        destNode (node2->getID ()),
        destChannel (node2Channel)
    {
    }

    static friend bool operator== (const Connection& lhs, const Connection& rhs)
    {
        if (lhs.sourceNode == rhs.sourceNode &&
            lhs.sourceChannel == rhs.sourceChannel &&
            lhs.destNode == rhs.destNode &&
            lhs.destChannel == rhs.destChannel)
            return true;

        return false;
    }

    static friend bool operator!= (const Connection& lhs, const Connection rhs)
    {
        return !(lhs==rhs);
    }
    
	int sourceNode = -1;
	int destNode = -1;
	int sourceChannel = -1;
	int destChannel = -1;
};

}

#endif