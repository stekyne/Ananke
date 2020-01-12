#include "NodeComponent.h"
#include "PinComponent.h"
#include "GraphComponent.h"
#include "../Source/Graph/Graph.h"

namespace Ananke {

NodeComponent::NodeComponent (GraphComponent* graphComponent, int id) :
	graphComp (graphComponent),
	id (id),
	font (13.0f, Font::bold)
{
	setSize (120, 35);
}

std::tuple<float, float> NodeComponent::getPinPos (const int index, const bool isInput)
{
	const auto findPin = [&](auto& pin) -> bool {
		return pin->ChannelIndex == index && isInput == pin->IsInput;
	};

	auto findResult = std::find_if (inputs.begin (), inputs.end (), findPin);

	if (findResult == inputs.end ()) {
		findResult = std::find_if (outputs.begin (), outputs.end (), findPin);

		// Could not find matching pin
		if (findResult == outputs.end ()) 
			return std::make_tuple<float, float> (-1.f, -1.f);
	}

	return std::make_tuple<float, float> (
		getX () + (*findResult)->getX () + (*findResult)->getWidth () * 0.05f,
		getY () + (*findResult)->getY () + (*findResult)->getHeight () * 0.05f);
}

void NodeComponent::paint (Graphics& g)
{
	const auto node = graphComp->getGraph()->getNodeForID (id);

	g.setColour (Colours::lightgrey);
	g.fillRoundedRectangle (5.f, 5.f, getWidth () - 10.f, getHeight () - 10.f, 5.f);
	g.setColour (Colours::darkgrey);
	g.setFont (Font ("Arial", "Bold", 12.f));
	
	if (node != nullptr)
		g.drawText (node->getName (), 0, 0, getWidth (), getHeight (), Justification::centred, false);
}

void NodeComponent::resized ()
{
	// Figure out distances between the pins based on the number of them 
	auto x = getWidth () / (float)(inputs.size () + 1);

	// Put pins in correct location
	for (auto i = 0; i < (int)inputs.size (); ++i)
	{
		inputs[i]->setBounds ((int)((i + 1 * x) - 5.0f), 0,
			inputs[i]->getWidth (),
			inputs[i]->getHeight ());
	}

	x = getWidth () / (float)(outputs.size () + 1);

	for (auto i = 0; i < (int)outputs.size (); ++i)
	{
		outputs[i]->setBounds ((int)((i + 1 * x) - 5.0f), getHeight () - 10,
			outputs[i]->getWidth (),
			outputs[i]->getHeight ());
	}

	if (midiIn)
	{
		midiIn->setBounds (0, (getHeight () / 2) - (midiIn->getHeight () / 2),
			midiIn->getWidth (),
			midiIn->getHeight ());
	}

	if (midiOut)
	{
		midiOut->setBounds (getWidth () - 10,
			(getHeight () / 2) - (midiOut->getHeight () / 2),
			midiOut->getWidth (),
			midiOut->getHeight ());
	}

	const auto updatePin = [&](auto& pin) {
		const int total = pin->IsInput ? numIns : numOuts;
		const int index = (pin->ChannelIndex == PinComponent::MidiNum) ? (total - 1) : pin->ChannelIndex;

		pin->setBounds ((int)(proportionOfWidth ((1 + index) / (total + 1.0f)) - 5 / 2.f),
			pin->IsInput ? 0 : (getHeight () - 5), 5, 5);
	};

	std::for_each (inputs.begin (), inputs.begin (), updatePin);
	std::for_each (outputs.begin (), outputs.begin (), updatePin);
}

void NodeComponent::update ()
{
	jassert (graphComp != nullptr);

	const auto node = graphComp->getGraph()->getNodeForID (id);
	jassert (node != nullptr);
	numIns = node->getNumInputChannels ();

	if (node->acceptsMidi ())
		++numIns;

	numOuts = node->getNumOutputChannels ();
	if (node->producesMidi ())
		++numOuts;

	int width = 100;
	int height = 40;

	width = jmax (width, (jmax (numIns, numOuts) + 1) * 20);

	const int textWidth = font.getStringWidth (node->getName ());
	width = jmax (width, 16 + jmin (textWidth, 300));

	if (textWidth > 300)
		height = 100;

	setSize (width, height);
	setName (node->getName ());

	setCentreRelative (node->x, node->y);

	// Check for pin changes
	if (numIns != numInputs || numOuts != numOutputs)
	{
		numInputs = numIns;
		numOutputs = numOuts;

		inputs.clear ();
		outputs.clear ();
		midiIn = nullptr;
		midiOut = nullptr;

		for (auto i = 0; i < node->getNumInputChannels (); ++i)
		{
			inputs.emplace_back (std::make_unique<PinComponent> (PinComponent::AudioInput, id, i, graphComp));
			addAndMakeVisible (inputs.back ().get ());
		}

		for (auto i = 0; i < node->getNumOutputChannels (); ++i)
		{
			outputs.emplace_back (std::make_unique<PinComponent> (PinComponent::AudioOutput, id, i, graphComp));
			addAndMakeVisible (outputs.back ().get ());
		}

		if (node->acceptsMidi ())
		{
			midiIn = std::make_unique<PinComponent> (PinComponent::MidiInput, id, PinComponent::MidiNum, graphComp);
			addAndMakeVisible (*midiIn);
		}

		if (node->producesMidi ())
		{
			midiOut = std::make_unique<PinComponent> (PinComponent::MidiOutput, id, PinComponent::MidiNum, graphComp);
			addAndMakeVisible (*midiOut);
		}

		resized ();
	}
}

void NodeComponent::mouseDown (const MouseEvent& e)
{
	toFront (true);
	dragger.startDraggingComponent (this, e);
}

void NodeComponent::mouseDrag (const MouseEvent& e)
{
	jassert (graphComp != nullptr);

	dragger.dragComponent (this, e, nullptr);
	auto node = graphComp->getGraph()->getNodeForID (id);

	node->x = (getX () + getWidth () / 2.0f) / (float)getParentWidth ();
	node->y = (getY () + getHeight () / 2.0f) / (float)getParentHeight ();

	// Update this nodes connections and connections going into it to ensure they follow
	// this node while dragging
	//graphComp->

	graphComp->redrawSubComponents ();
}

void NodeComponent::mouseUp (const MouseEvent& /*e*/)
{
	graphComp->redrawSubComponents ();
}

}
