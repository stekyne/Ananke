#include "..\src\Graph.h"
#include "..\src\Node.h"
#include "..\src\Nodes.h"

#include "Helpers\TestNodes.h"
#include "Helpers\GraphVerifier.h"

#include "catch.hpp"

namespace AudioWidgetsTests
{
    SCENARIO ("Graph should process nodes in the correct order")
    {
        GIVEN ("A graph with 3 nodes")
        {
            float** inputBuffers = new float*[1];
            inputBuffers[0] = new float[1];
            inputBuffers[0][0] = 0.f;

            float** outputBuffers = new float*[1];
            outputBuffers[0] = new float[1];
            outputBuffers[0][0] = 0.f;

            Graph graph (Graph::Settings (1, 1, 1));

            REQUIRE (graph.addNode (new InputOutputNode (1)));
            REQUIRE (graph.addNode (new InputOutputNode (2)));
            REQUIRE (graph.addNode (new InputOutputNode (3)));

            WHEN ("Connections are added, A:0->C:0, B:0->C:0")
            {
                REQUIRE (graph.addConnection (Connection (1, 0, 3, 0)));
                REQUIRE (graph.addConnection (Connection (2, 0, 3, 0)));

                THEN ("The nodes should produce output based on the correct order")
                {
                    REQUIRE (graph.buildGraph ());
                    REQUIRE (graph.processGraph (const_cast<const float**> (inputBuffers), 1,
                                                 outputBuffers, 1, 1));
                }
            }

            delete[] inputBuffers[0];
            delete[] inputBuffers;

            delete[] outputBuffers[0];
            delete[] outputBuffers;
        }
    }

    SCENARIO ("Graph should sum node inputs where appropriate")
    {
        GIVEN ("A graph with 5 nodes")
        {
            Graph graph (Graph::Settings (1, 1, 1));

            REQUIRE (graph.addNode (new InputOutputNode (1)));
            REQUIRE (graph.addNode (new InputOutputNode (2)));
            REQUIRE (graph.addNode (new InputOutputNode (3)));
            REQUIRE (graph.addNode (new InputOutputNode (4)));
            REQUIRE (graph.addNode (new InputOutputNode (5)));

            WHEN ("Connections are added, A:0->E:0, B:0->E:0, C:0->E:0, D:1->E:1")
            {
                REQUIRE (graph.addConnection (Connection (1, 0, 5, 0)));
                REQUIRE (graph.addConnection (Connection (2, 0, 5, 0)));
                REQUIRE (graph.addConnection (Connection (3, 0, 5, 0)));
                REQUIRE (graph.addConnection (Connection (4, 0, 5, 1)));

                THEN ("Build graph and produce valid output, inputs to E should be summed")
                {
                    REQUIRE (graph.buildGraph ());
                    /*REQUIRE (GraphVerifier::Assert (graph, 
                                "A:0->E:0, B:0->E:0, C:0->E:0, D:1->E:1"));*/
                }
            }
        }
    }

    SCENARIO ("Graph processing audio")
    {
        Graph graph (Graph::Settings (44100, 128, 32));

        GIVEN ("A graph with 7 nodes")
        {
            REQUIRE (graph.addNode (new SawOSCNode (1)));
            REQUIRE (graph.addNode (new SawOSCNode (2)));
            REQUIRE (graph.addNode (new SawOSCNode (3)));
            REQUIRE (graph.addNode (new SawOSCNode (4)));
            REQUIRE (graph.addNode (new SawOSCNode (5)));
            REQUIRE (graph.addNode (new GainNode (6)));
            REQUIRE (graph.addNode (new LowPassNode (7)));

            WHEN ("Connections are made and graph is built")
            {
                REQUIRE (graph.addConnection (Connection (Graph::AudioInputID, 0, 6, 0)));
                REQUIRE (graph.addConnection (Connection (Graph::AudioInputID, 1, 6, 1)));
                REQUIRE (graph.addConnection (Connection (1, 0, 6, 0)));
                REQUIRE (graph.addConnection (Connection (1, 1, 6, 1)));
                REQUIRE (graph.addConnection (Connection (2, 0, 6, 0)));
                REQUIRE (graph.addConnection (Connection (2, 1, 6, 1)));
                REQUIRE (graph.addConnection (Connection (3, 0, 6, 0)));
                REQUIRE (graph.addConnection (Connection (3, 1, 6, 1)));
                REQUIRE (graph.addConnection (Connection (4, 0, 6, 0)));
                REQUIRE (graph.addConnection (Connection (4, 1, 6, 1)));
                REQUIRE (graph.addConnection (Connection (5, 0, 6, 0)));
                REQUIRE (graph.addConnection (Connection (5, 1, 6, 1)));
                REQUIRE (graph.addConnection (Connection (6, 0, 7, 0)));
                REQUIRE (graph.addConnection (Connection (6, 1, 7, 1)));
                REQUIRE (graph.addConnection (Connection (7, 0, Graph::AudioOutputID, 0)));
                REQUIRE (graph.addConnection (Connection (7, 1, Graph::AudioOutputID, 1)));
                REQUIRE (graph.buildGraph ());

                WHEN ("Graph is processing data for a number of iterations")
                {
                    float** inputBuffers = new float*[1];
                    inputBuffers[0] = new float[1];
                    inputBuffers[0][0] = 0.f;

                    float** outputBuffers = new float*[1];
                    outputBuffers[0] = new float[1];
                    outputBuffers[0][0] = 0.f;

                    for (int i = 0; i < 10'000; ++i)
                    {
                        REQUIRE (graph.processGraph (const_cast<const float**> (inputBuffers), 1,
                                                     outputBuffers, 1, 1));
                    }

                    THEN ("Graph should clean up afterwards cleanly")
                    {
                        graph.clearGraph ();
                        REQUIRE (graph.isEmpty ());
                    }

                    delete[] inputBuffers[0];
                    delete[] inputBuffers;

                    delete[] outputBuffers[0];
                    delete[] outputBuffers;
                }
            }
        }
    }
}
