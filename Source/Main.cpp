#include "vld.h"
#include "../JuceLibraryCode/JuceHeader.h"
#define _CRTDBG_MAP_ALLOC
#include "MainComponent.h"
#include "Controller\AppController.h"
#include "LookAndFeel.h"

namespace Ananke {

class AudioWidgetsApplication : public JUCEApplication
{
public:
	AudioWidgetsApplication () {}

	const String getApplicationName () override { return ProjectInfo::projectName; }
	const String getApplicationVersion () override { return ProjectInfo::versionString; }
	bool moreThanOneInstanceAllowed () override { return true; }

	void initialise (const String& /*commandLine*/) override
	{
		LookAndFeel::setDefaultLookAndFeel (&lookAndFeel);

		if (!appController.initialiseAudioDevice ())
		{
			AlertWindow::showMessageBoxAsync (
				AlertWindow::WarningIcon,
				"Audio Device Error",
				"Could not open a suitable device or no device present");
			// TODO shutdown app
		}
		else
		{
			mainWindow = std::make_unique<MainWindow> (appController);
		}
	}

	void shutdown () override
	{
		mainWindow = nullptr;

#ifdef _DEBUG
		_CrtDumpMemoryLeaks ();
#endif
	}

	void systemRequestedQuit () override
	{
		quit ();
	}

	void anotherInstanceStarted (const String& /*commandLine*/) override
	{
	}

	class MainWindow : public DocumentWindow
	{
	public:
		MainWindow (AppController& _appController) :
			DocumentWindow ("Ananke", Colours::lightgrey, DocumentWindow::allButtons)
		{
			setContentOwned (new MainContentComponent (_appController), true);
			centreWithSize (getWidth (), getHeight ());
			setVisible (true);
		}

		void closeButtonPressed ()
		{
			JUCEApplication::getInstance ()->systemRequestedQuit ();
		}

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
	};

private:
	AppController appController;
	std::unique_ptr<MainWindow> mainWindow;
	AnankeLookAndFeel lookAndFeel;
};

START_JUCE_APPLICATION (AudioWidgetsApplication)

}
