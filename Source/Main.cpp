/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"
#include "AppController.h"
#include <memory>

class AudioWidgetsApplication  : public JUCEApplication
{
public:
    AudioWidgetsApplication() {}

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return true; }

    void initialise (const String& /*commandLine*/) override
    {
        appController = std::make_shared<AppController> ();
        mainWindow = new MainWindow (appController);
    }

    void shutdown() override
    {
        mainWindow = nullptr;
        appController.reset ();
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const String& /*commandLine*/) override
    {
    }

    class MainWindow : public DocumentWindow
    {
    public:
        MainWindow (std::shared_ptr<AppController>& _appController)  : 
            DocumentWindow ("MainWindow",
                            Colours::lightgrey,
                            DocumentWindow::allButtons)
        {
            setContentOwned (new MainContentComponent (_appController), true);

            centreWithSize (getWidth(), getHeight());
            setVisible (true);
        }

        void closeButtonPressed()
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::shared_ptr<AppController> appController;
    ScopedPointer<MainWindow> mainWindow;
};

START_JUCE_APPLICATION (AudioWidgetsApplication)
