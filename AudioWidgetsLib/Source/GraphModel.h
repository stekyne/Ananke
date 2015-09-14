#pragma once

#ifndef GRAPHMODEL_H_INCLUDED
#define GRAPHMODEL_H_INCLUDED

#include <vector>
#include <stack>
#include <list>
#include <unordered_map>
#include <sstream>

#include "AudioBuffer.h"
#include "AudioBufferManager.h"
#include "Connection.h"
#include "DSP.h"
#include "GraphOp.h"
#include "NodeModel.h"
#include "..\Source\NodeTypes\ExternalNode.h"

class GraphModel
{
public:
    struct Settings
    {
        float sampleRate {44100};
        int blockSize {256};
		int controlRate {32};

        Settings (float sampleRate, int blockSize, int controlRate)
            :   sampleRate (sampleRate),
                blockSize (blockSize),
				controlRate (controlRate)
        {
        }
    };

private:
    using NodeMap = std::unordered_map<uint32_t, NodeModel*>;

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

    // Initialise the graph based on environmental parameters
    // Such as number of inputs, outputs, sample and control rate
    bool initialise ();

    // Clear all nodes and operations from the graph
    void clearGraph ();

    // Sort and build the list of operations involved in producing output
    bool buildGraph ();

    // Execute the graph of nodes to generate output
    void processGraph (const float** audioIn, float** audioOut,
                       const uint32_t blockSize);    

    void setSettings (Settings settings);
    const Settings& getSettings () const;

    // Pretty print of the current graph showing connections and node processing order
    std::string printGraph () const;
    
    // Get the list of graph operations required to produce audio output
    const std::vector<GraphOp*> getGraphOps () const;
    
    // Get the node processing order (based on dependencies) as a list of node IDs
    std::vector<int> getGraphOrderAsList () const;

public:
    struct Listener
    {
        Listener () = default;
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
    struct Markers
    {
        bool permanentMark = false;
        bool temporaryMark = false;
    };

    int getFreeInternalID ()
    {
        return ++internalIDcount;
    }

    // Returns false if the current node has already been visited
    bool topologicalSortUtil (const NodeModel& parentNode,
                              NodeModel& currentNode,
                              std::unordered_map<int, Markers>& visited,
                              std::vector<NodeModel>& sortedNodes);
    
    // Returns true if graph can be sorted with no loops
    bool performSort (std::vector<NodeModel>& sortedNodes);

    std::vector<uint32_t> getDependentsForNode (unsigned int nodeID);
    void clearConnectionsForNode (unsigned int nodeID);

    // Add 'empty' and 'input'/'output' nodes to cleared graph
    void addFixedNodes ();

    // Attach incoming raw buffers to the external nodes
    void setInputNodeBuffers (const float** const buffers, uint32_t numChannels, uint32_t numSamples);
    void setOutputNodeBuffers (float** const buffers, uint32_t numChannels, uint32_t numSamples);

private:
    NodeMap nodes;
    std::vector<Connection> connections;
    std::vector<GraphOp*> graphOps;
    std::vector<Listener*> listeners;
    AudioBufferManager audioBufferManager {50};
    ExternalNode* inputNode {nullptr};
    ExternalNode* outputNode {nullptr};
    Settings settings {44100.f, 50, 32};
    unsigned int internalIDcount {0};
};

#endif
