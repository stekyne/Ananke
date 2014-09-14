#pragma once

#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED

#include "NodeModel.h"

struct Connection
{
    Connection (const NodeModel::ID _sourceNode, 
                const NodeModel::ID _destNode)
        :   sourceNode (_sourceNode),
            destNode (_destNode)
    {}
    
    const NodeModel::ID sourceNode {0};
    const NodeModel::ID destNode {0};
};

#endif