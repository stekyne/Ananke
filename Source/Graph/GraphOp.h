#pragma once

#ifndef GRAPHOP_H_INCLUDED
#define GRAPHOP_H_INCLUDED

#include "AudioBuffer.h"

namespace APG {

// Interface for a graph operation
struct GraphOp
{
    virtual ~GraphOp () {};
    // TODO remove block size argument? requires graph to be rebuilt anyway
    virtual void perform (const uint32_t blockSize) = 0;
    virtual std::string getName () const = 0;

private:
    static uint32_t count;
};

class ProcessNodeOp : public GraphOp
{
public:
    ProcessNodeOp (ReadOnlyBufferArray&& audioIn, BufferArray&& audioOut, Node* node) :
        audioIn (std::forward<ReadOnlyBufferArray>(audioIn)),
        audioOut (std::forward<BufferArray>(audioOut)),
        node (node)
    {
    }

    void perform (const uint32_t blockSize) override
    {
        // TODO Convert call to template function and see if that can replace the virtual function call
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

    uint32_t getNodeId () const
    {
        return node->getID ();
    }

private:
    ReadOnlyBufferArray audioIn;
    BufferArray audioOut;
    Node* const node;
};

class SumBuffersOp : public GraphOp
{
public:
    SumBuffersOp (BufferArray&& inputBuffers, 
                  AudioBuffer<DSP::SampleType>* outputBuffer) :
        inputBuffers (inputBuffers),
        outputBuffer (outputBuffer)
    {
        std::vector<AudioBufferID> nodeIDs;

        for (const auto& inputBuffer : inputBuffers)
        {
            nodeIDs.push_back (inputBuffer->getID ());
        }

        outputBuffer->setID (AudioBufferID (std::move (nodeIDs)));
    }

    void perform (const uint32_t) override
    {
        outputBuffer->clear ();

        for (auto& buffer : inputBuffers)
        {
            outputBuffer->addDataFrom (buffer);
        }
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
