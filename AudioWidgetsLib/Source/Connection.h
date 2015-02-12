#pragma once

#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED

#include "NodeModel.h"

struct Connection
{
    Connection (const int sourceNode, const int destNode)
        :   sourceNode (sourceNode),
            destNode (destNode)
    {}

    Connection (const NodeModel& node1, const NodeModel& node2)
        :   sourceNode (node1.getID ()),
            destNode (node2.getID ())
    {}
    
    NodeID sourceNode {0};
    NodeID destNode {0};
};

#endif