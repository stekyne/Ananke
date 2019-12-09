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

	void setChannelCount (int count)
	{
		numberInputChannels = count;
		// TODO inform graph of update?
	}

	std::string getName () const override {
		return "Audio In";
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

	void process (const BufferArray& /*buffersIn*/, const BufferArray& /*buffersOut*/, 
		const int /*numSamples*/) override
	{
		// TODO implement node process
	}

private:
	int numberInputChannels = -1;
};

}

#endif
