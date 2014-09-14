#pragma once

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <memory>

class AppController;

class MainContentComponent   : public Component
{
public:
    MainContentComponent (std::shared_ptr<AppController> appController);
    ~MainContentComponent ();

    void paint (Graphics&);
    void resized ();

private:
    std::shared_ptr <AppController> appController;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif