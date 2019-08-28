#pragma once

#ifndef GRAPHVERIFIER_H
#define GRAPHVERIFIER_H

#include "..\..\src\Graph.h"

namespace APG
{
    struct GraphVerifier
    {
        static bool Assert (const Graph& /*graph*/, const std::string&& /*graphTopology*/)
        {
            // TODO parse the graph topology string
            //graphTopology
            // TODO convert to data structures to compare against graph

            // TODO test graph topology with actual graph structure
            //for (const auto& op : graph.graphOps)
            {

            }

            return false;
        }

    private:
        // Used to check if a certain Node is processed before another
        static bool comesBefore (const Graph& graph, uint32_t nodeId1, uint32_t nodeId2)
        {
            for (const auto& op : graph.graphOps)
            {
                const auto node = dynamic_cast<ProcessNodeOp*> (op);

                if (node != nullptr)
                {
                    if (node->getNodeId () == nodeId1)
                        return true;
                    else if (node->getNodeId () == nodeId2)
                        return false;
                }
            }

            return false;
        }
    };

}

#endif
