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
#include "DSP.h"

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
    bool canConnect (const Connection& testConnection) const;

    // Clear all nodes and operations from the graph
    void clearGraph ();

    // Sort and build the list of operations involved in producing output
    void buildGraph ();

    // Execute the graph of nodes to generate output
    void processGraph (const AudioBuffer<DSP::SampleType>& audioIn,
                       AudioBuffer<DSP::SampleType>& audioOut);

    void setSettings (Settings settings);
    const Settings& getSettings () const;

    struct Listener;
    bool addListener (Listener* const newListener);
    bool removeListener (const Listener* listener);

public:
    struct Listener
    {
        Listener () {}
        Listener (const Listener&) = delete;
        Listener& operator= (const Listener&) = delete;
        virtual ~Listener () = default;

        virtual void newNodeAdded () = 0;
        virtual void nodeRemoved () = 0;
        virtual void newConnectionAdded () = 0;
        virtual void connectionRemoved () = 0;
    };

private:
    void topologicalSortUtil (const NodeModel& parentNode,
                              const NodeModel& currentNode,
                              std::map<int, bool>& visited);

private:
    NodeMap nodes;
    std::vector<GraphOp*> graphOps;
    std::vector<Listener*> listeners;
    AudioBufferManager audioBufferManager;
    Settings settings;
};

#endif
