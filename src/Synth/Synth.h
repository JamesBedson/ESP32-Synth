#pragma once
#include <stdint.h>
#include "../Utils/Constants.h"
#include "../Utils/AudioParameter.h"
#include <math.h>

class Synth 
{
public:
    AudioParameter<float> frequency {"synth.frequency", 440.f, 20.f, 10000.f};
    AudioParameter<float> amplitude {"synth.amplitude", 0.5f, 0.f, 1.f};

    void noteOn(float freq);
    void noteOff();

    void renderAudio(
        float* buffer, 
        int numSamples,
        int numChannels
    );

private:
    bool active = false;
    float phase = 0.0f;
};