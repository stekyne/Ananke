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
    int dbg (const char* format, ...) { return 0; }
#endif

Graph::Graph () :
	settings(44100.f, 128, 64, 0, 2),
	audioBufferManager (128),
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
		// FIXME not a great solution
		if (node->getID () == AudioInputID || node->getID () == AudioOutputID)
			continue;

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
        listener->newNodeAdded (*newNode);

    return true;
}

bool Graph::removeNode (const Node* const node)
{
    if (node == nullptr)
        return false;

    // Check if node exists in container
    if (getNodeForID (node->getID ()) == nullptr)
        return false;

	// TODO can't remove fixed nodes (audio in/out, midi in/out)

    clearConnectionsForNode (node->getID ());

    nodes.erase (
        std::remove_if (std::begin (nodes), std::end (nodes), [&](const Node* n) { 
            return n->getID () == node->getID (); }),
        std::end (nodes));

    for (auto& listener : listeners)
        listener->nodeRemoved (*node);

    return true;
}

int Graph::nodeCount () const
{
    return (int)nodes.size ();
}

Node* const Graph::getNodeForID (int nodeId)
{
    const auto result = std::find_if (std::cbegin (nodes), std::cend (nodes), 
		[&](Node* n) { return n->getID () == nodeId; });

    if (result != std::cend (nodes))
        return *result;

    return nullptr;
}

bool Graph::addConnection (const Connection newConnection)
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
	// TODO call rebuild graph?
    return false;
}

bool Graph::addConnection (std::vector<Connection>& newConnections)
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
        for (auto& connection : newConnections)
            listener->newConnectionAdded (connection);

        return true;
    }

	// TODO remove invalid connections on failure?
	// TODO rebuild graph?

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
    auto iterator = std::remove (std::begin (connections), std::end (connections), connection);

    // Found the connection so remove it and update listeners
    if (iterator != std::end (connections))
    {
        connections.erase (iterator);

        for (auto& listener : listeners)
            listener->connectionRemoved (connection);

        // TODO rebuild graph?
        return true;
    }

    return false;
}

bool Graph::removeAnyConnection (const int nodeId, const int channelIndex)
{
    auto connectionMatcher = [&](const Connection& conn) {
        return conn.destNode == nodeId && conn.destChannel == channelIndex ||
               conn.sourceNode == nodeId && conn.sourceChannel == channelIndex;
    };

    auto foundElems = std::find_if (std::begin (connections), std::end (connections), connectionMatcher);
	auto iterator = std::remove_if (std::begin (connections), std::end (connections), connectionMatcher);

    // Notify listeners of deletion first
    while (foundElems != std::end (connections)) {
        // TODO add generic graph configuration has changed listener
        for (auto& listener : listeners)
            listener->connectionRemoved (*foundElems);
    }

    // Found one or more connections matching the criteria, delete and update listeners
	if (iterator != std::end (connections))
	{
		connections.erase (iterator);
        
        // TODO rebuild graph?
        return true;
	}

    return false;
}

int Graph::connectionCount () const
{
    return (int)connections.size ();
}

bool Graph::connectionExists (const Connection& testConnection) const
{
    return std::find (std::cbegin (connections), std::cend (connections), testConnection) != std::cend (connections);
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
        /*if (testConnection.sourceNode == AudioInputID)
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
        }*/

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
    return !(srcMatched == false || dstMatched == false);
}

void Graph::clearGraph ()
{
    connections.clear ();
    nodes.clear ();
    graphOps.clear ();
}

