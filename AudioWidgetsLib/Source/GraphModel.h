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
    using NodeMap = std::map<const NodeID, NodeModel*>;
    using ConnectionTable = std::vector<bool>;

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

    // Clear all nodes and operations from the graph
    void clearGraph ();

    // Sort and build the list of operations involved in producing output
    void buildGraph ();

    // Execute the graph of nodes to generate output
    void processGraph (const AudioBuffer<DSP::SampleType>& audioIn,
                       AudioBuffer<DSP::SampleType>& audioOut);

    void setSettings (Settings settings);
    const Settings& getSettings () const;

    std::string printGraph ()
    {
        std::stringstream buffer;
        buffer << "Graph: Connections: " << std::endl;

        buffer << "  ";
        for (int x = 0; x < totalNodeCount; ++x)
            buffer << x % 10;

        buffer << std::endl;
        
        for (int x = 0; x < totalNodeCount; ++x)
        {
            buffer << x % 10 << " ";
            for (int y = 0; y < totalNodeCount; ++y)
            {
                const bool value = connections[x * totalNodeCount + y];
                buffer << (value == true ? "o" : "-");
            }
            buffer << std::endl;
        }
        buffer << std::endl << "Operations: " << std::endl;
        for (int x = graphOps.size (); --x >= 0;)
        {
            buffer << graphOps[x]->getName () << std::endl;
        }
        buffer << "____________________" << std::endl;
        return buffer.str ();
    }

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

    inline bool setConnectionTableValue (unsigned int sourceNode, 
                                         unsigned int destinationNode,
                                         bool value)
    {
        assert (totalNodeCount * totalNodeCount < (int)connections.size ());
        connections[(sourceNode * totalNodeCount) + destinationNode] = value;
        if (value) activeConnections++; else activeConnections--;
        return false;
    }

    inline bool getConnectionTableValue (unsigned int sourceNode,
                                         unsigned int destinationNode) const
    {
        assert (totalNodeCount * totalNodeCount < (int)connections.size ());
        return connections[(sourceNode * totalNodeCount) + destinationNode];
    }

    void resizeTable (int newSize, int oldRowSize, int oldColumnSize)
    {
        ConnectionTable oldConnectionTable (connections);
        connections.clear ();
        connections.reserve (newSize);

        for (int x = 0; x < oldRowSize; ++x)
        {
            for (int y = 0; y < oldColumnSize; ++y)
            {
                connections[x + y] = oldConnectionTable[x + y];
            }
        }
    }

    std::vector<int> getDependentsForNode (unsigned int nodeID)
    {
        std::vector<int> dependentVec;

        for (int i = 0; i < totalNodeCount; ++i)
        {
            if (connections[(nodeID * totalNodeCount) + i] == true)
                dependentVec.push_back (i);
        }

        return dependentVec;
    }

    void clearConnectionsForNode (unsigned int nodeID)
    {
        for (int i = 0; i < totalNodeCount; ++i)
        {
            connections[nodeID + i] = false;
        }
    }

private:
    NodeMap nodes;
    ConnectionTable connections;
    std::vector<GraphOp*> graphOps;
    std::vector<Listener*> listeners;
    AudioBufferManager audioBufferManager;
    Settings settings;
    int activeConnections {0}, totalNodeCount {0};
    unsigned int internalIDcount {0};
};

#endif
