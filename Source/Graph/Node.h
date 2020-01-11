#pragma once

#ifndef Node_H_INCLUDED
#define Node_H_INCLUDED

#include <vector>
#include <string>

#include "AudioBuffer.h"

namespace Ananke {

class Graph;

class Node
{
public:
    Node () = default;

    Node (int id) :
        id (id)
    {
    }

	void setParentGraph (Graph* graph_) 
	{
		this->graph = graph_;
	}

    virtual ~Node () = default;

    friend bool operator==(const Node& lhs, const Node& rhs)
    {
        if (lhs.getID () == rhs.getID ())
            return true;

        return false;
    }

    friend bool operator!=(const Node& lhs, const Node& rhs)
    {
        if (lhs.getID () == rhs.getID ())
            return false;

        return true;
    }

    virtual std::string getName () const
    {
        std::string name ("Node ");
        name += std::to_string (id);
        return name;
    }

	void setID (int _id) { this->id = _id; }
	int getID () const { return id; }

    virtual int getNumInputChannels () const
    {
        return 0;
    }

    virtual int getNumOutputChannels () const
    {
        return 0;
    }

    virtual bool acceptsMidi () const
    {
        return false;
    }

    virtual bool producesMidi () const
    {
        return false;
    }

    virtual void process (const BufferArray& /*buffersIn*/,
                          const BufferArray& /*buffersOut*/,
                          const int numSamples)
    {
        assert (numSamples > 0);
        // TODO need this assertion to work but also deal with 'Empty' buffers
        /*assert (buffersIn.size () > 0 && buffersIn[0] != AudioBuff? 
                    buffersIn[0]->getSize () == numSamples : true);
        assert (buffersOut.size () > 0 ? 
                    buffersOut[0]->getSize () == numSamples : true);*/
    }

    // TODO implement better
    float x = 0.5f;
    float y = 0.5f;

private:
    int id = -1;
	Graph* graph = nullptr;
};

}

#endif
