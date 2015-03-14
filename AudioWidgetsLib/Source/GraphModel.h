#pragma once

#ifndef GRAPHMODEL_H_INCLUDED
#define GRAPHMODEL_H_INCLUDED

#include <vector>
#include <stack>
#include <list>
#include <map>
#include <sstream>

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
    int graphCapacity {50};

    Settings (float sampleRate, int blockSize, int graphCapacity)
        :   sampleRate (sampleRate),
            blockSize (blockSize),
            graphCapacity (graphCapacity)
    {
    }
};

class GraphModel
{
private:
    using NodeMap = std::map<uint32_t, NodeModel*>;

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
    bool addConnection (const NodeModel& srcNode, const NodeModel& destNode);
    bool removeConnection (const Connection& connection);
    int connectionCount () const;
    bool connectionExists (const Connection& testConnection) const;
    bool canConnect (const Connection& testConnection) const;
    const std::vector<Connection>& GraphModel::getConnections () const;

    // Clear all nodes and operations from the graph
    void clearGraph ();

    // Sort and build the list of operations involved in producing output
    void buildGraph ();

    // Execute the graph of nodes to generate output
    void processGraph (const AudioBuffer<DSP::SampleType>& audioIn,
                       AudioBuffer<DSP::SampleType>& audioOut);

    void setSettings (Settings settings);
    const Settings& getSettings () const;

    std::string printGraph () const;
    const std::vector<GraphOp*> getGraphOps () const;

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
        virtual void graphSettingsChanged () = 0;
    };

    bool addListener (Listener* const newListener);
    bool removeListener (const Listener* listener);

private:
    int getFreeInternalID ()
    {
        // TODO need a way of tracking free IDss
        return internalIDcount++;
    }

    void topologicalSortUtil (const NodeModel& parentNode,
                              const NodeModel& currentNode,
                              std::map<int, bool>& visited);
    std::vector<int> getDependentsForNode (unsigned int nodeID);
    void clearConnectionsForNode (unsigned int nodeID);

private:
    NodeMap nodes;
    std::vector<Connection> connections;
    std::vector<GraphOp*> graphOps;
    std::vector<Listener*> listeners;
    AudioBufferManager audioBufferManager;
    Settings settings;
    unsigned int internalIDcount {0};
};

#endif
