#include <stack>
#include <iostream>
#include <list>
#include <iterator>
#include <algorithm>

#include "GraphModel.h"

#ifdef _DEBUG
# include <Windows.h>

int dbg (const char* format, ...)
{
    char buffer[512];
    int _Result;
    va_list _ArgList;
    va_start (_ArgList, format);
    #pragma warning(push)
    #pragma warning(disable: 4996) // Deprecation
    _Result = _vsprintf_l (buffer, format, NULL, _ArgList);
    #pragma warning(pop)
    va_end (_ArgList);
    OutputDebugString (buffer);
    return _Result;
}
#else
int dbg (const char* format, ...) {}
#endif

GraphModel::GraphModel ()
{
    addFixedNodes ();
}

GraphModel::GraphModel (Settings settings)  :   
    settings (settings),
    audioBufferManager (settings.blockSize)
{
    addFixedNodes ();
}

GraphModel::~GraphModel ()
{
    for (unsigned int i = 0; i < graphOps.size (); ++i)
    {
        if (graphOps[i] != nullptr)
            delete graphOps[i];
    }

    for (auto& node : nodes)
    {
        if (node.second != nullptr)
        {
            if (node.second->getID () != NodeModel::Empty.getID ())
            {
                delete node.second;
            }
        }
    }
}

bool GraphModel::addNode (NodeModel* const newNode)
{
    if (newNode == nullptr)
        return false;

    newNode->setID (getFreeInternalID ());
    nodes[newNode->getID ()] = newNode;
    
    for (auto& listener : listeners)
        listener->newNodeAdded ();

    return true;
}

bool GraphModel::addNodeWithID (NodeModel* const newNode, uint32_t id)
{
    if (newNode == nullptr)
        return false;

    newNode->setID (id);
    nodes[newNode->getID ()] = newNode;

    for (auto& listener : listeners)
        listener->newNodeAdded ();

    return true;
}

bool GraphModel::removeNode (const NodeModel* const node)
{
    if (node == nullptr)
        return false;

    bool isNodeFound = false;
    auto iter = nodes.cbegin ();

    while (iter != nodes.end ())
    {
        auto itrNode = iter->second;
        if (itrNode->getID () == node->getID ())
        {
            isNodeFound = true;
            nodes.erase (iter);
            clearConnectionsForNode (node->getID ());
            break;
        }
        ++iter;
    }

    for (auto& listener : listeners)
        listener->nodeRemoved ();

    return isNodeFound;
}

int GraphModel::nodeCount () const
{
    // Subtract the 'empty' node
    return nodes.size () - 1;
}

const GraphModel::NodeMap& GraphModel::getNodes () const
{
    return nodes;
}

// TODO return const, needs to be mutable currently to set position
NodeModel* const GraphModel::getNodeForID (int id)
{
    return nodes[id];
}

bool GraphModel::addConnection (const Connection& newConnection)
{
    connections.push_back (newConnection);

    for (auto& listener : listeners)
        listener->newConnectionAdded ();

    return true;
}

bool GraphModel::addConnection (const NodeModel& sourceNode, uint32_t sourceChannel, 
                                const NodeModel& destNode, uint32_t destChannel)
{
    connections.push_back (Connection (sourceNode.getID (), sourceChannel,
                                        destNode.getID (), destChannel));

    for (auto& listener : listeners)
        listener->newConnectionAdded ();

    return true;
}

bool GraphModel::removeConnection (const Connection& connection)
{
    auto result = 
        std::remove (connections.begin (), connections.end (), connection);
    connections.erase (result, connections.end ());

    for (auto& listener : listeners)
        listener->connectionRemoved ();

    return true;
}

int GraphModel::connectionCount () const
{
    return connections.size ();
}

bool GraphModel::connectionExists (const Connection& testConnection) const
{
    return std::find (connections.cbegin (), connections.cend (), 
                      testConnection) != connections.cend ();
}

bool GraphModel::canConnect (const Connection& testConnection)
{
    if (!validateConnection (testConnection) == true)
        return false;

    // Add connection and perform sort, if loop is detected return false
    // Remove connection either way
    addConnection (testConnection);

    std::vector<NodeModel*> sortedNodes;
    sortedNodes.reserve (nodes.size ());
    const auto result = performSort (sortedNodes);
    removeConnection (testConnection);
    return result;
}

bool GraphModel::validateConnection (const Connection& testConnection)
{
    // Connection can't point to itself
    if (testConnection.destNode == testConnection.sourceNode)
        return false;

    bool srcMatched = false, dstMatched = false;

    for (const auto& node : nodes)
    {
        if (testConnection.sourceNode == node.first)
        {
            srcMatched = true;
            if (dstMatched) break;
        }
        if (testConnection.destNode == node.first)
        {
            dstMatched = true;
            if (srcMatched) break;
        }
    }

    // Source or destination node does not exist
    if (srcMatched == false || dstMatched == false)
        return false;

    // TODO check channels are valid for each node?
    
    return true;
}

