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
#include "../DSP.h"

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
private:
    using NodeMap = std::map<const NodeID, NodeModel*>;
    
public:
    GraphModel ();
    GraphModel (Settings settings);
    ~GraphModel ();

    bool addNode (NodeModel* const newNode);
    bool removeNode (const NodeModel* const node);
    int nodeCount () const;

    const NodeMap& getNodes () const;
    const NodeModel* const getNodeForID (int id);

    bool addConnection (const Connection& newConnection);
    bool addConnection (const NodeModel* const srcNode, 
                        const NodeModel* const destNode);
    bool removeConnection (const Connection& connection);
 
    int connectionCount () const;
    bool connectionExists (const Connection& testConnection) const;

    // Clear all nodes and operations from the graph
    void clearGraph ();

    // Sort and build the list of operations involved in producing output
    void buildGraph ();

    // Execute the graph of nodes to generate output
    void processGraph (const AudioBuffer<DSP::SampleType>& audioIn,
                       AudioBuffer<DSP::SampleType>& audioOut);

    void setSettings (Settings settings);
    Settings getSettings () const;

private:
    void topologicalSortUtil (const NodeModel& parentNode,
                              const NodeModel& currentNode,
                              std::map<int, bool>& visited);

private:
    NodeMap nodes;
    std::vector<GraphOp*> graphOps;
    AudioBufferManager audioBufferManager;
    Settings settings;
};

#endif
