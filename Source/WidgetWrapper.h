#pragma once

#ifndef WIDGETWRAPPER_H_INCLUDED
#define WIDGETWRAPPER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class WidgetWrapper : public juce::DrawablePath
{
public:
    WidgetWrapper ();
    ~WidgetWrapper ();

private:
    static juce::Path createWidgetPath ();
};

#endif