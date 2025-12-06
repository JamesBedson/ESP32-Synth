#pragma once
#include <math.h>

namespace MidiUtils
{
    inline float midiToFreq(uint8_t note)
    {
        return 440.0f * powf(2.0f, (note - 69) / 12.0f);
    }
}
