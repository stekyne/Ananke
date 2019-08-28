#pragma once

#ifndef Node_H_INCLUDED
#define Node_H_INCLUDED

#include <vector>
#include <string>

#include "AudioBuffer.h"

namespace APG {

class Node
{
public:
    Node () = default;

    Node (int id) : 
        id (id)
    {
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

    virtual uint32_t getNumInputChannels () const
    {
        return 0;
    }

    virtual uint32_t getNumOutputChannels () const
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

    void setID (uint32_t _id) { this->id = _id; }
    uint32_t getID () const { return id; }

    virtual void process (const ReadOnlyBufferArray& /*buffersIn*/,
                          const BufferArray& /*buffersOut*/,
                          const uint32_t numSamples)
    {
        assert (numSamples > 0);
        // TODO need this assertion to work but also deal with 'Empty' buffers
        /*assert (buffersIn.size () > 0 && buffersIn[0] != AudioBuff? 
                    buffersIn[0]->getSize () == numSamples : true);
        assert (buffersOut.size () > 0 ? 
                    buffersOut[0]->getSize () == numSamples : true);*/
    }

    // An empty instance of this class
    static Node Empty;

private:
    int id {0};
};

}

#endif
