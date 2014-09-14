#pragma once

#include "WidgetWrapper.h"

WidgetWrapper::WidgetWrapper ()
{
    setPath (createWidgetPath ());
}

WidgetWrapper::~WidgetWrapper ()
{

}

juce::Path WidgetWrapper::createWidgetPath ()
{
    juce::Path path;

    return path;
}