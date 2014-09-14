#include "stdafx.h"
#include "CppUnitTest.h"
#include <memory>

#include <cstdio>
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>

#include "GraphModel.h"

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
            graph.addNode (NodeModel (1));
            Assert::AreEqual (1, graph.nodeCount (), L"Graph should contain one node");
        }

        TEST_METHOD (GraphModel_RemoveNode)
        {
            GraphModel graph;
            graph.clearGraph ();

            NodeModel node (1);
            Assert::AreEqual (0, graph.nodeCount (), L"Graph not empty");
            graph.addNode (node);
            Assert::AreEqual (1, graph.nodeCount (), L"Graph should contain one node");
            graph.removeNode (node);
            Assert::AreEqual (0, graph.nodeCount (), L"Could not remove node from graph");
        }

        TEST_METHOD (GraphModel_AddConnection)
        {
            GraphModel graph;

            NodeModel node1 (1), node2 (2);
            graph.addNode (node1);
            graph.addNode (node2);
            Assert::AreEqual (0, graph.connectionCount (), L"Connections present when there should not be");
            graph.addConnection (Connection (node1.getID (), node2.getID ()));
            Assert::AreEqual (1, graph.connectionCount (), L"More than 1 or no connections present");
        }

        TEST_METHOD (GraphModel_RemoveConnection)
        {
            GraphModel graph;

            NodeModel node1 (1), node2 (2);
            graph.addNode (node1);
            graph.addNode (node2);
            Connection connection (node1.getID (), node2.getID ());
            Assert::AreEqual (0, graph.connectionCount (), L"Connections present when there should not be");
            graph.addConnection (connection);
            Assert::AreEqual (1, graph.connectionCount (), L"More than 1 or no connections present");
            graph.removeConnection (connection);
            Assert::AreEqual (0, graph.connectionCount (), L"Failed to remove connection");
        }

        TEST_METHOD (GraphModel_BuildGraph)
        {
            GraphModel graph;

            graph.addNode (NodeModel (1));
            graph.addNode (NodeModel (2));
            graph.addNode (NodeModel (3));
            graph.addNode (NodeModel (4));
            graph.addNode (NodeModel (5));

            graph.addConnection (Connection (1, 2));
            graph.addConnection (Connection (3, 4));
            graph.addConnection (Connection (5, 1));
            graph.addConnection (Connection (2, 3));

            graph.buildGraph ();
        }

    private:
	};
}