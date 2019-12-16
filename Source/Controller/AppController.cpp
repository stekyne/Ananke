#pragma once

#include "AppController.h"
#include "../Source/Graph/NodeTypes/GainNode.h"
#include "../Source/Graph/NodeTypes/LowPass.h"
#include "../Source/Graph/NodeTypes/SawOsc.h"

namespace Ananke {

AppController::AppController () :
	valueTree (juce::Identifier ("audioWidgets")),
	deviceManager (std::make_unique<AudioDeviceManager> ()),
	formatManager (std::make_unique<AudioFormatManager> ())
{
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
	graphModel.processGraph (inputChannelData, totalNumInputChannels,
							 outputChannelData, totalNumOutputChannels,
							 numSamples);
}

void AppController::audioDeviceAboutToStart (AudioIODevice* device)
{
	const auto sampleRate = (float)device->getCurrentSampleRate ();
	const auto blockSize = device->getCurrentBufferSizeSamples ();
	const auto inputChannels = device->getActiveInputChannels ().getHighestBit () + 1;
	const auto outputChannels = device->getActiveOutputChannels ().getHighestBit () + 1;

	graphModel.setSettings (Graph::Settings (sampleRate, blockSize, 32, inputChannels, outputChannels));
	loadTestData ();
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
	using namespace Ananke;

	auto gainNode = graphModel.createNode<GainNode> ();
	auto sawNode = graphModel.createNode<SawOSCNode> ();
	auto lowPassNode = graphModel.createNode<LowPassNode> ();

	gainNode->setGain (0.4);
	sawNode->setFrequency (50.f);
	lowPassNode->setCutoff (500.f);

	graphModel.addConnection (Connection (*sawNode, 0, *gainNode, 0));
	graphModel.addConnection (Connection (*sawNode, 1, *gainNode, 1));
	graphModel.addConnection (Connection (*gainNode, 0, *lowPassNode, 0));
	graphModel.addConnection (Connection (*gainNode, 1, *lowPassNode, 1));

	graphModel.addConnection (Connection (lowPassNode->getID (), 0, Graph::AudioOutputID, 0));
	graphModel.addConnection (Connection (lowPassNode->getID (), 1, Graph::AudioOutputID, 1));

	graphModel.buildGraph ();
}

}