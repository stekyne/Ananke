#pragma once

#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED

#include "NodeModel.h"

struct Connection
{
    Connection () = default;
    Connection (const uint32_t sourceNode, const uint32_t destNode)
        :   sourceNode (sourceNode),
            destNode (destNode)
    {}

    Connection (const NodeModel& node1, const NodeModel& node2)
        :   sourceNode (node1.getID ()),
            destNode (node2.getID ())
    {}

    static friend bool operator== (const Connection& lhs, const Connection& rhs)
    {
        if (lhs.sourceNode == rhs.sourceNode &&
            lhs.sourceChannel == rhs.sourceChannel &&
            lhs.destNode == rhs.destNode &&
            lhs.destChannel == rhs.destChannel)
            return true;

        return false;
    }
    
    uint32_t sourceNode {0}, destNode {0};
    uint32_t sourceChannel {0}, destChannel {0};

};

#endif