#include "stdafx.h"
#include "CppUnitTest.h"
#include <memory>

#include <cstdio>
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>

#include "GraphModel.h"
#include "Nodes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AudioWidgetsTests
{		
    TEST_CLASS (TestGraphModel)
    {
    public:
        TEST_METHOD (GraphModel_AddNode)
        {
            GraphModel graph;

            Assert::AreEqual (0, graph.nodeCount (), L"Graph not empty");
            graph.addNode (new NodeModel (1));
            Assert::AreEqual (1, graph.nodeCount (), L"Graph should contain one node");
        }

        TEST_METHOD (GraphModel_RemoveNode)
        {
            GraphModel graph;
            graph.clearGraph ();

            auto node = new NodeModel(1);
            Assert::AreEqual (0, graph.nodeCount (), L"Graph not empty");
            graph.addNode (node);
            Assert::AreEqual (1, graph.nodeCount (), L"Graph should contain one node");
            graph.removeNode (node);
            Assert::AreEqual (0, graph.nodeCount (), L"Could not remove node from graph");
        }

        TEST_METHOD (GraphModel_RemoveNodeWithConnections)
        {
            GraphModel graph;
            auto node2 = new NodeModel (2);
            graph.addNode (new NodeModel (1));
            graph.addNode (node2);
            graph.addNode (new NodeModel (3));

            graph.addConnection (Connection (1, 0, 2, 0));
            graph.addConnection (Connection (2, 0, 3, 0));

            Assert::AreEqual (2, graph.connectionCount (), L"Connection count be be two");
            graph.removeNode (node2);

            Assert::AreEqual (1, graph.connectionCount (), L"Connection count should be one");
        }

        TEST_METHOD (GraphModel_AddConnection)
        {
            GraphModel graph;

            auto node1 = new NodeModel (1);
            auto node2 = new NodeModel (2);
            Assert::AreEqual (0, graph.nodeCount (), L"Graph should have no nodes");
            graph.addNode (node1);
            graph.addNode (node2);
            Assert::AreEqual (0, graph.connectionCount (), L"Connections present when there should not be");
            graph.addConnection (Connection (*node1, 0, *node2, 0));
            Assert::AreEqual (1, graph.connectionCount (), L"More than 1 or no connections present");
        }

        TEST_METHOD (GraphModel_RemoveConnection)
        {
            GraphModel graph;

            auto node1 = new NodeModel (1);
            auto node2 = new NodeModel (2);
            graph.addNode (node1);
            graph.addNode (node2);
            Connection connection (*node1, 0, *node2, 0);
            Assert::AreEqual (0, graph.connectionCount (), L"Connections present when there should not be");
            graph.addConnection (connection);
            Assert::AreEqual (1, graph.connectionCount (), L"More than 1 or no connections present");
            graph.removeConnection (connection);
            Assert::AreEqual (0, graph.connectionCount (), L"Failed to remove connection");
        }

        TEST_METHOD (GraphModel_ConnectionExists)
        {
            GraphModel graph;
            graph.addNode (new NodeModel (1));
            graph.addNode (new NodeModel (2));

            Assert::AreEqual (0, graph.connectionCount (), L"Graph should have no connections");
            
            Connection connection (1, 0, 2, 0);
            graph.addConnection (connection);
            Assert::AreEqual (1, graph.connectionCount (), L"Graph should have one connection");

            const auto result = graph.connectionExists (connection);
            Assert::AreEqual (true, result, L"Connection should exist but doesn't");
        }

        TEST_METHOD (GraphModel_CanConnect)
        {
            GraphModel graph;
            auto node1 = new NodeModel;
            auto node2 = new NodeModel;
            auto node3 = new NodeModel;
            graph.addNode (node1);
            graph.addNode (node2);
            graph.addNode (node3);

            graph.addConnection (Connection (1, 0, 2, 0));
            graph.addConnection (Connection (2, 0, 3, 0));

            Connection testConnection (3, 0, 1, 0);
            auto result = graph.canConnect (testConnection);
            Assert::AreEqual (false, result, L"Should not be able to connect node 3 to 1 (loop)");

            Connection testConnection2 (1, 0, 3, 0);
            result = graph.canConnect (testConnection2);
            Assert::AreEqual (true, result, L"Should be able to connect node 1 to 3");
        }

        TEST_METHOD (GraphModel_ValidateConnection)
        {
            GraphModel graph;
            auto node1 = new NodeModel;
            auto node2 = new NodeModel;
            auto node3 = new NodeModel;
            graph.addNode (node1);
            graph.addNode (node2);
            graph.addNode (node3);

            graph.addConnection (Connection (*node1, 0, *node2, 0));
            graph.addConnection (Connection (2, 0, 3, 0));

            auto result = graph.validateConnection (Connection (52, 0, 22, 0));
            Assert::AreEqual (false, result, L"Connection should not be valid");

            result = graph.validateConnection (Connection (1, 0, 3, 0));
            Assert::AreEqual (true, result, L"Connection should be valid but isn't");
        }

        TEST_METHOD (GraphModel_BuildGraph)
        {
            GraphModel graph;

            auto node1 = DSP::createNode<NodeModel> ();
            auto node2 = DSP::createNode<NodeModel> ();
            auto node3 = DSP::createNode<NodeModel> ();
            auto node4 = DSP::createNode<NodeModel> ();
            auto node5 = DSP::createNode<NodeModel> ();

            graph.addNode (node1);
            graph.addNode (node2);
            graph.addNode (node3);
            graph.addNode (node4);
            graph.addNode (node5);

            graph.addConnection (Connection (*node1, 0, *node2, 0));
            graph.addConnection (Connection (*node3, 0, *node4, 0));
            graph.addConnection (Connection (*node5, 0, *node1, 0));
            graph.addConnection (Connection (*node2, 0, *node3, 0));

            graph.buildGraph ();
            Logger::WriteMessage (graph.printGraph ().c_str ());
        }

        TEST_METHOD (GraphModel_ProcessGraph)
        {
            GraphModel graph;

            auto node1 = DSP::createNode<SawOSCNode> ();
            auto node2 = DSP::createNode<LowPassNode> ();
            auto node3 = DSP::createNode<GainNode> ();

            graph.addNode (node1);
            graph.addNode (node2);
            graph.addNode (node3);

            graph.addConnection (*node1, 0, *node2, 0);
            graph.addConnection (*node2, 0, *node3, 0);

            graph.buildGraph ();
            Logger::WriteMessage (graph.printGraph ().c_str ());

            AudioBuffer<float> audioIn (50);
            AudioBuffer<float> audioOut (50);
            graph.processGraph (audioIn, audioOut);
        }

        TEST_METHOD (GraphModel_AddListener)
        {
            GraphModel graph;

            struct GraphListener : public GraphModel::Listener
            {
                void newNodeAdded () {}
                void nodeRemoved () {}
                void newConnectionAdded () {}
                void connectionRemoved () {}
                void graphSettingsChanged () {}
            } graphListener;

            const auto result = graph.addListener (&graphListener);

            Assert::AreEqual (true, result, L"Failed to add new listener");
        }

        TEST_METHOD (GraphModel_RemoveListener)
        {
            GraphModel graph;

            struct GraphListener : public GraphModel::Listener
            {
                void newNodeAdded () {}
                void nodeRemoved () {}
                void newConnectionAdded () {}
                void connectionRemoved () {}
                void graphSettingsChanged () {}
            } graphListener;

            graph.addListener (&graphListener);
            const auto result = graph.removeListener (&graphListener);

            Assert::AreEqual (true, result, L"Failed to remove listener");
        }

    private:
    };
}