const std::vector<Connection>& GraphModel::getConnections () const
{
    return connections;
}

bool GraphModel::initialise ()
{
    // TODO 
    return true;
}

void GraphModel::clearGraph ()
{
    nodes.clear ();
    graphOps.clear ();
    addFixedNodes ();
}

bool GraphModel::buildGraph ()
{
    std::vector<NodeModel*> sortedNodes;
    sortedNodes.reserve (nodes.size ());
    const auto result = performSort (sortedNodes);

    // Graph contained loops so cannot continue
    if (result == false)
        return false;

    for (int i = sortedNodes.size (); --i >= 0;)
    {
        auto node = sortedNodes[i];

        // If parent node is 'empty' it means there is no incoming node
        const auto parentBufferID = (node->getParentID () == NodeModel::Empty) ? nullptr 
            : audioBufferManager.getAssociatedBufferForNodeOutput (node->getParentID (), 0);

        // Associate this node's output with free buffers
        OutputBufArray outputBuffers;
        for (auto i = 0u; i < node->getNumOutputChannels (); ++i)
        {
            auto freeBuffer = audioBufferManager.getFreeBuffer ();
            freeBuffer->setID (AudioBufferID (node->getID (), i));
            outputBuffers.push_back (freeBuffer);
        }

        // Find all incoming buffers required by this node
        InputBufArray inputBufers;
        if (node->getNumInputChannels () > 0)
        {
            for (auto& connection : connections)
            {
                if (connection.destNode == node->getID ())
                {
                    auto buffer =
                        audioBufferManager.getBufferFromID (AudioBufferID (connection.sourceNode,
                                                                           connection.sourceChannel));
                    assert (buffer != nullptr);
                    inputBufers.push_back (buffer);
                }
            }
        }

        // This node has no incoming audio if parentBufferId is null/empty
        if (parentBufferID == nullptr)
        {
            graphOps.push_back (new GeneratorNode (std::move(outputBuffers), node));
        }
        else
        {
            graphOps.push_back (new FilterNodeOp (std::move (inputBufers), 
                                                  std::move (outputBuffers), 
                                                  node));
        }
    }

    return true;
}

void GraphModel::processGraph (const float** audioIn, float** audioOut,
                               const uint32_t blockSize)
{
    setInputNodeBuffers (audioIn, 2, blockSize);
    setOutputNodeBuffers (audioOut, 2, blockSize);

    for (size_t i = 0u; i < graphOps.size (); ++i)
    {
        graphOps[i]->perform (blockSize);
    }
    
}

void GraphModel::setInputNodeBuffers (const float** const buffers, 
                                      uint32_t numChannels, uint32_t numSamples)
{
    // Find the node in the map and cache the reference
    if (inputNode == nullptr)
    {
        // Find input node if it exists
        auto inputNodeItr = nodes.find (NodeModel::InputNodeID);

        if (inputNodeItr != std::end (nodes))
        {
            auto inputNode = dynamic_cast<ExternalNode*> (inputNodeItr->second);
            
            if (inputNode != nullptr)
                inputNode->setExternalBuffers ((float**)buffers, numChannels, numSamples);
        }
    }
}

void GraphModel::setOutputNodeBuffers (float** const buffers, uint32_t numChannels, uint32_t numSamples)
{
    // Find the node in the map and cache the reference
    if (outputNode == nullptr)
    {
        // Find output node if it exists
        auto outputNodeItr = nodes.find (NodeModel::OutputNodeID);

        if (outputNodeItr != std::end (nodes))
        {
            auto outputNode = dynamic_cast<ExternalNode*> (outputNodeItr->second);
            
            if (outputNode != nullptr)
                outputNode->setExternalBuffers (buffers, numChannels, numSamples);
        }
    }
}

