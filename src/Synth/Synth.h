#pragma once
#include <stdint.h>
#include "../Utils/Constants.h"
#include <math.h>

class Synth 
{
public:
    void noteOn(float freq);
    void noteOff();

    void renderAudio(
        int16_t* buffer, 
        int numSamples
    );

private:
    bool active = false;
    float phase = 0.0f;
    float frequency = 0.0f;
};