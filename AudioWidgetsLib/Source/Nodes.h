#pragma once

#ifndef NODES_H_INCLUDED
#define NODES_H_INCLUDED

#include "NodeTypes\GainNode.h"
#include "NodeTypes\LowPass.h"
#include "NodeTypes\SawOsc.h"
#include "NodeTypes\ExternalNode.h"

namespace DSP
{
    // FIXME probably don't need this anymore
    template <typename NodeType>
    static NodeModel* const createNode ()
    {
        return new NodeType (0, 0.f, 0.f);
    }

    template <typename NodeType>
    static NodeModel* const createNode (float positionX, float positionY)
    {
        return new NodeType (0, positionX, positionY);
    }

}
#endif
