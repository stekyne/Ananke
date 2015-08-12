/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "MainComponent.h"
#include "Controller\AppController.h"
#include "LookAndFeel.h"

#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"

class AudioWidgetsApplication  : public JUCEApplication
{
public:
    AudioWidgetsApplication() {}

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return true; }

    void initialise (const String& /*commandLine*/) override
    {
		LookAndFeel::setDefaultLookAndFeel (&lookAndFeel);
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
            DocumentWindow ("Ananke",
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
	AnankeLookAndFeel lookAndFeel;
};

START_JUCE_APPLICATION (AudioWidgetsApplication)
