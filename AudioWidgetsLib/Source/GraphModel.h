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


class GraphModel
{
public:
    struct Settings
    {
        float sampleRate {44100};
        int blockSize {256};
        int graphCapacity {50};

        Settings (float sampleRate, int blockSize, int graphCapacity)
            : sampleRate (sampleRate),
            blockSize (blockSize),
            graphCapacity (graphCapacity)
        {
        }
    };

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
    NodeModel* const getNodeForID (int id);

    bool addConnection (const Connection& newConnection);
    bool addConnection (const NodeModel& sourceNode, uint32_t sourceChannel,
                        const NodeModel& destNode, uint32_t destChannel);
    bool removeConnection (const Connection& connection);
    int connectionCount () const;
    bool connectionExists (const Connection& testConnection) const;
    bool canConnect (const Connection& testConnection);
    bool validateConnection (const Connection& connection);
    const std::vector<Connection>& GraphModel::getConnections () const;

    // Clear all nodes and operations from the graph
    void clearGraph ();

    // Sort and build the list of operations involved in producing output
    bool buildGraph ();

    // Execute the graph of nodes to generate output
    void processGraph (const AudioBuffer<DSP::SampleType>& audioIn,
                       AudioBuffer<DSP::SampleType>& audioOut,
                       const unsigned int blockSize);

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

    struct Markers
    {
        bool permanentMark = false;
        bool temporaryMark = false;
    };

private:
    struct NodeDescriptor
    {
        int nodeId {0}, parentNode {0};
        
        NodeDescriptor (uint32_t nodeID, uint32_t parentNode) :
            nodeId (nodeID), parentNode (parentNode)
        {
        }
    };

    int getFreeInternalID ()
    {
        // TODO need a way of tracking free IDss
        return ++internalIDcount;
    }

    // Returns false if the current node has already been visited
    bool topologicalSortUtil (const NodeModel& parentNode,
                              const NodeModel& currentNode,
                              std::map<int, Markers>& visited,
                              std::vector<NodeDescriptor>& sortedNodes);
    
    // Returns true if graph can be sorted with no loops
    bool performSort (std::vector<NodeDescriptor>& sortedNodes);

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
