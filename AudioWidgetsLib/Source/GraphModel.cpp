#include "GraphModel.h"
#include <stack>
#include <iostream>
#include <list>

GraphModel::GraphModel ()
    : audioBufferManager (settings.blockSize)
{
}

GraphModel::~GraphModel ()
{
    for (unsigned int i = 0; i < graphOps.size (); ++i)
        delete graphOps[i];
}

bool GraphModel::addNode (const NodeModel& newNode)
{
    nodes[newNode.getID ()] = newNode;
    return true;
}

bool GraphModel::removeNode (const NodeModel& node)
{
    auto iter = nodes.cbegin ();

    while (iter != nodes.end ())
    {
        if (iter->second.getID () == node.getID ())
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

bool GraphModel::addConnection (const Connection& newConnection)
{
    auto iter = nodes.begin ();

    while (iter != nodes.end ())
    {
        if (iter->second.getID () == newConnection.sourceNode)
        {
            iter->second.addDependentNode (newConnection.destNode);
            break;
        }
        ++iter;
    }

    return true;
}

bool GraphModel::removeConnection (const Connection& connection)
{
    auto iter = nodes.begin ();

    while (iter != nodes.end ())
    {
        if (iter->second.getID () == connection.sourceNode)
        {
            iter->second.removeDependentNode (connection.destNode);
            break;
        }
        ++iter;
    }

    return true;
}

int GraphModel::connectionCount () const
{
    int connectionCount = 0;
    auto iter = nodes.begin ();

    while (iter != nodes.end ())
    {
        connectionCount += iter->second.dependentNodeCount ();
        ++iter;
    }

    return connectionCount;
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
        visited[node.second.getID ().getNumber ()] = false;
    }

    // Call the recursive helper function to store Topological Sort
    // starting from all vertices one by one
    for (auto& node : nodes)
    {
        if (visited[node.second.getID ().getNumber ()] == false)
            topologicalSortUtil (NodeModel::Empty, node.second, visited);
    }
}

void GraphModel::processGraph ()
{
    for (auto& op : graphOps)
    {
        op->perform ();
    }
}

void GraphModel::topologicalSortUtil (const NodeModel& parentNode,
                                      const NodeModel& currentNode, 
                                      std::map<int, bool>& visited)
{
    const int currentNodeID = currentNode.getID ().getNumber ();

    // Mark the current node as visited.
    visited[currentNodeID] = true;

    // Recur for all the nodes adjacent to this node
    for (auto& adjacentNode : currentNode.getDependentNodes ())
    {
        if (!visited[adjacentNode.getNumber ()])
        {
            topologicalSortUtil (currentNode, nodes[adjacentNode.getNumber ()],
                                 visited);
        }
    }

    
    // If parent mode is 'empty' it means there is no incoming node
    if (parentNode == NodeModel::Empty)
    {
        graphOps.push_back (new ProcessNodeOp (AudioBufferManager::AudioBufferID::Empty, 
            audioBufferManager.getFreeBuffer (), settings.blockSize, const_cast<NodeModel&>(currentNode),
            audioBufferManager));
    }
    else
    {
        // This node depends on the parent node to be processed first, we need to get the buffer that 
        // contains the output of the parent node and use it as the input of this node
        graphOps.push_back (new ProcessNodeOp (AudioBufferManager::AudioBufferID::Empty,
            audioBufferManager.getFreeBuffer (), settings.blockSize, const_cast<NodeModel&>(currentNode),
            audioBufferManager));
    }
}

void GraphModel::setSettings (Settings settings)
{
    this->settings = settings;
    audioBufferManager.setBlockSize (settings.blockSize);
    //TODO implement change listener
}

GraphModel::Settings GraphModel::getSettings () const
{
    return settings;
}
