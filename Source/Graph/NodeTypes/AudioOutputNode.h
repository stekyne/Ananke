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

	int getNumInputChannels () const override
	{
		// TODO adjust to audio interface
		return 2;
	}

	int getNumOutputChannels () const override
	{
		// TODO adjust to audio interface
		return 2;
	}

	bool acceptsMidi () const override
	{
		return false;
	}

	bool producesMidi () const override
	{
		return false;
	}

	void process (const ReadOnlyBufferArray& buffersIn, const BufferArray& buffersOut,
		const int numSamples) override
	{
		for (int i = 0; i < buffersIn.size(); ++i)
		{
			// TODO fix this ugly syntax
			if (i < buffersOut.size ())
				buffersOut[i]->copyDataFrom (buffersIn[i]->getData(), buffersIn[i]->getSize ());
		}
	}
};

}

#endif