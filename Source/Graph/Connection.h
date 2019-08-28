#pragma once

#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED

#include "Node.h"

namespace APG {

struct Connection
{
    Connection () = default;
    Connection (uint32_t sourceNode, uint32_t sourceChannel, 
                uint32_t destNode, uint32_t destChannel) :
        sourceNode (sourceNode),
        sourceChannel (sourceChannel),
        destNode (destNode),
        destChannel (destChannel)
    {}

    Connection (const Node& node1, uint32_t node1Channel,
                const Node& node2, uint32_t node2Channel) :
        sourceNode (node1.getID ()), 
        sourceChannel (node1Channel),
        destNode (node2.getID ()), 
        destChannel (node2Channel)
    {}

    Connection (const Node* const node1, uint32_t node1Channel,
                const Node* const node2, uint32_t node2Channel) :
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
    
    uint32_t sourceNode {0}, destNode {0};
    uint32_t sourceChannel {0}, destChannel {0};
};

}

#endif