#include <stack>
#include <iostream>
#include <list>
#include <iterator>

#include "GraphModel.h"

GraphModel::GraphModel ()
    :   settings {44100.f, 50},
        audioBufferManager (50)
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
    nodes[newNode->getID ()] = newNode;
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
    for (auto& nodePair : nodes)
    {
        auto node = nodePair.second;

        if (node->getID () == newConnection.sourceNode)
        {
            node->addDependentNode (newConnection.destNode);
            return true;
        }
    }

    return false;
}

bool GraphModel::addConnection (const NodeModel* const srcNode, 
                                const NodeModel* const destNode)
{
    for (auto& nodePair : nodes)
    {
        auto node = nodePair.second;

        if (node->getID () == srcNode->getID ())
        {
            node->addDependentNode (destNode->getID ());
            return true;
        }
    }

    return false;
}

bool GraphModel::removeConnection (const Connection& connection)
{
    for (auto& nodePair : nodes)
    {
        const auto node = nodePair.second;

        if (node->getID () == connection.sourceNode)
        {
            node->removeDependentNode (connection.destNode);
            return true;
        }
    }

    return false;
}

int GraphModel::connectionCount () const
{
    int connectionCount = 0;

    for (auto& node : nodes)
    {
        connectionCount += node.second->dependentNodeCount ();
    }

    return connectionCount;
}

bool GraphModel::connectionExists (const Connection& testConnection) const
{
    // TODO implement connection checking
    return false;
}

bool GraphModel::canConnect (const Connection& testConnection) const
{
    // TODO implement if one node and connect to another
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
    for (auto& adjacentNode : currentNode.getDependentNodes ())
    {
        if (!visited[adjacentNode.getNumber ()])
        {
            topologicalSortUtil (currentNode, *nodes[adjacentNode.getNumber ()],
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
