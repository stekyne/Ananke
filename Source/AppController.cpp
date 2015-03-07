#pragma once

#include "AppController.h"

AppController::AppController ()
    : graphModel (new GraphModel)
    //: valueTree (juce::Identifier ("audioWidgets"))
{
    loadTestData ();
}

AppController::~AppController ()
{

}

void AppController::loadWidgets ()
{

}

void AppController::loadTestData ()
{
    auto node1 = DSP::createNode<GainNode> (20, 20);
    auto node2 = DSP::createNode<SawOSCNode> (50, 20);
    auto node3 = DSP::createNode<LowPassNode> (80, 20);

    graphModel->addNode (node1);
    graphModel->addNode (node2);
    graphModel->addNode (node3);

    graphModel->addConnection (*node2, *node1);
    graphModel->addConnection (*node1, *node3);

    graphModel->buildGraph ();
}