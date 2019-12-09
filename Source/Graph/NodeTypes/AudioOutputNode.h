#pragma once

#ifndef AUDIOOUTPUTNODE_H_INCLUDED
#define AUDIOOUTPUTNODE_H_INCLUDED

#include "..\Node.h"

namespace Ananke
{

class AudioOutputNode : public Node
{
public:
	AudioOutputNode (int id) :
		Node (id)
	{
	}

	void setChannelCount (int count)
	{
		numOutputChannels = count;
		// TODO inform graph of update?
	}

	std::string getName () const override {
		return "Audio Out";
	}

	// The number of channels incoming and outgoing is equal for this node
	// It should match the audio device output settings
	int getNumInputChannels () const override
	{
		return numOutputChannels;
	}

	int getNumOutputChannels () const override
	{
		return numOutputChannels;
	}

	bool acceptsMidi () const override
	{
		return false;
	}

	bool producesMidi () const override
	{
		return false;
	}

	void process (const BufferArray& buffersIn, const BufferArray& buffersOut,
		const int numSamples) override
	{
		assert (numOutputChannels == buffersIn.size ());
		assert (buffersIn.size () == buffersOut.size ());
		assert (numSamples == buffersIn[0]->getSize ());

		// Map incoming channels to output
		// FIXME try and elimate this redundancy somehow
		for (int chanIndex = 0; chanIndex < buffersIn.size (); ++chanIndex)
		{
			const auto incomingBuffer = buffersIn[chanIndex]->getData ();
			const auto incomingBufferSize = buffersIn[chanIndex]->getSize ();
			buffersOut[chanIndex]->copyDataFrom (incomingBuffer, incomingBufferSize);
		}
	}

private:
	int numOutputChannels = -1;
};

}

#endif
