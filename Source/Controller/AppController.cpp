#pragma once

#include "AppController.h"

AppController::AppController () :
    graphModel (new GraphModel),
    valueTree (juce::Identifier ("audioWidgets")),
    deviceManager (std::make_unique<AudioDeviceManager> ()),
    formatManager (std::make_unique<AudioFormatManager> ())
{
    loadTestData ();
}

AppController::~AppController ()
{
    if (deviceManager != nullptr)
    {
        deviceManager->removeAllChangeListeners ();
        deviceManager->removeAudioCallback (this);
    }
}

bool AppController::initialiseAudioDevice ()
{
    const String result (deviceManager->initialise (0, 2, 0, true));

    // 'result' will be empty if 'initialise' succeeded in opening a device
    if (result.isNotEmpty ())
    {
        DBG ("Error: A device could not be opened.");
        
        return false;
    }
    else
    {
        deviceManager->addAudioCallback (this);
        deviceManager->addChangeListener (this);
    }

    formatManager->registerBasicFormats ();
    return true;
}

void AppController::audioDeviceIOCallback (
    const float** inputChannelData, int /*totalNumInputChannels*/,
    float **outputChannelData, int /*totalNumOutputChannels*/,
    int numSamples)
{
    // If the blockSize does not match the callback, then we must re-allocate memory
    if (graphModel->getSettings ().blockSize != numSamples)
    {
        const auto sampleRate = 
            (float)deviceManager->getCurrentAudioDevice ()->getCurrentSampleRate ();

        // TODO how to adjust control rate?
        graphModel->setSettings (GraphModel::Settings (sampleRate, numSamples, 50));
    }
    
    graphModel->processGraph (inputChannelData, outputChannelData, numSamples);
}

void AppController::audioDeviceAboutToStart (AudioIODevice* device)
{
    const auto sampleRate = (float)device->getCurrentSampleRate ();
    const auto blockSize = device->getCurrentBufferSizeSamples ();

    graphModel->setSettings (GraphModel::Settings (sampleRate, blockSize, 50));
}

void AppController::audioDeviceStopped ()
{

}

std::unique_ptr<AudioDeviceSelectorComponent> AppController::getSelector (
    int width, int height)
{
    std::unique_ptr<AudioDeviceSelectorComponent> deviceSelector =
        std::make_unique<AudioDeviceSelectorComponent> (
            *deviceManager, 0, 2, 0, 2, true, true, true, false);

    deviceSelector->setSize (width, height);
    return deviceSelector;
}

void AppController::changeListenerCallback (ChangeBroadcaster* /*source*/)
{

}

void AppController::loadTestData ()
{
    auto inputNode = new ExternalNode (0.1f, 0.1f, ExternalNode::InputType);
    auto outputNode = new ExternalNode (0.9f, 0.9f, ExternalNode::OutputType);

    auto node1 = DSP::createNode<GainNode> (0.5f, 0.5f);
    auto node2 = DSP::createNode<SawOSCNode> (0.5f, 0.25f);
    auto node3 = DSP::createNode<LowPassNode> (0.5f, 0.75f);

    graphModel->addNodeWithID (inputNode, NodeModel::InputNodeID);
    graphModel->addNodeWithID (outputNode, NodeModel::OutputNodeID);

    graphModel->addNode (node1);
    graphModel->addNode (node2);
    graphModel->addNode (node3);

    graphModel->addConnection (*node2, 0, *node1, 0);
    graphModel->addConnection (*node2, 1, *node1, 1);
    graphModel->addConnection (*node1, 0, *node3, 0);
    graphModel->addConnection (*node1, 1, *node3, 1);
    graphModel->addConnection (*node3, 0, *outputNode, 0);
    graphModel->addConnection (*node3, 1, *outputNode, 1);

    graphModel->buildGraph ();
}