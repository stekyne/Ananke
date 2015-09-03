#ifndef __AUDIOMANAGER_23ADECB__
#define __AUDIOMANAGER_23ADECB__

#include "../JuceLibraryCode/JuceHeader.h"

class AudioManager	:	public ChangeListener,
                        public ChangeBroadcaster,
                        public AudioIODeviceCallback
{
public:
    AudioManager ();
    virtual ~AudioManager ();

    void changeListenerCallback (ChangeBroadcaster*) {}

    ScopedPointer<AudioDeviceSelectorComponent> getSelector (
        int width = 500, int height = 400);

    double getCPU () const 
    { 
        return deviceManager->getCpuUsage (); 
    }

    AudioFormatManager* getFormatManager () const
    {
        return formatManager;
    }

private:
    ScopedPointer<AudioDeviceManager> deviceManager;
    ScopedPointer<AudioFormatManager> formatManager;
    ScopedPointer<MixerAudioSource> mixerSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioManager);
};

#endif