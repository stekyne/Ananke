#include "..\src\Graph.h"
#include "..\src\Node.h"
#include "..\src\Nodes.h"

#include "Helpers\TestNodes.h"

#include "catch.hpp"

namespace AudioWidgetsTests
{
    TEST_CASE ("Graph usage testing")
    {
        // Create a new graph free of nodes and connections
        Graph graph;
        REQUIRE (graph.isEmpty ());

        SECTION ("Add nodes to graph")
        {
            auto node1 = new InputOutputNode (1);
            auto node2 = new InputOutputNode (2);
            auto node3 = new InputOutputNode (3);
            auto node4 = new InputOutputNode (4);
            graph.addNode (node1);
            graph.addNode (node2);
            graph.addNode (node3);
            graph.addNode (node4);
            const auto NodeCount = 4;
            REQUIRE (graph.nodeCount() == NodeCount);

            SECTION ("Adding Nodes with default ID should be rejected")
            {
                auto node5 = std::make_unique<Node> ();
                REQUIRE (!graph.addNode (node5.get ()));
            }

            SECTION ("Adding Nodes with duplicate ID's should be rejected")
            {
                auto node1Dup = std::make_unique<Node> (1);
                REQUIRE (!graph.addNode (node1Dup.get ()));
            }

            SECTION ("Retrieve nodes by ID from graph")
            {
                REQUIRE (graph.getNodeForID (1)->getID () == 1);
                REQUIRE (graph.getNodeForID (3)->getID () == 3);
                REQUIRE (graph.getNodeForID (99) == nullptr);
            }

            SECTION ("Add connections between nodes")
            {
                Connection connection1 (node1, 0, node2, 0);
                Connection connection2 (node2, 0, node3, 0);
                Connection connection3 (node1, 0, node3, 0);
                graph.addConnection (connection1);
                graph.addConnection (connection2);
                graph.addConnection (connection3);
                const auto ConnectionCount = 3;
                REQUIRE (graph.connectionCount () == ConnectionCount);
                REQUIRE (graph.getConnections ().size () == ConnectionCount);

                SECTION ("Addition of invalid or existing connection should be ignored and rejected")
                {
                    REQUIRE_FALSE (graph.addConnection (Connection (55, 0, 66, 0)));
                    REQUIRE_FALSE (graph.addConnection (Connection (node3, 0, node1, 0)));
                }

                SECTION ("Check connection exists in graph")
                {
                    // Connection should not exist
                    REQUIRE (!graph.connectionExists (Connection (75, 0, 88, 0)));

                    // Connection should exist
                    REQUIRE (graph.connectionExists (connection1));
                }

                SECTION ("Check can connect with new connection")
                {
                    // Connection would create a loop, invalid
                    Connection testConnection (3, 0, 1, 0);
                    REQUIRE_FALSE (graph.addConnection (testConnection));

                    // Connection already exists, invalid
                    Connection testConnection3 (1, 0, 3, 0);
                    REQUIRE_FALSE (graph.addConnection (testConnection3));

                    // Valid new connection
                    Connection testConnection2 (3, 0, 4, 0);
                    REQUIRE (graph.addConnection (testConnection2));
                }

                SECTION ("Validate that a connection exists ")
                {
                    // Invalid nodes specified in connection
                    REQUIRE_FALSE (graph.isValidNewConnection (Connection (52, 0, 22, 0)));

                    // Invalid channel
                    REQUIRE_FALSE (graph.isValidNewConnection (Connection (1, 9229, 3, 0)));

                    // Invalid channel, exceeds node's max output (2)
                    REQUIRE_FALSE (graph.isValidNewConnection (Connection (1, 2, 3, 0)));

                    // Valid connection
                    REQUIRE (graph.isValidNewConnection (Connection (1, 0, 3, 0)));
                }

                SECTION ("Build valid graph successfully")
                {
                    REQUIRE (graph.buildGraph ());
                }

                SECTION ("Process graph, audio settings change")
                {
                    const auto BlockSize1 = 10;
                    const auto BlockSize2 = 20;
                    const auto InputChans = 2;

                    const float** inputArray = new const float*[InputChans];
                    for (int i = 0; i < InputChans; ++i) 
                        inputArray[i] = new float[BlockSize1];

                    const auto OutputChans = 2;
                    float** outputArray = new float*[OutputChans];
                    for (int i = 0; i < OutputChans; ++i)
                        outputArray[i] = new float[BlockSize1];

                    // First process call should initialise graph
                    auto result = 
                        graph.processGraph (inputArray, InputChans, 
                                            outputArray, OutputChans, BlockSize1);
                    REQUIRE (result == true);

                    //// Second call should have no update
                    //result =
                    //    graph.processGraph (inputArray, InputChans,
                    //                        outputArray, OutputChans, BlockSize1);
                    //REQUIRE_FALSE (result == true);

                    //result = graph.processGraph (inputArray, InputChans,
                    //                             outputArray, OutputChans, BlockSize2);
                    //REQUIRE (result == true);

                    for (int i = 0; i < InputChans; ++i)
                        delete[] inputArray[i];

                    for (int i = 0; i < OutputChans; ++i)
                        delete[] outputArray[i];

                    delete[] inputArray;
                    delete[] outputArray;
                }

                SECTION ("Add listener")
                {
                    struct GraphListener : public Graph::Listener
                    {
                        bool nodeAddedCalled = false;
                        bool nodeRemovedCalled = false;
                        bool connectionAddedCalled = false;
                        bool connectionRemovedCalled = false;
                        bool graphSettingsChangeCalled = false;

                        void newNodeAdded (const Node* const /*newNode*/)
                        {
                            nodeAddedCalled = true;
                        }

                        void nodeRemoved (const Node* const /*removedNode*/)
                        {
                            nodeRemovedCalled = true;
                        }

                        void newConnectionAdded (const Connection& /*newConnection*/)
                        {
                            connectionAddedCalled = true;
                        }

                        void connectionRemoved (const Connection& /*removedConnection*/)
                        {
                            connectionRemovedCalled = true;
                        }

                        void graphSettingsChanged ()
                        {
                            graphSettingsChangeCalled = true;
                        }
                    } listener;

                    REQUIRE (graph.addListener (&listener));

                    SECTION ("Adding new node should trigger listener")
                    {
                        const auto newNode = std::make_unique<Node> (12);
                        graph.addNode (newNode.get ());
                        REQUIRE (listener.nodeAddedCalled == true);

                        SECTION ("Removing node should trigger listener")
                        {
                            graph.removeNode (newNode.get ());
                            REQUIRE (listener.nodeRemovedCalled == true);
                        }
                    }

                    SECTION ("Adding new connection should trigger listener")
                    {
                        Connection newConnection (node3, 0, node4, 0);
                        graph.addConnection (newConnection);
                        REQUIRE (listener.connectionAddedCalled == true);

                        SECTION ("Removing connection should trigger listener")
                        {
                            graph.removeConnection (newConnection);
                            REQUIRE (listener.connectionRemovedCalled == true);
                        }
                    }

                    SECTION ("Changing graph settings should trigger listener")
                    {
                        graph.setSettings (Graph::Settings (88000, 16, 32));
                        REQUIRE (listener.graphSettingsChangeCalled == true);
                    }

                    SECTION ("Remove listener")
                    {
                        REQUIRE (graph.removeListener (&listener));
                    }
                }

                SECTION ("Removing a connection should reduce the number of connections reported")
                {
                    graph.removeConnection (connection1);
                    REQUIRE (graph.connectionCount () == ConnectionCount - 1);
                }

                SECTION ("Remove node with connections, should remove all now invalid connection")
                {
                    // Node 1 is connected to Node 2 and Node 3, removing it 
                    // should reduce connection count by 2
                    const auto connectionCount = graph.connectionCount ();
                    graph.removeNode (node1);
                    REQUIRE (graph.connectionCount () == connectionCount - 2);
                }

                SECTION ("Remove non-existant connection")
                {
                    REQUIRE_FALSE (graph.removeConnection (Connection (222, 0, 424, 4)));
                }
            }

            SECTION ("Remove node from graph")
            {
                graph.removeNode (node1);
                REQUIRE (graph.nodeCount () == NodeCount - 1);
            }

            SECTION ("Clear graph")
            {
                graph.clearGraph ();
                REQUIRE (graph.isEmpty ());
            }
        }
    }
}
