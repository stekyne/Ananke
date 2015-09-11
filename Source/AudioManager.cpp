#include "AudioManager.h"

AudioManager::AudioManager () :	
    deviceManager (std::make_unique<AudioDeviceManager>()),
    formatManager (std::make_unique<AudioFormatManager>())
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
        //deviceManager->addAudioCallback (this);
        //deviceManager->addChangeListener (this);
    }

    formatManager->registerBasicFormats ();
}

AudioManager::~AudioManager ()
{
    deviceManager->removeAllChangeListeners ();
    deviceManager->removeAudioCallback (this);
}

std::unique_ptr<AudioDeviceSelectorComponent> AudioManager::getSelector (
    int width, int height)
{
    std::unique_ptr<AudioDeviceSelectorComponent> deviceSelector =
         std::make_unique<AudioDeviceSelectorComponent> (
             *deviceManager, 0, 2, 0, 2, true, true, true, false);

    deviceSelector->setSize (width, height);
    return deviceSelector;
}
