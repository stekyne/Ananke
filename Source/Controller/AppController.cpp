#pragma once

#include "AppController.h"

AppController::AppController ()
    :   graphModel (new GraphModel)
    //: valueTree (juce::Identifier ("audioWidgets"))
{
    loadTestData ();
}

AppController::~AppController ()
{

}

void AppController::audioDeviceIOCallback (
    const float** inputChannelData, int totalNumInputChannels,
    float **outputChannelData, int totalNumOutputChannels,
    int numSamples)
{
    jassert (totalNumInputChannels >= totalNumOutputChannels);
    graphModel->processGraph (inputChannelData, outputChannelData, numSamples);
}

void AppController::audioDeviceAboutToStart (AudioIODevice* /*device*/)
{

}

void AppController::audioDeviceStopped ()
{

}

void AppController::loadWidgets ()
{

}

void AppController::loadTestData ()
{
    auto node1 = DSP::createNode<GainNode> (0.5f, 0.5f);
    auto node2 = DSP::createNode<SawOSCNode> (0.5f, 0.25f);
    auto node3 = DSP::createNode<LowPassNode> (0.5f, 0.75f);

    graphModel->addNode (node1);
    graphModel->addNode (node2);
    graphModel->addNode (node3);

    graphModel->addConnection (*node2, 0, *node1, 0);
    graphModel->addConnection (*node2, 1, *node1, 1);
    graphModel->addConnection (*node1, 0, *node3, 0);
    graphModel->addConnection (*node1, 1, *node3, 1);

    graphModel->buildGraph ();
}