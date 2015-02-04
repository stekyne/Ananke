#pragma once

#ifndef GRAPHMODEL_H_INCLUDED
#define GRAPHMODEL_H_INCLUDED

#include <vector>
#include <stack>
#include <list>
#include <map>

#include "AudioBuffer.h"
#include "AudioBufferManager.h"
#include "Connection.h"
#include "GraphOp.h"
#include "NodeModel.h"

struct Settings
{
    float sampleRate {44100};
    int blockSize {256};

    Settings (float sampleRate, int blockSize)
        :   sampleRate (sampleRate),
            blockSize (blockSize)
    {
    }
};

class GraphModel
{
public:
    GraphModel ();
    GraphModel (Settings settings);
    ~GraphModel ();

    bool addNode (NodeModel* const newNode);
    bool removeNode (const NodeModel* const node);
    int nodeCount () const;
    const std::map<NodeID, NodeModel*>& getNodes () const;

    bool addConnection (const Connection& newConnection);
    bool addConnection (const NodeModel* const srcNode, 
                        const NodeModel* const destNode);
    bool removeConnection (const Connection& connection);
    int connectionCount () const;
    bool connectionExists (const Connection& testConnection) const;

    const NodeModel& getNodeForID (int id);

    void clearGraph ();

    // Sort and build the list of operations involved in producing output
    void buildGraph ();

    // Execute the graph of nodes to generate output
    void processGraph (const AudioBuffer<float>& audioIn,
                       AudioBuffer<float>& audioOut);

    void setSettings (Settings settings);
    Settings getSettings () const;

private:
    void topologicalSortUtil (const NodeModel& parentNode,
                              const NodeModel& currentNode,
                              std::map<int, bool>& visited);

private:
    std::map<NodeID, NodeModel*> nodes;
    std::vector<GraphOp*> graphOps;
    AudioBufferManager<float> audioBufferManager;
    Settings settings;
};

#endif
