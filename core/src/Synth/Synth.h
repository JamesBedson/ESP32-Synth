#pragma once
#include <stdint.h>
#include "../Utils/Constants.h"
#include "../Parameters/AudioParameter.h"
#include "../Parameters/SmoothedAudioParameter.h"
#include <math.h>

class Synth 
{
public:
    AudioParameter<float> frequency {"synth.frequency", 440.f, 20.f, 10000.f};
    AudioParameter<float> amplitude {"synth.amplitude", 0.5f, 0.f, 1.f};

    SmoothedAudioParameter<float> smoothedAmplitude {amplitude, Constants::SAMPLE_RATE, 0.02f, SmoothingMode::Linear};
    SmoothedAudioParameter<float> smoothedFreq {frequency, Constants::SAMPLE_RATE, 0.0001f, SmoothingMode::Exponential};


    void noteOn(float freq);
    void noteOff();

    void renderAudio(
        float* buffer, 
        int numSamples,
        int numChannels
    );

private:
    bool active = false;
    double phase = 0.0;
};