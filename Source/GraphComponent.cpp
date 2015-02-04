#include "GraphComponent.h"
#include "Node.h"
#include "Connector.h"
#include "Pin.h"
#include "../AudioWidgetsLib/Source/GraphModel.h"

GraphComponent::GraphComponent (std::shared_ptr<GraphModel> graph)
    :   graph (graph),
        draggingConnector (nullptr)
{
    setOpaque (true);
    setSize (600, 400);
}

GraphComponent::~GraphComponent ()
{
    deleteAllChildren ();
}

void GraphComponent::resized ()
{
    updateGraph ();
}

void GraphComponent::paint (Graphics& g)
{
    g.fillAll (Colours::grey);
}

Node* GraphComponent::getComponentForFilter (const uint32 filterID) const
{
    for (int i = getNumChildComponents (); --i >= 0;)
    {
        Node* const fc = dynamic_cast <Node*>(getChildComponent (i));

        if (fc != nullptr && fc->id == filterID)
            return fc;
    }

    return nullptr;
}

Connector* GraphComponent::getComponentForConnection (
    const AudioProcessorGraph::Connection& conn) const
{
    for (int i = getNumChildComponents (); --i >= 0;)
    {
        Connector* const c = dynamic_cast<Connector*>(getChildComponent (i));

        if (c != nullptr
            && c->sourceFilterID == conn.sourceNodeId
            && c->destFilterID == conn.destNodeId
            && c->sourceFilterChannel == conn.sourceChannelIndex
            && c->destFilterChannel == conn.destChannelIndex)
        {
            return c;
        }
    }

    return nullptr;
}

void GraphComponent::beginConnector (const uint32	sourceFilterID,
                                     const int		sourceFilterChannel,
                                     const uint32	destFilterID,
                                     const int		destFilterChannel,
                                     const MouseEvent& e)
{
    /** Delete previous connector if it exists, get a pointer to the connector
    if the user clicked on one */
    delete draggingConnector;
    draggingConnector = dynamic_cast<Connector*> (e.originalComponent);

    /** User didnt click on a connector so create a new one */
    if (draggingConnector == nullptr)
    {
        draggingConnector = new Connector (*graph.get (),
                                           sourceFilterID, sourceFilterChannel,
                                           destFilterID, destFilterChannel);
    }

    /** Set inital position of the connector */
    draggingConnector->setInput (sourceFilterID, sourceFilterChannel);
    draggingConnector->setOutput (destFilterID, destFilterChannel);

    addAndMakeVisible (draggingConnector);
    draggingConnector->toFront (false);

    dragConnector (e);
}

void GraphComponent::dragConnector (const MouseEvent& e)
{
    const MouseEvent e2 (e.getEventRelativeTo (this));

    if (draggingConnector != nullptr)
    {
        //draggingConnector->setTooltip( String::empty );

        int x = e2.x;
        int y = e2.y;

        Pin* const pin = findPin (x, y);

        if (pin != nullptr)
        {
            uint32  srcFilter = draggingConnector->sourceFilterID;
            int		srcChannel = draggingConnector->sourceFilterChannel;

            uint32	dstFilter = draggingConnector->destFilterID;
            int		dstChannel = draggingConnector->destFilterChannel;

            if (srcFilter == 0 && !pin->isInput)
            {
                srcFilter = pin->filterID;
                srcChannel = pin->index;
            }
            else if (dstFilter == 0 && pin->isInput)
            {
                dstFilter = pin->filterID;
                dstChannel = pin->index;
            }

            //if (graph.canConnect (srcFilter, srcChannel, dstFilter, dstChannel))
            {
                x = pin->getParentComponent ()->getX () + pin->getX () + pin->getWidth () / 2;
                y = pin->getParentComponent ()->getY () + pin->getY () + pin->getHeight () / 2;

                //draggingConnector->setTooltip (pin->getTooltip());
            }
        }

        if (draggingConnector->sourceFilterID == 0)
            draggingConnector->dragStart (x, y);
        else
            draggingConnector->dragEnd (x, y);
    }

}

void GraphComponent::endConnector (const MouseEvent& e)
{
    if (draggingConnector == nullptr)
        return;

    //draggingConnector->setTooltip( String::empty );

    const MouseEvent e2 (e.getEventRelativeTo (this));

    uint32  srcFilter = draggingConnector->sourceFilterID;
    int		srcChannel = draggingConnector->sourceFilterChannel;

    uint32	dstFilter = draggingConnector->destFilterID;
    int		dstChannel = draggingConnector->destFilterChannel;

    deleteAndZero (draggingConnector);

    Pin* const pin = findPin (e2.x, e2.y);

    if (pin != nullptr)
    {
        if (srcFilter == 0)
        {
            if (pin->isInput)
                return;

            srcFilter = pin->filterID;
            srcChannel = pin->index;
        }
        else
        {
            if (!pin->isInput)
                return;

            dstFilter = pin->filterID;
            dstChannel = pin->index;
        }

        /*if (graph->addConnection (srcFilter, srcChannel, dstFilter, dstChannel))
        {
            DBG ("Connection is successful: " + String (srcFilter) + " to " + String (dstFilter));
            updateGraph ();
        }
        else
        {
            DBG ("Connection unsuccessful: " + String (srcFilter) + " to " + String (dstFilter));
        }*/
    }
}

Pin* GraphComponent::findPin (const int x, const int y) const
{
    for (int i = getNumChildComponents (); --i >= 0;)
    {
        Node* const fc = dynamic_cast<Node*>(getChildComponent (i));

        if (fc != nullptr)
        {
            Pin* const pin = dynamic_cast<Pin*>(fc->getComponentAt (x - fc->getX (),
                y - fc->getY ()));

            if (pin != nullptr)
                return pin;
        }
    }

    return nullptr;
}

void GraphComponent::updateGraph ()
{
    int i;
    for (i = getNumChildComponents (); --i >= 0;)
    {
        Node* const fc = dynamic_cast<Node*>(getChildComponent (i));

        if (fc != nullptr)
            fc->update ();
    }

    for (i = getNumChildComponents (); --i >= 0;)
    {
        Connector* const cc = dynamic_cast<Connector*>(getChildComponent (i));

        if (cc != nullptr && cc != draggingConnector)
        {
            const Connection testConnection (cc->sourceFilterID,
                                             //cc->sourceFilterChannel,
                                             cc->destFilterID);
                                             //cc->destFilterChannel);

            if (graph->connectionExists (testConnection) == false)
            {
                delete cc;
            }
            else
            {
                cc->update ();
            }
        }
    }

    for (auto& elem : graph->getNodes ())
    {
        if (getComponentForFilter (elem.first.getNumber ()) == 0)
        {
            //Node* const node = new Node (graph, f->nodeId,
            //                             f->getProcessor ()->getName (),
            //                             f->getProcessor ()->getNumInputChannels (),
            //                             f->getProcessor ()->getNumOutputChannels (),
            //                             f->getProcessor ()->acceptsMidi (),
            //                             f->getProcessor ()->producesMidi ());
            //addAndMakeVisible (node);
            //node->update ();
        }
    }

    /*for (i = graph.getNumConnections (); --i >= 0;)
    {
        const AudioProcessorGraph::Connection* const c = graph.getConnection (i);

        if (getComponentForConnection (*c) == 0)
        {
            Connector* const conn = new Connector (graph);

            addAndMakeVisible (conn);

            conn->setInput (c->sourceNodeId, c->sourceChannelIndex);
            conn->setOutput (c->destNodeId, c->destChannelIndex);
        }
    }*/
}
