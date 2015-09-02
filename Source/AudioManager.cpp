#include "AudioManager.h"

AudioManager::AudioManager ()
    :	deviceManager (new AudioDeviceManager),
        formatManager (new AudioFormatManager)
{
    const String result (deviceManager->initialise (0, 2, 0, true));

    // 'result' will be empty if 'initialise' succeeded in opening a device
    if (result.isNotEmpty ())
    {
        DBG ("Error: A device could not be opened.");
        AlertWindow::showMessageBoxAsync (  
            AlertWindow::WarningIcon,
            "Audio Device Error",
            "Could not open a suitable device or no device present");
    }
    else
    {
        deviceManager->addAudioCallback (this);
        deviceManager->addChangeListener (this);
    }

    formatManager->registerBasicFormats ();
}

AudioManager::~AudioManager ()
{
    deviceManager->removeAllChangeListeners ();
    deviceManager->removeAudioCallback (this);
}

ScopedPointer<AudioDeviceSelectorComponent> AudioManager::getSelector (
    int width, int height)
{
    ScopedPointer<AudioDeviceSelectorComponent> deviceSelector 
         (new AudioDeviceSelectorComponent (*deviceManager,
                                            0, 2, 0, 2, 
                                            true, true, true, false));

    deviceSelector->setSize (width, height);
    return deviceSelector;
}

void AudioManager::audioDeviceIOCallback (
    const float** inputChannelData, int totalNumInputChannels,
    float **outputChannelData, int totalNumOutputChannels,
    int numSamples)
{
    //jassert (totalNumInputChannels >= totalNumOutputChannels);

    //for (int channelNum = 0; channelNum < totalNumInputChannels; ++channelNum)
    //{
    //    AudioBuffer<float> inputBuffer (inputChannelData[channelNum], numSamples);
    //    AudioBuffer<float> outputBuffer (outputChannelData[channelNum], numSamples);

    //    graphModel->processGraph (inputBuffer, outputBuffer, numSamples);
    //}
}

void AudioManager::audioDeviceAboutToStart (AudioIODevice* /*device*/)
{

}

void AudioManager::audioDeviceStopped ()
{

}
