#pragma once

#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

#include "AudioBuffer.h"

namespace Utilities
{

    static void fillWithDC (AudioBuffer<float>& buffer, float gain)
    {
        for (int i = 0; i < buffer.getBufferSize (); ++i)
        {
            buffer[i] = gain;
        }
    }

}

#endif
