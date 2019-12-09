#pragma once

#ifndef AUDIOINPUTNODE_H_INCLUDED
#define AUDIOINPUTNODE_H_INCLUDED

#include "..\Node.h"

namespace Ananke
{

class AudioInputNode : public Node
{
public:
	AudioInputNode () = delete;

	AudioInputNode (int id) :
		Node (id)
	{
	}

	int getNumInputChannels () const override
	{
		return numberInputChannels;
	}

	int getNumOutputChannels () const override
	{
		return numberInputChannels;
	}

	bool acceptsMidi () const override
	{
		return false;
	}

	bool producesMidi () const override
	{
		return false;
	}

	void process (const ReadOnlyBufferArray& /*buffersIn*/, const BufferArray& /*buffersOut*/, 
		const int /*numSamples*/) override
	{
		// TODO need this assertion to work but also deal with 'Empty' buffers
		/*assert (buffersIn.size () > 0 && buffersIn[0] != AudioBuff?
					buffersIn[0]->getSize () == numSamples : true);
		assert (buffersOut.size () > 0 ?
					buffersOut[0]->getSize () == numSamples : true);*/
	}

private:
	int numberInputChannels = -1;
};

}

#endif
