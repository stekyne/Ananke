#pragma once

#ifndef APPCONTROLLER_H_INCLUDED
#define APPCONTROLLER_H_INCLUDED

#include "../Source/AudioProcessingGraph.h"
#include "../View/WidgetWrapper.h"
#include "../JuceLibraryCode/JuceHeader.h"

class AppController :   public AudioIODeviceCallback,
                        public ChangeListener
{
public:
    AppController ();
    ~AppController ();

    APG::Graph& getGraphModel ()
    {
        return graphModel;
    }

    bool initialiseAudioDevice ();

    void AppController::audioDeviceIOCallback (
        const float** inputChannelData, int totalNumInputChannels,
        float **outputChannelData, int totalNumOutputChannels,
        int numSamples) override;

    void audioDeviceAboutToStart (AudioIODevice* device) override;
    void audioDeviceStopped () override;

    std::unique_ptr<AudioDeviceSelectorComponent> getSelector (
        int width = 500, int height = 400);

    void changeListenerCallback (ChangeBroadcaster* source);

private:
    void loadTestData ();

private:
    juce::ValueTree valueTree;
    OwnedArray<WidgetWrapper> widgets;
    APG::Graph graphModel;
    std::unique_ptr<AudioDeviceManager> deviceManager;
    std::unique_ptr<AudioFormatManager> formatManager;
};

#endif