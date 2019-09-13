#pragma once

#ifndef Graph_H_INCLUDED
#define Graph_H_INCLUDED

#include <vector>
#include <unordered_map>
#include <atomic>

#include "AudioBuffer.h"
#include "AudioBufferManager.h"
#include "Connection.h"
#include "DSP.h"
#include "GraphOp.h"
#include "Node.h"
#include "../Graph/NodeTypes/AudioInputNode.h"
#include "../Graph/NodeTypes/AudioOutputNode.h"

namespace APG {

class Graph
{
public:
    enum IDs
    {
        AudioInputID = 0x1001,
        AudioOutputID = 0x2002,
        MidiInputID = 0x3003,
        MidiOutputID = 0x4004
    };

public:
    struct Settings
    {
        // This is the expected sampling rate that the audio interface is using
        float sampleRate {44100};

        // This is the expected block size that the audio callback will ask for
        int blockSize {256};

        // This is the internal rate at which control signals will update
        int controlRate {32};

        // Number of input channels for the graph
        int numberInputChannels {2};

        // Number of output channels for the graph
        int numberOutputChannels {2};

        Settings (float sampleRate, int blockSize, int controlRate) :
            sampleRate (sampleRate),
            blockSize (blockSize),
            controlRate (controlRate)
        {
        }
    };

public:
    Graph ();
    Graph (Settings settings);
    ~Graph ();

    // Returns true if the graph has no nodes or connections present
    bool isEmpty () const;

    // Adds new Node to the graph and takes ownership if held by graph
    // at the time of graph destruction
    bool addNode (Node* const newNode);

    // Removes Node from the graph but does not delete Node
    bool removeNode (const Node* const node);
    int nodeCount () const;
    const std::vector<Node*>& getNodes () const { return nodes; }

    Node* const getNodeForID (int id);

    // Adds new Connection to graph if valid
    bool addConnection (const Connection& newConnection);
    bool addConnection (std::vector<Connection> newConnections);
    bool canConnect (const Connection& newConnection);
    bool removeConnection (const Connection& connection);
    int connectionCount () const;

    // Tests if the provided connection already exists in the graph
    bool connectionExists (const Connection& testConnection) const;

    const std::vector<Connection>& getConnections () const { return connections; }

    // Tests whether a provided connection is valid
    // This does not include whether or not adding this connection will create a loop
    // This does check if the information given is valid for this graph 
    // i.e. node ID's and channel numbers are valid for current graph
    bool isValidNewConnection (const Connection& connection) const;

    // Clear all nodes and operations from the graph
    void clearGraph ();

    // Sort and build the list of operations involved in producing output
    bool buildGraph ();

    const std::vector<GraphOp*>& Graph::getGraphOps () const { return graphOps; }

    // Execute the graph of nodes to generate output
    // Returns true if the graph has reconfigured due to incoming parameter changes
    // i.e. number of inputs/outputs has changed or block size changed
    bool processGraph (const float** audioIn, const int inputChannelNum, 
                       float** audioOut, const int outputChannelNum,
                       const int blockSize);

    void setSettings (Settings settings);
    const Settings& getSettings () const;

    // Pretty print of the current graph showing connections and node processing order
    std::string printGraph () const;

public:
    struct Listener
    {
        Listener () = default;
        Listener (const Listener&) = delete;
        Listener& operator= (const Listener&) = delete;
        virtual ~Listener () = default;

        virtual void newNodeAdded (const Node* const /*newNode*/) {};
        virtual void nodeRemoved (const Node* const /*removedNode*/) {};
        virtual void newConnectionAdded (const Connection& /*newConnection*/) {};
        virtual void newConnectionAdded (const std::vector<Connection>& /*newConnection*/) {};
        virtual void connectionRemoved (const Connection& /*removedConnection*/) {};
        virtual void graphSettingsChanged () {};
    };

    bool addListener (Listener* const newListener);
    bool removeListener (const Listener* listener);

private:
    struct Markers
    {
        bool permanentMark = false;
        bool temporaryMark = false;
    };

    // Returns false if the current node has already been visited
    bool topologicalSortUtil (Node* currentNode,
                              std::unordered_map<int, Markers>& visited,
                              std::vector<Node*>& sortedNodes);
    
    // Returns true if graph can be sorted with no loops
    bool performSort (std::vector<Node*>& sortedNodes);

    std::vector<int> getDependentsForNode (int nodeID);
    void clearConnectionsForNode (int nodeID);

    // Attach external audio buffers to graph
    bool setIONodeBuffers (const float** const inBuffers, int inputChannels,
                           float** const outBuffers, int outputChannels, 
                           int numSamples);

private:
    std::vector<Node*> nodes;
    std::vector<Connection> connections;
    // TODO thread safety, needs to be an atomic reference to pass new graphOps on audio thread
    std::vector<GraphOp*> graphOps;
    std::atomic<bool> hasGraphOpsChanged {false};
    AudioBufferManager audioBufferManager {50};
    Settings settings {44100.f, 32, 16};
    std::vector<Listener*> listeners;
	AudioInputNode audioIn;
	AudioOutputNode audioOut;

    friend struct GraphVerifier;
};

}

#endif
