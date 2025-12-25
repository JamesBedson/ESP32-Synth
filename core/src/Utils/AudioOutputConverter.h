#pragma once
#include <stdint.h>

class AudioOutputConverter {
public:
    static void floatToInt16(const float* in, int16_t* out, int count) {
        for (int i = 0; i < count; i++) {
            float s = in[i];
            if (s > 1.0f) s = 1.0f;
            if (s < -1.0f) s = -1.0f;
            out[i] = static_cast<int16_t>(s * 32767.0f);
        }
    }
};
