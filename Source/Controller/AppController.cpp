#pragma once

#include "AppController.h"

AppController::AppController () :
    graphModel (new APG::Graph),
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
        DBG ("Error: An audio device could not be opened.");
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
    const float** inputChannelData, int totalNumInputChannels,
    float** outputChannelData, int totalNumOutputChannels,
    int numSamples)
{
    graphModel->processGraph (inputChannelData, totalNumInputChannels, 
                              outputChannelData, totalNumOutputChannels,
                              numSamples);
}

void AppController::audioDeviceAboutToStart (AudioIODevice* device)
{
    const auto sampleRate = (float)device->getCurrentSampleRate ();
    const auto blockSize = device->getCurrentBufferSizeSamples ();

    graphModel->setSettings (APG::Graph::Settings (sampleRate, blockSize, 50));    
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
    using namespace APG;

    auto gainNode = new GainNode (1);
    auto sawNode = new SawOSCNode (2);
    auto lowPassNode = new LowPassNode (3);

    graphModel->addNode (gainNode);
    graphModel->addNode (sawNode);
    graphModel->addNode (lowPassNode);

    graphModel->addConnection (Connection (*sawNode, 0, *gainNode, 0));
    graphModel->addConnection (Connection (*sawNode, 1, *gainNode, 1));
    graphModel->addConnection (Connection (*gainNode, 0, *lowPassNode, 0));
    graphModel->addConnection (Connection (*gainNode, 1, *lowPassNode, 1));

    graphModel->addConnection (Connection (lowPassNode->getID (), 0, Graph::AudioOutputID, 0));
    graphModel->addConnection (Connection (lowPassNode->getID (), 1, Graph::AudioOutputID, 1));

    graphModel->buildGraph ();
}