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

        TEST_METHOD (GraphModel_AddConnection)
        {
            GraphModel graph;

            auto node1 = new NodeModel (1);
            auto node2 = new NodeModel (2);
            graph.addNode (node1);
            graph.addNode (node2);
            Assert::AreEqual (0, graph.connectionCount (), L"Connections present when there should not be");
            graph.addConnection (Connection (*node1, *node2));
            Assert::AreEqual (1, graph.connectionCount (), L"More than 1 or no connections present");
        }

        TEST_METHOD (GraphModel_AddConnectionByNodes)
        {
            GraphModel graph;

            auto node1 = new NodeModel (1);
            auto node2 = new NodeModel (2);
            graph.addNode (node1);
            graph.addNode (node2);
            Assert::AreEqual (0, graph.connectionCount (), L"Connections present when there should not be");
            graph.addConnection (node1, node2);
            Assert::AreEqual (1, graph.connectionCount (), L"More than 1 or no connections present");
        }

        TEST_METHOD (GraphModel_RemoveConnection)
        {
            GraphModel graph;

            auto node1 = new NodeModel (1);
            auto node2 = new NodeModel (2);
            graph.addNode (node1);
            graph.addNode (node2);
            Connection connection (*node1, *node2);
            Assert::AreEqual (0, graph.connectionCount (), L"Connections present when there should not be");
            graph.addConnection (connection);
            Assert::AreEqual (1, graph.connectionCount (), L"More than 1 or no connections present");
            graph.removeConnection (connection);
            Assert::AreEqual (0, graph.connectionCount (), L"Failed to remove connection");
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

            graph.addConnection (Connection (*node1, *node2));
            graph.addConnection (Connection (*node3, *node4));
            graph.addConnection (Connection (*node5, *node1));
            graph.addConnection (Connection (*node2, *node3));

            graph.buildGraph ();
        }

        TEST_METHOD (GraphModel_ProcessGraph)
        {
            GraphModel graph;

            graph.addNode (new SawOSCNode (1));
            graph.addNode (new LowPassNode (2));
            graph.addNode (new GainNode (3));

            graph.addConnection (Connection (1, 2));
            graph.addConnection (Connection (2, 3));

            graph.buildGraph ();

            AudioBuffer<float> audioIn (50);
            AudioBuffer<float> audioOut (50);
            graph.processGraph (audioIn, audioOut);
        }

    private:
	};
}