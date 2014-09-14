#pragma once

#ifndef GRAPHMODEL_H_INCLUDED
#define GRAPHMODEL_H_INCLUDED

#include <vector>
#include <stack>
#include <list>
#include <map>

#include "AudioBufferManager.h"
#include "Connection.h"
#include "GraphOp.h"
#include "NodeModel.h"

class GraphModel
{
public:
    GraphModel ();
    ~GraphModel ();

    bool addNode (const NodeModel& newNode);
    bool removeNode (const NodeModel& node);
    int nodeCount () const;

    bool addConnection (const Connection& newConnection);
    bool removeConnection (const Connection& connection);
    int connectionCount () const;

    void clearGraph ();

    // Sort and build the list of operations involved in producing output
    void buildGraph ();

    // Execute the graph of nodes to generate output
    void processGraph ();

    struct Settings
    {
        float sampleRate {44100};
        int blockSize {256};
    };

    void setSettings (Settings settings);
    Settings getSettings () const;

private:
    void topologicalSortUtil (const NodeModel& parentNode,
                              const NodeModel& currentNode,
                              std::map<int, bool>& visited);

private:
    std::map<NodeModel::ID, NodeModel> nodes;
    std::vector<GraphOp*> graphOps;
    AudioBufferManager audioBufferManager;
    Settings settings;
};

#endif