bool GraphModel::topologicalSortUtil (const NodeModel* parentNode, NodeModel* currentNode, 
                                      std::unordered_map<int, Markers>& visited, 
                                      std::vector<NodeModel*>& sortedNodes)
{
    const int currentNodeID = currentNode->getID ();

    // FIXME: if this is the first node to be processed, it won't have a parentNode. 
    // Not a strong enough design, needs to be redone
    if (currentNode->getParentID () != NodeModel::Empty.getID ())
        currentNode->setParentID (parentNode->getID ());

    // Check if current node has been visited (temporary marker) 
    // if so, there is a loop in the graph so just return false as it's not a DAG
    if (visited[currentNodeID].temporaryMark == true)
    {
        dbg ("Node: %d, already visited, not a DAG, must contain a loop \n", currentNodeID);
        return false;
    }

    if (visited[currentNodeID].permanentMark == false)
    {
        // Mark the current node as visited.
        visited[currentNodeID].temporaryMark = true;
        dbg ("Node: %d, adding temporary mark \n", currentNodeID);

        // Recur for all the nodes adjacent to this node
        for (auto& adjacentNode : getDependentsForNode (currentNodeID))
        {
            if (visited[adjacentNode].permanentMark == true)
            {
                // Node has been visited (permanently marked) already so skip
                dbg ("Node: %d, already visited so skipping \n", adjacentNode);
                
                // If parent node ID hasn't been set, add it now
                // FIXME: Not sure if this can cause problems
                auto* const nodeModel = nodes[adjacentNode];
                assert (nodeModel != nullptr);
                assert (nodeModel->getID () != NodeModel::Empty);
                nodeModel->setParentID (currentNodeID);
                continue;
            }

            auto* const nodeModel = nodes[adjacentNode];
            assert (nodeModel != nullptr);
            assert (nodeModel->getID () != NodeModel::Empty);

            dbg ("Node: %d, attempting to visit node: %d \n", currentNodeID, nodeModel->getID ());

            const auto result = 
                topologicalSortUtil (currentNode, nodeModel, visited, sortedNodes);

            if (result == false)
                return false;
        }

        visited[currentNodeID].permanentMark = true;
        visited[currentNodeID].temporaryMark = false;

        sortedNodes.push_back (currentNode);
        dbg ("Node %d, added to sorted list \n", currentNodeID);
        return true;
    }

    // Node has been visited (permanently marked) already so skip
    return true;
}

bool GraphModel::performSort (std::vector<NodeModel*>& sortedNodes)
{
    // Mark all the vertices as not visited
    std::unordered_map<int, Markers> visited;

    for (auto& node : nodes)
    {
        assert (node.second != nullptr);

        if (*node.second == NodeModel::Empty)
            continue;
        
        visited[node.second->getID ()].permanentMark = false;
        visited[node.second->getID ()].temporaryMark = false;
    }

    // Call the recursive helper function to store Topological Sort
    // starting from all vertices one by one
    for (const auto& node : nodes)
    {
        if (*node.second == NodeModel::Empty)
            continue;

        NodeModel* const nodeModel = node.second;

        if (visited[nodeModel->getID ()].permanentMark == false)
        {
            dbg ("Visiting node: %d \n", nodeModel->getID ());

            const bool result = 
                topologicalSortUtil (&NodeModel::Empty, nodeModel,
                                     visited, sortedNodes);

            if (result == false)
                return false;
        }
    }

    return true;
}

void GraphModel::setSettings (Settings settings)
{
    this->settings = settings;

    // Re-allocate buffers if size has changed
    audioBufferManager.setBlockSize (settings.blockSize);
    
    for (auto& listener : listeners)
        listener->graphSettingsChanged ();
}

const GraphModel::Settings& GraphModel::getSettings () const
{
    return settings;
}

std::string GraphModel::printGraph () const
{
    std::stringstream buffer;
    buffer << "Graph: Connections: " << std::endl;
    for (const auto& connection : connections)
    {
        buffer << "Src: (" << connection.sourceNode << ", " << connection.sourceChannel 
               << ") -> Dst: (" << connection.destNode << ", " << connection.destChannel << ")";
        buffer << std::endl;
    }
    buffer << std::endl << "Operations: " << std::endl;
    for (int x = 0; x < (int)graphOps.size (); ++x)
    {
        buffer << graphOps[x]->getName () << std::endl;
    }
    buffer << "____________________" << std::endl;
    return buffer.str ();
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

std::vector<uint32_t> GraphModel::getDependentsForNode (unsigned int nodeID)
{
    std::vector<uint32_t> dependentVec;

    for (const auto& connection : connections)
    {
        if (connection.sourceNode == nodeID)
        {
            if (std::find (std::cbegin (dependentVec), std::cend (dependentVec),
                           connection.destNode) == std::end (dependentVec))
            {
                dependentVec.push_back (connection.destNode);
            }
        }
    }

    return dependentVec;
}

void GraphModel::clearConnectionsForNode (unsigned int nodeID)
{
    auto result = std::remove_if (connections.begin (), connections.end (), 
        [&](const Connection& conn) { 
            return conn.sourceNode == nodeID || conn.destNode == nodeID;
    });
    
    if (result != connections.end ())
        connections.erase (result);
}

const std::vector<GraphOp*> GraphModel::getGraphOps () const
{
    return graphOps;
}

std::vector<int> GraphModel::getGraphOrderAsList () const
{
    std::vector<int> graphList;

    return graphList;
}

void GraphModel::addFixedNodes ()
{
    nodes[NodeModel::Empty.getID ()] = &NodeModel::Empty;
}