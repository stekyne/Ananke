#pragma once

#ifndef EXTERNALNODE_H_INCLUDED
#define EXTERNALNODE_H_INCLUDED

#include <assert.h>
#include "..\NodeModel.h"

// FIXME remove this class and come up with a better solution
//class ExternalNode : public NodeModel
//{
//public:
//    enum NodeType
//    {
//        InputType,
//        OutputType
//    };
//
//public:
//    ExternalNode () = delete;
//    ExternalNode (float positionX, float positionY, NodeType _type) : 
//        NodeModel (_type == InputType ? InputNodeID : OutputNodeID,
//                   positionX, positionY),
//        type (_type)        
//    {
//    }
//
//    ~ExternalNode () = default;
//
//    void process (InputBufArray buffersIn,
//                  OutputBufArray buffersOut,
//                  const uint32_t _numSamples) override
//    {
//        // No need to process anything if there are no incoming channels
//        if (numChannels == 0)
//            return;
//
//        assert (buffers != nullptr);
//        assert (numSamples == _numSamples);
//
//        if (type == NodeType::InputType)
//        {
//            assert (buffersOut.size () <= numChannels);
//
//            for (auto i = 0u; i < buffersOut.size (); ++i)
//            {
//                auto* bufIn = buffers[i];
//                auto* bufOut = buffersOut[i];
//                bufOut->copyDataFrom (bufIn, numSamples);
//            }
//        }
//        else
//        {
//            assert (buffersIn.size () <= numChannels);
//
//            for (auto i = 0u; i < buffersIn.size (); ++i)
//            {
//                auto* bufIn = buffersIn[i];
//                auto* bufOut = buffers[i];
//                bufIn->copyDataTo (bufOut, numSamples);
//            }
//        }
//    }
//
//    const char* const getName () const override
//    {
//        return type == InputType ? "InputNode" : "OutputNode";
//    }
//
//    unsigned int getNumInputChannels () const override
//    {
//        return type == InputType ? 0 : numChannels;
//    }
//
//    unsigned int getNumOutputChannels () const override
//    {
//        return type == InputType ? numChannels : 0;
//    }
//
//    void setExternalBuffers (float** const _buffers, 
//                             uint32_t _numChannels, uint32_t _numSamples)
//    {
//        this->buffers = _buffers;
//        this->numChannels = _numChannels;
//        this->numSamples = _numSamples;
//    }
//
//private:
//    float** buffers {nullptr};
//    uint32_t numChannels {2}, numSamples {0};
//    NodeType type;
//};

#endif
