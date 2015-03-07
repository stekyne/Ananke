#include <stack>
#include <iostream>
#include <list>
#include <iterator>

#include "GraphModel.h"

GraphModel::GraphModel ()
    :   settings {44100.f, 50, 50},
        audioBufferManager (50),
        connections (50)
{
}

GraphModel::GraphModel (Settings settings)
    :   settings (settings),
        audioBufferManager (settings.blockSize)
{
}

GraphModel::~GraphModel ()
{
    for (unsigned int i = 0; i < graphOps.size (); ++i)
        if (graphOps[i] != nullptr)
            delete graphOps[i];

    for (auto& node : nodes)
        if (node.second != nullptr)
            delete node.second;
}

bool GraphModel::addNode (NodeModel* const newNode)
{
    newNode->setID (getFreeInternalID ());
    nodes[newNode->getID ().getNumber ()] = newNode;
    totalNodeCount++;
    // TODO if the total amount of nodes is greater than the capacity
    // We now need to reallocate more space
    return true;
}

bool GraphModel::removeNode (const NodeModel* const node)
{
    auto iter = nodes.cbegin ();

    while (iter != nodes.end ())
    {
        auto itrNode = iter->second;
        if (itrNode->getID () == node->getID ())
        {
            nodes.erase (iter);
            totalNodeCount--;
            clearConnectionsForNode (node->getID ().getNumber ());
            break;
        }
        ++iter;
    }

    return true;
}

int GraphModel::nodeCount () const
{
    return nodes.size ();
}

const GraphModel::NodeMap& GraphModel::getNodes () const
{
    return nodes;
}

const NodeModel* const GraphModel::getNodeForID (int id)
{
    return nodes[id];
}

bool GraphModel::addConnection (const Connection& newConnection)
{
    return setConnectionTableValue (newConnection.sourceNode.getNumber (),
                                    newConnection.destNode.getNumber (), true);
}

bool GraphModel::addConnection (const NodeModel& sourceNode, 
                                const NodeModel& destNode)
{
    return setConnectionTableValue (sourceNode.getID ().getNumber (),
                                    destNode.getID ().getNumber (), true);
}

bool GraphModel::removeConnection (const Connection& connection)
{
    return setConnectionTableValue (connection.sourceNode.getNumber (),
                                    connection.destNode.getNumber (), false);
}

int GraphModel::connectionCount () const
{
    return activeConnections;
}

bool GraphModel::connectionExists (const Connection& testConnection) const
{
    return getConnectionTableValue (testConnection.sourceNode.getNumber (), 
                                    testConnection.destNode.getNumber ());
}

bool GraphModel::canConnect (const Connection& testConnection) const
{
    // TODO implement if one node and connect to another
    // perform a topological sort and test for loops
    return false;
}

void GraphModel::clearGraph ()
{
    nodes.clear ();
    graphOps.clear ();
}

void GraphModel::buildGraph ()
{
    // Mark all the vertices as not visited
    std::map<int, bool> visited;

    for (auto& node : nodes)
    {
        visited[node.second->getID ().getNumber ()] = false;
    }

    // Call the recursive helper function to store Topological Sort
    // starting from all vertices one by one
    for (auto& node : nodes)
    {
        if (visited[node.second->getID ().getNumber ()] == false)
            topologicalSortUtil (NodeModel::Empty, *node.second, visited);
    }
}

void GraphModel::processGraph (const AudioBuffer<DSP::SampleType>& audioIn,
                               AudioBuffer<DSP::SampleType>& audioOut)
{
    for (int i = graphOps.size (); --i >= 0;)
    {
        graphOps[i]->perform ();
    }
}

void GraphModel::topologicalSortUtil (const NodeModel& parentNode,
                                      const NodeModel& currentNode, 
                                      std::map<int, bool>& visited)
{
    const int currentNodeID = currentNode.getID ().getNumber ();

    // Mark the current node as visited.
    visited[currentNodeID] = true;
    
    // Associate this node's output with a buffer
    auto freeBuffer = audioBufferManager.getFreeBuffer ();
    audioBufferManager.associatedBufferWithNode (freeBuffer, currentNodeID);

    // Recur for all the nodes adjacent to this node
    for (auto& adjacentNode : getDependentsForNode (currentNodeID))
    {
        if (!visited[adjacentNode])
        {
            topologicalSortUtil (currentNode, *nodes[adjacentNode],
                                 visited);
        }
    }
    
    // If parent mode is 'empty' it means there is no incoming node
    const auto parentBufferID = parentNode == NodeModel::Empty ? AudioBufferID::Empty : 
        audioBufferManager.getAssociatedBufferForNodeId (parentNode.getID ().getNumber ());

    // This node depends on the parent node to be processed first, we need to get the buffer that 
    // contains the output of the parent node and use it as the input of this node
    graphOps.push_back (new ProcessNodeOp (parentBufferID, freeBuffer,
        settings.blockSize, const_cast<NodeModel&>(currentNode), audioBufferManager));
}

void GraphModel::setSettings (Settings settings)
{
    this->settings = settings;
    audioBufferManager.setBlockSize (settings.blockSize);
    //TODO implement change listener
}

const Settings& GraphModel::getSettings () const
{
    return settings;
}

bool GraphModel::addListener (Listener* const newListener)
{
    assert (newListener != nullptr);

    const auto result = 
        std::find (std::begin (listeners), std::end (listeners), newListener);

    // Didn't find the listener so add it
    if (result == std::end (listeners))
    {
        listeners.push_back (newListener);
        return true;
    }

    return false;
}

bool GraphModel::removeListener (const Listener* listener)
{
    assert (listener != nullptr);

    const auto result =
        std::find (std::begin (listeners), std::end (listeners), listener);
    
    if (result != std::end (listeners))
    {
        listeners.erase (result);
        return true;
    }

    return false;
}
