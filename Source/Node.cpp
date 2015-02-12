#include "Node.h"
#include "Pin.h"
#include "GraphComponent.h"
#include "../AudioWidgetsLib/Source/GraphModel.h"

Node::Node (std::shared_ptr<GraphModel> graph_,
            uint32 id_, String name_,
            int inputNum, int outputNum,
            bool hasMidiIn, bool hasMidiOut)
    :   graph (graph_),
        id (id_),
        name (name_),
        numIns (0),
        numOuts (0),
        numInputs (0),
        numOutputs (0),
        font (13.0f, Font::bold)
{
    /** Set size which calls resized to position pins also */
    setSize (120, 35);
}

void Node::getPinPos (const int index, const bool isInput, float& x, float& y)
{
    for (int i = 0; i < getNumChildComponents (); ++i)
    {
        Pin* const pc = dynamic_cast<Pin*>(getChildComponent (i));

        if (pc != nullptr &&
            pc->index == index &&
            isInput == pc->isInput)
        {
            x = getX () + pc->getX () + pc->getWidth () * 0.5f;
            y = getY () + pc->getY () + pc->getHeight () * 0.5f;
            break;
        }
    }
}

void Node::paint (Graphics& g)
{
    g.setColour (Colours::blue);
    g.fillRect (5, 5, getWidth () - 10, getHeight () - 10);
    g.setColour (Colours::yellow);
    g.drawText (name, 0, 0, getWidth (), getHeight (),
                Justification::centred, false);
}

void Node::resized ()
{
    /** Figure out distances between the pins based on the number of them */
    //float x = getWidth() / (float)(inputs.size() + 1);
    //		
    ///** Put pins in correct location */
    //for( int i = 0; i < inputs.size(); ++i )
    //{
    //	inputs[i]->setBounds( (int)((i+1 * x) - 5.0f), 0, 
    //						  inputs[i]->getWidth(), 
    //						  inputs[i]->getHeight() );
    //}

    //x = getWidth() / (float)(outputs.size() + 1);

    //for( int i = 0; i < outputs.size(); ++i )
    //{
    //	outputs[i]->setBounds( (int)((i+1 * x) - 5.0f), getHeight() - 10,
    //							outputs[i]->getWidth(),
    //							outputs[i]->getHeight() );
    //}

    //if( midiIn != nullptr )
    //{
    //	midiIn->setBounds( 0, (getHeight() / 2) - (midiIn->getHeight() / 2),
    //					   midiIn->getWidth(),
    //					   midiIn->getHeight() );
    //}

    //if( midiOut != nullptr )
    //{
    //	midiOut->setBounds( getWidth() - 10, 
    //					    (getHeight() / 2) - (midiOut->getHeight() / 2),
    //					    midiOut->getWidth(),
    //					    midiOut->getHeight() );
    //}
    for (int i = 0; i < getNumChildComponents (); ++i)
    {
        Pin* const pc = dynamic_cast<Pin*>(getChildComponent (i));

        if (pc != nullptr)
        {
            const int total = pc->isInput ? numIns : numOuts;
            const int index = pc->index == Pin::midi_num ? (total - 1) : pc->index;

            pc->setBounds (proportionOfWidth ((1 + index) / (total + 1.0f)) - 5.0f / 2,
                           pc->isInput ? 0 : (getHeight () - 5.0f),
                           5.0f, 5.0f);
        }
    }
}

void Node::update ()
{
    //const AudioProcessorGraph::Node::Ptr f (graph.getNodeForId (id));
    const auto node = graph->getNodeForID (id);

    numIns = node->getNumInputChannels ();

    if (node->acceptsMidi ())
        ++numIns;

    numOuts = node->getNumOutputChannels ();
    if (node->producesMidi ())
        ++numOuts;

    int w = 100;
    int h = 40;

    w = jmax (w, (jmax (numIns, numOuts) + 1) * 20);

    const int textWidth = font.getStringWidth (node->getName ());
    w = jmax (w, 16 + jmin (textWidth, 300));
    if (textWidth > 300)
        h = 100;

    setSize (w, h);
    setName (node->getName ());

    // TODO need to store the node position in the graph somewhere
    /*setCentreRelative (f->properties["x"],
                       f->properties["y"]);*/

    if (numIns != numInputs || numOuts != numOutputs)
    {
        numInputs = numIns;
        numOutputs = numOuts;

        /** Clear all pins from node */
        //inputs.clear();
        //outputs.clear();
        //midiIn  = 0;
        //midiOut = 0;
        deleteAllChildren ();

        unsigned int i;
        for (i = 0; i < node->getNumInputChannels (); ++i)
        {
            Pin* const newPin = new Pin (Pin::AudioInput, id, i, true);
            //inputs.add( newPin );
            addAndMakeVisible (newPin);
        }

        if (node->acceptsMidi ())
        {
            Pin* const midiIn = new Pin (Pin::MidiInput, id, Pin::midi_num, true);
            addAndMakeVisible (midiIn);
        }

        for (i = 0; i < node->getNumOutputChannels (); ++i)
        {
            Pin* const newPin = new Pin (Pin::AudioOutput, id, i, false);
            //outputs.add( newPin );
            addAndMakeVisible (newPin);
        }

        if (node->producesMidi ())
        {
            Pin* const midiOut = new Pin (Pin::MidiOutput, id, Pin::midi_num, false);
            addAndMakeVisible (midiOut);
        }

        resized ();
    }
}

void Node::mouseDown (const MouseEvent& e)
{
    toFront (true);
    dragger.startDraggingComponent (this, e);
}

void Node::mouseDrag (const MouseEvent& e)
{
    dragger.dragComponent (this, e, nullptr);

    const auto& node = graph->getNodeForID (id);

    // TODO need to store the position somewhere for placement in the graph
    /*node->properties.set ("x", (getX () + getWidth () / 2.0f) / (float)getParentWidth ());
    node->properties.set ("y", (getY () + getHeight () / 2.0f) / (float)getParentHeight ());*/

    getGraph ()->updateGraph ();
}

void Node::mouseUp (const MouseEvent& e)
{
}

GraphComponent* Node::getGraph () const
{
    GraphComponent* graphComponent =
        findParentComponentOfClass<GraphComponent> ();

    if (graphComponent == nullptr)
    {
        DBG ("Could not find graph component for pin");
    }

    return graphComponent;
}
