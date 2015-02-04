#pragma once

#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED

#include "NodeModel.h"

struct Connection
{
    Connection (const NodeID _sourceNode,
                const NodeID _destNode)
        :   sourceNode (_sourceNode),
            destNode (_destNode)
    {}
    
    NodeID sourceNode {0};
    NodeID destNode {0};
};

#endif