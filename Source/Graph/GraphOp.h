#pragma once

#ifndef GRAPHOP_H_INCLUDED
#define GRAPHOP_H_INCLUDED

#include "AudioBuffer.h"

namespace Ananke {

// Interface for a graph operation
struct GraphOp
{
    virtual ~GraphOp () {};
    // TODO remove block size argument? requires graph to be rebuilt anyway
    virtual void perform (const int blockSize) = 0;
    virtual std::string getName () const = 0;
};

class ProcessNodeOp : public GraphOp
{
public:
    ProcessNodeOp (BufferArray&& audioIn, BufferArray&& audioOut, Node* node) :
        audioIn (std::forward<BufferArray>(audioIn)),
        audioOut (std::forward<BufferArray>(audioOut)),
        node (node)
    {
    }

    void perform (const int blockSize) override
    {
        // TODO get audio points for audiobuffer IDs and call process
        node->process (audioIn, audioOut, blockSize);
    }

    std::string getName () const override
    {
        std::string name ("ProcessNodeOp: "); 
        name += node->getName ();
        name += ", Input: ";
        
        if (audioIn.size () == 0)
        {
            name += "(-)";
        }
        else
        {
            for (auto i = 0u; i < audioIn.size (); ++i)
            {
                name += audioIn[i]->getID ().toString ();

                if (i != audioIn.size () - 1)
                    name += ", ";
            }
        }

        name += " Output: ";

        if (audioOut.size () == 0)
        {
            name += "(-)";
        }
        else
        {
            for (auto i = 0u; i < audioOut.size (); ++i)
            {
                name += audioOut[i]->getID ().toString ();

                if (i != audioOut.size () - 1)
                    name += ", ";
            }
        }

        return name;
    }

    int getNodeId () const
    {
        return node->getID ();
    }

private:
    BufferArray audioIn;
    BufferArray audioOut;
    Node* const node;
};

class SumBuffersOp : public GraphOp
{
public:
    SumBuffersOp (BufferArray&& inputBuffers, AudioBuffer<DSP::SampleType>* outputBuffer) :
        inputBuffers (inputBuffers),
        outputBuffer (outputBuffer)
    {
    }

    void perform (const int) override
    {
        outputBuffer->clear ();

        for (auto& buffer : inputBuffers)
            outputBuffer->addDataFrom (buffer);
    }

    std::string getName () const override
    {
        std::string name ("SumBuffersOp: ");
        
        for (auto i = 0u; i < inputBuffers.size (); ++i)
        {
            const auto bufferID = inputBuffers[i]->getID ();

            name += bufferID.toString ();

            if (i != inputBuffers.size () - 1)
            {
                name += ", ";
            }
            else
            {
                name += ", Output Buffer: ";
                name += outputBuffer->getID ().toString ();
            }
        }

        return name;
    }

private:
    BufferArray inputBuffers;
    AudioBuffer<DSP::SampleType>* outputBuffer;
};

}

#endif
