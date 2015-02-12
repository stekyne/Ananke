#pragma once

#ifndef NODES_H_INCLUDED
#define NODES_H_INCLUDED

#include "NodeTypes\GainNode.h"
#include "NodeTypes\LowPass.h"
#include "NodeTypes\SawOsc.h"

static int nodeIDcount = 1;

namespace DSP
{

template <typename NodeType>
static NodeModel* const createNode ()
{
    return new NodeType (nodeIDcount++);
}

}
#endif
