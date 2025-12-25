#pragma once
#include <stdint.h>

namespace Constants
{
    static constexpr int BUFFER_SIZE = 64;
    static constexpr int SAMPLE_RATE = 44100;
    static constexpr int CHANNELS = 2;

    static constexpr uint8_t MAX_STEP_SIZE = 64;
    static constexpr uint8_t MAX_MIDI_RANGE = 127;
}