#include <stack>
#include <iostream>
#include <list>
#include <iterator>
#include <algorithm>
#include <sstream>

#include "Graph.h"

namespace Ananke {

#if defined (_DEBUG) && defined (_MSC_VER)
# include <Windows.h>
    int dbg (const char* format, ...)
    {
        char buffer[1024];
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

Graph::Graph () :
	audioInNode (AudioInputID),
	audioOutNode (AudioOutputID)
{
	addNode (&audioInNode);
	addNode (&audioOutNode);
}

Graph::Graph (Settings settings) :   
    settings (settings),
    audioBufferManager (settings.blockSize),
	audioInNode (AudioInputID),
	audioOutNode (AudioOutputID)
{
	addNode (&audioInNode);
	addNode (&audioOutNode);
	// TODO add midi nodes
}

Graph::~Graph ()
{
    for (auto i = 0; i < graphOps.size (); ++i)
    {
        if (graphOps[i] != nullptr)
        {
            delete graphOps[i];
            graphOps[i] = nullptr;
        }
    }

    for (auto& node : nodes)
    {
        if (node != nullptr)
        {
            delete node;
            node = nullptr;
        }
    }
}

bool Graph::isEmpty () const
{
    return nodes.size () == 0 && connections.size () == 0;
}

bool Graph::addNode (Node* const newNode)
{
    if (newNode == nullptr)
        return false;

    // New Node must have an ID set
    if (newNode->getID () == 0)
        return false;

    // New Node must have an ID not already in use
    if (getNodeForID (newNode->getID ()) != nullptr)
        return false;

    nodes.push_back (newNode);
    
    for (auto& listener : listeners)
        listener->newNodeAdded (newNode);

    return true;
}

bool Graph::removeNode (const Node* const node)
{
    if (node == nullptr)
        return false;

    // Check if node exists in container
    if (getNodeForID (node->getID ()) == nullptr)
        return false;

    clearConnectionsForNode (node->getID ());

    nodes.erase (
        std::remove_if (std::begin (nodes), std::end (nodes),
                        [&](Node* n) { return n->getID () == node->getID (); }),
        std::end (nodes));

    for (auto& listener : listeners)
        listener->nodeRemoved (node);

    return true;
}

int Graph::nodeCount () const
{
    return (int)nodes.size ();
}

Node* const Graph::getNodeForID (int id)
{
	// TODO need to find graph fixed nodes, audio in/out, midi
	if (id == AudioOutputID)
		return &audioOutNode;

	if (id == AudioInputID)
		return &audioInNode;

	// TODO add midi nodes

    const auto result = std::find_if (std::cbegin (nodes), std::cend (nodes), 
		[&](Node* n) { return n->getID () == id; });

    if (result != std::cend (nodes))
        return *result;

    return nullptr;
}

bool Graph::addConnection (const Connection& newConnection)
{
    if (connectionExists (newConnection))
        return false;

    if (!isValidNewConnection (newConnection))
        return false;

    connections.push_back (newConnection);

    std::vector<Node*> sortedNodes;
    sortedNodes.reserve (nodes.size ());
    const auto result = performSort (sortedNodes);

    // Connection doesn't cause a loop
    if (result == true)
    {
        for (auto& listener : listeners)
            listener->newConnectionAdded (newConnection);

        return true;
    }

    // Connection isn't valid
    removeConnection (newConnection);
    return false;
}

bool Graph::addConnection (std::vector<Connection> newConnections)
{
    for (auto newConnection : newConnections)
    {
        if (connectionExists (newConnection))
            return false;

        if (!isValidNewConnection (newConnection))
            return false;

        connections.push_back (newConnection);
    }

    std::vector<Node*> sortedNodes;
    sortedNodes.reserve (nodes.size ());
    const auto result = performSort (sortedNodes);

    // Connection doesn't cause a loop
    if (result == true)
    {
        for (auto& listener : listeners)
            listener->newConnectionAdded (newConnections);

        return true;
    }

    return false;
}

bool Graph::canConnect (const Connection& newConnection)
{
    const auto canConnect = addConnection (newConnection);
    removeConnection (newConnection);
    return canConnect;
}

bool Graph::removeConnection (const Connection& connection)
{
    auto iterator = std::remove (connections.begin (), connections.end (), connection);

    if (iterator != std::end (connections))
    {
        connections.erase (iterator);
    }
    else
    {
        // Nothing to delete, not found
        return false;
    }

    for (auto& listener : listeners)
        listener->connectionRemoved (connection);

    return true;
}

int Graph::connectionCount () const
{
    return (int)connections.size ();
}

bool Graph::connectionExists (const Connection& testConnection) const
{
    return std::find (connections.cbegin (), connections.cend (), 
		testConnection) != connections.cend ();
}

bool Graph::isValidNewConnection (const Connection& testConnection) const
{
    // Connection can't point to itself
    if (testConnection.destNode == testConnection.sourceNode)
        return false;

    // Check that each node mentioned in the test connection exists
    bool srcMatched = false;
    bool dstMatched = false;

    for (const auto& node : nodes)
    {
        // Check if connection is refering to graph input / output 
        if (testConnection.sourceNode == AudioInputID)
        {
            if (testConnection.sourceChannel >= settings.numberInputChannels)
                return false;

            srcMatched = true;

            if (dstMatched)
                break;
        }
        else if (testConnection.sourceNode == AudioOutputID)        
        {
            if (testConnection.sourceChannel >= settings.numberOutputChannels)
                return false;

            srcMatched = true;

            if (dstMatched)
                break;
        }

        if (testConnection.destNode == AudioInputID)
        {
            if (testConnection.destChannel >= settings.numberInputChannels)
                return false;

            dstMatched = true;

            if (srcMatched)
                break;
        }
        else if (testConnection.destNode == AudioOutputID)
        {
            if (testConnection.destChannel >= settings.numberOutputChannels)
                return false;
            
            dstMatched = true;

            if (srcMatched)
                break;
        }

        if (testConnection.sourceNode == node->getID ())
        {
            // Check that channel is valid
            if (testConnection.sourceChannel >= node->getNumOutputChannels ())
                return false;

            srcMatched = true;

            if (dstMatched) 
                break;
        }

        if (testConnection.destNode == node->getID ())
        {
            // Check that channel is valid
            if (testConnection.destChannel >= node->getNumInputChannels ())
                return false;

            dstMatched = true;

            if (srcMatched) 
                break;
        }
    }

    // Source or destination node does not exist
    if (srcMatched == false || dstMatched == false)
        return false;

    return true;
}

void Graph::clearGraph ()
{
    nodes.clear ();
    graphOps.clear ();
}

bool Graph::buildGraph ()
{
    std::vector<Node*> sortedNodes;
    sortedNodes.reserve (nodes.size ());
    graphOps.clear ();

    // Calculate node dependency order via topological sort
    const auto result = performSort (sortedNodes);

    // Graph contained loops so cannot continue
    if (result == false)
        return false;

    for (auto i = (int)sortedNodes.size (); --i >= 0;)
    {
        auto& node = sortedNodes[i];
        assert (node != nullptr);

        // Associate this node's output with free buffers
        BufferArray nodeOutputBuffers;

        for (auto j = 0; j < node->getNumOutputChannels (); ++j)
        {
            auto freeBuffer = audioBufferManager.getFreeBuffer (AudioBufferID (node->getID (), j));
            nodeOutputBuffers.push_back (freeBuffer);
        }

        // Find all incoming buffers required by this node
        ReadOnlyBufferArray nodeInputBuffers;

        if (node->getNumInputChannels () > 0)
        {
            for (auto nodeChan = 0; nodeChan < node->getNumOutputChannels (); ++nodeChan)
            {
                std::vector<AudioBuffer<DSP::SampleType>*> buffers;

                for (auto& connection : connections)
                {
                    if (connection.destNode == node->getID () && connection.destChannel == nodeChan)
                    {
                        auto bufferPtr = audioBufferManager.getBufferFromID (
							AudioBufferID (connection.sourceNode, connection.sourceChannel));

                        assert (bufferPtr != nullptr);
                        buffers.push_back (bufferPtr);
                    }
                }

                // If buffers size is 0, no connections are feeding into this node's channel
                // If buffers size is 1, we have a single connection only
                // If buffers size is >1, there are multiple channels incoming and need to be summed
                if (buffers.size () == 1)
                {
                    nodeInputBuffers.push_back (buffers[0]);
                }
                else if (buffers.size () > 1)
                {
                    // TODO lookup existing buffer IDs to see if this sum op already exists
                    // How to store sum buffer IDs?
                    auto freeBuffer = audioBufferManager.getFreeBuffer (AudioBufferID (99, 0));
                    graphOps.push_back (new SumBuffersOp (std::move (buffers), freeBuffer));
                    nodeInputBuffers.push_back (freeBuffer);
                }
            }
        }

        // Bind new graph operation to process node with incoming/outgoing buffers
        // and node processing algorithm
        graphOps.push_back (new ProcessNodeOp (std::move (nodeInputBuffers), std::move (nodeOutputBuffers), node));
    }

#ifdef _DEBUG
    dbg (printGraph ().c_str ());
#endif

    return true;
}

bool Graph::processGraph (const float** audioIn, const int numAudioInputs,
                          float** audioOut, const int numAudioOutputs,
                          const int blockSize)
{
    assert (audioIn != nullptr);
    assert (audioOut != nullptr);
    assert (blockSize > 0);

    bool hasUpdated = false;

    // Buffer size has changed since last callback, need to reallocate buffers if larger
    if (audioBufferManager.getBufferSize () != blockSize)
    {
        audioBufferManager.setBufferSize (blockSize);
        settings.blockSize = blockSize;

        // TODO is this really needed? should be asynchronous at least
        for (auto& listener : listeners)
            listener->graphSettingsChanged ();

        hasUpdated = true;
    }
    
    // Hook up external buffers to graph input and output buffers
    //if (setIONodeBuffers (audioIn, numAudioInputs,
    //                      audioOut, numAudioOutputs, blockSize))
    //{
    //    hasUpdated = true;
    //}

    // Check if graph Ops have been recalculated and swap if so
    if (hasGraphOpsChanged)
    {
        // TODO swap pointer to new graph ops
        // TODO need to delete old graph ops on seperate thread
        // graphOps = 
    }

	// TODO process read only input buffers which copies audio to internal buffers to be used by graph
	// audioInNode.process()

    //for (auto& op : graphOps)
    //    op->perform (blockSize);

	// TODO finally collect all internal audio processing and output to external buffers via output node
	// audioOutNode.process ();

    return hasUpdated;
}

bool Graph::setIONodeBuffers (const float** const inBuffers, int inputChannels,
                              float** const outBuffers, int outputChannels,
                              int numSamples)
{
    bool hasChanged = false;

    // If number of inputs has changed then we need to re-build the graph to remove
    // any now invalid connections
    if (settings.numberInputChannels != inputChannels)
    {
        settings.numberInputChannels = inputChannels;
        
        // Delete old audio buffers registered in audio manager instance
        audioBufferManager.markFreeByID (Graph::AudioInputID);

        // Add new buffers to audio manager
        for (auto chan = 0; chan < inputChannels; ++chan)
        {
            // This will register a buffer with the appropriate ID
            auto freeBuffer = audioBufferManager.getFreeBuffer (AudioBufferID (Graph::AudioInputID, chan));
            assert (freeBuffer != nullptr);
        }
        
        hasChanged = true;
    }

    // Assign incoming buffer pointers to graph buffers
    for (auto chan = 0; chan < inputChannels; ++chan)
    {
        // Map incoming buffer pointers to existing IO buffers
        auto buffer = audioBufferManager.getBufferFromID (AudioBufferID (Graph::AudioInputID, chan));
        assert (buffer != nullptr);
        buffer->setBufferToUse (inBuffers[chan], numSamples);
    }

    if (settings.numberOutputChannels != outputChannels)
    {
        settings.numberOutputChannels = outputChannels;
        audioBufferManager.markFreeByID (Graph::AudioOutputID);

        // Add new buffers to audio manager
        for (auto chan = 0; chan < outputChannels; ++chan)
        {
            // This will register a buffer with the appropriate ID
            auto freeBuffer = audioBufferManager.getFreeBuffer (AudioBufferID (Graph::AudioOutputID, chan));
            assert (freeBuffer != nullptr);
        }

        hasChanged = true;
    }

    // Assign incoming buffer pointers to graph buffers
    for (auto chan = 0; chan < outputChannels; ++chan)
    {
        auto buffer = audioBufferManager.getBufferFromID (AudioBufferID (Graph::AudioOutputID, chan));
        assert (buffer != nullptr);
        buffer->setBufferToUse (outBuffers[chan], numSamples);
    }

    if (hasChanged)
        buildGraph ();

    return hasChanged;
}

bool Graph::topologicalSortUtil (Node* currentNode, std::unordered_map<int, Markers>& visited, 
	std::vector<Node*>& sortedNodes)
{
    const int currentNodeID = currentNode->getID ();

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
        for (const auto& adjacentNodeID : getDependentsForNode (currentNodeID))
        {
            if (visited[adjacentNodeID].permanentMark == true)
            {
                // Node has been visited (permanently marked) already so skip
                dbg ("Node: %d, already visited so skipping \n", adjacentNodeID);
                continue;
            }

            auto adjacentNode = getNodeForID (adjacentNodeID);
            assert (adjacentNode != nullptr);

            dbg ("Node: %d, attempting to visit node: %d \n", 
                 currentNodeID, adjacentNode->getID ());

            const auto result = topologicalSortUtil (adjacentNode, visited, sortedNodes);

            if (result == false)
                return false;
        }

        visited[currentNodeID].permanentMark = true;
        visited[currentNodeID].temporaryMark = false;

        sortedNodes.push_back (currentNode);
        dbg ("Node: %d, No more adjacent nodes \n", currentNodeID);
        dbg ("Node: %d, added to sorted list \n", currentNodeID);
        return true;
    }

    // Node has been visited (permanently marked) already so skip
    return true;
}

bool Graph::performSort (std::vector<Node*>& sortedNodes)
{
    // Mark all the vertices as not visited
    std::unordered_map<int, Markers> visited;

    for (auto& node : nodes)
    {
        assert (node != nullptr);
        visited[node->getID ()].permanentMark = false;
        visited[node->getID ()].temporaryMark = false;
    }

    dbg ("----- Begin Graph Sort ------\n");

    // Call the recursive helper function to store Topological Sort
    // starting from all vertices one by one
    for (const auto& node : nodes)
    {
        assert (node != nullptr);

        // Traverse nodes that haven't been already mapped out and 
        // which don't have any incoming node input
        if (visited[node->getID ()].permanentMark == false)
        {
            dbg ("Visiting node: %d \n", node->getID ());

            if (!topologicalSortUtil (node, visited, sortedNodes))
            {
                dbg ("----- End Graph Sort -----\n");
                return false;
            }
        }
    }

    dbg ("----- End Graph Sort -----\n");
    return true;
}

void Graph::setSettings (Settings _settings)
{
    this->settings = _settings;

    // Re-allocate buffers if size has changed
    audioBufferManager.setBufferSize (_settings.blockSize);
    
    for (auto& listener : listeners)
        listener->graphSettingsChanged ();
}

const Graph::Settings& Graph::getSettings () const
{
    return settings;
}

std::string Graph::printGraph () const
{
    std::stringstream buffer;
    buffer << "____________________" << std::endl;
    buffer << "Graph Connections: " << std::endl;

    for (const auto& connection : connections)
    {
        buffer << "Src: (" << connection.sourceNode << ", " << connection.sourceChannel 
               << ") -> Dst: (" << connection.destNode << ", " << connection.destChannel << ")";
        buffer << std::endl;
    }

    buffer << std::endl << "Operations: " << std::endl;

    for (size_t x = 0; x < graphOps.size (); ++x)
    {
        buffer << graphOps[x]->getName () << std::endl;
    }

    buffer << "____________________" << std::endl;
    return buffer.str ();
}

bool Graph::addListener (Listener* const newListener)
{
    assert (newListener != nullptr);

    const auto result = std::find (std::cbegin (listeners), std::cend (listeners), newListener);

    // Didn't find the listener so add it
    if (result == std::cend (listeners))
    {
        listeners.push_back (newListener);
        return true;
    }

    return false;
}

bool Graph::removeListener (const Listener* listener)
{
    assert (listener != nullptr);

	const auto result = std::find (std::cbegin (listeners), std::cend (listeners), listener);
    
    if (result != std::cend (listeners))
    {
        listeners.erase (result);
        return true;
    }

    return false;
}

std::vector<int> Graph::getDependentsForNode (int nodeID)
{
    std::vector<int> dependentVec;

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

void Graph::clearConnectionsForNode (int nodeID)
{
    auto result = std::remove_if (connections.begin (), connections.end (), 
        [&](const Connection& conn) { 
            return conn.sourceNode == nodeID || conn.destNode == nodeID;
    });
    
    connections.erase (result, connections.end ());
}

}