bool Graph::buildGraph ()
{
	if (suspendRebuilding)
	{
		dbg ("Suspending rebuild of graph");
		return false;
	}

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
			AudioBufferID bufferId (node->getID (), j);
            auto buffer = audioBufferManager.registerFreeBufferWithID (bufferId);
            nodeOutputBuffers.push_back (buffer);
        }

        // Find all incoming buffers required by this node
        BufferArray nodeInputBuffers;

        if (node->getNumInputChannels () > 0)
        {
            for (auto nodeChan = 0; nodeChan < node->getNumOutputChannels (); ++nodeChan)
            {
				BufferArray buffers;

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
                    auto freeBuffer = audioBufferManager.registerFreeBufferWithID (AudioBufferID (99, 0));
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
    
	// Check for audio input / output changes
    if (setIONodeBuffers (numAudioInputs, numAudioOutputs))
        hasUpdated = true;

    // Check if graph Ops have been recalculated and swap if so
    if (hasGraphOpsChanged)
    {
        // TODO swap pointer to new graph ops
        // TODO need to delete old graph ops on seperate thread
        // graphOps = 
    }

	// Copy incoming audio into graph buffers
	for (int i = 0; i < numAudioInputs; ++i)
	{
		const auto inputBuffer = audioBufferManager.getBufferFromID (AudioBufferID (AudioInputID, i));
		assert (inputBuffer != nullptr);
		assert (blockSize <= inputBuffer->getSize ());
		inputBuffer->copyDataFrom (audioIn[i], blockSize);
	}

    for (auto& op : graphOps)
        op->perform (blockSize);

	// Copy processed audio from graph buffers to audio output
	for (int i = 0; i < numAudioOutputs; ++i)
	{
		const auto outputBuffer = audioBufferManager.getBufferFromID (AudioBufferID (AudioOutputID, i));
		assert (outputBuffer != nullptr);
		assert (blockSize <= outputBuffer->getSize ());
		//outputBuffer->copyDataTo (audioOut[i], blockSize);
	}

    return hasUpdated;
}

bool Graph::setIONodeBuffers (int inputChannels,  int outputChannels)
{
    bool hasChanged = false;

    // If number of inputs or outputs has changed then we need to re-build the graph to remove
    // any now invalid connections
    if (settings.numberInputChannels != inputChannels)
    {
		// Remove any now invalid connections
		for (int i = inputChannels; i < settings.numberInputChannels; ++i)
			removeAnyConnection (AudioInputID, i);
		
        settings.numberInputChannels = inputChannels;
		audioInNode.setChannelCount (settings.numberInputChannels);
        audioBufferManager.markFreeByID (Graph::AudioInputID);

        // Add new buffers to audio manager
        for (auto chan = 0; chan < inputChannels; ++chan)
        {
            // This will register a buffer with the appropriate ID
            auto freeBuffer = audioBufferManager.registerFreeBufferWithID (AudioBufferID (Graph::AudioInputID, chan));
            assert (freeBuffer != nullptr);
        }
        
        hasChanged = true;
    }

	if (settings.numberOutputChannels != outputChannels)
	{
		// Remove any now invalid connections
		for (int i = outputChannels; i < settings.numberOutputChannels; ++i)
			removeAnyConnection (AudioOutputID, i);

		settings.numberOutputChannels = outputChannels;
		audioOutNode.setChannelCount (outputChannels);
		audioBufferManager.markFreeByID (Graph::AudioOutputID);

		// Add new buffers to audio manager
		for (auto chan = 0; chan < outputChannels; ++chan)
		{
			// This will register a buffer with the appropriate ID
			auto freeBuffer = audioBufferManager.registerFreeBufferWithID (AudioBufferID (Graph::AudioOutputID, chan));
			assert (freeBuffer != nullptr);
		}

		hasChanged = true;
	}

    if (hasChanged)
        buildGraph ();

    return hasChanged;
}

bool Graph::isNodeConnected (Node& node)
{
    for (auto& connection : connections)
        if (connection.destNode == node.getID () || connection.sourceNode == node.getID ())
            return true;

    return false;
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

        if (isNodeConnected(*currentNode))
        {
            sortedNodes.push_back (currentNode);
            dbg ("Node: %d, no more adjacent nodes \n", currentNodeID);
            dbg ("Node: %d, added to sorted list \n", currentNodeID);
        }
        else 
        {
            dbg ("Node: %d, unconnected node so not adding to sorted list \n", currentNodeID);
        }
        
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

void Graph::setSettings (Settings newSettings)
{
    this->settings = newSettings;

	// Update audio input/output configuration if changed
	audioInNode.setChannelCount (newSettings.numberInputChannels);
	audioOutNode.setChannelCount (newSettings.numberOutputChannels);

    // Re-allocate buffers if size has changed
    audioBufferManager.setBufferSize (newSettings.blockSize);
    
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

    // TODO lock listeners list
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

std::vector<int> Graph::getDependentsForNode (int nodeId)
{
    std::vector<int> dependentVec;

    for (const auto& connection : connections)
    {
        if (connection.sourceNode != nodeId)
            continue;

        if (std::find (std::cbegin (dependentVec), std::cend (dependentVec),
				connection.destNode) == std::end (dependentVec))
        {
            dependentVec.push_back (connection.destNode);
        }
    }

    return dependentVec;
}

void Graph::clearConnectionsForNode (int nodeId)
{
    auto result = std::remove_if (std::begin (connections), std::end (connections), 
        [&](const Connection& conn) { 
            return conn.sourceNode == nodeId || conn.destNode == nodeId;
    });
    
    connections.erase (result, std::end (connections));
}

}

