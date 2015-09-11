#ifndef __AUDIOMANAGER_23ADECB__
#define __AUDIOMANAGER_23ADECB__

#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"

class AudioManager	:	public ChangeListener,
                        public ChangeBroadcaster,
                        public AudioIODeviceCallback
{
public:
    AudioManager ();
    virtual ~AudioManager ();

    void changeListenerCallback (ChangeBroadcaster*) {}

    std::unique_ptr<AudioDeviceSelectorComponent> getSelector (
        int width = 500, int height = 400);

    double getCPU () const 
    { 
        return deviceManager->getCpuUsage (); 
    }

    AudioFormatManager* getFormatManager () const
    {
        return formatManager.get();
    }

private:
    std::unique_ptr<AudioDeviceManager> deviceManager;
    std::unique_ptr<AudioFormatManager> formatManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioManager);
};

#endif