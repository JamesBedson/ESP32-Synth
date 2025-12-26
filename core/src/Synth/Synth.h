#pragma once
#include <stdint.h>
#include "../Utils/Constants.h"
#include "../Generated/SynthParams.h"
#include "../Infrastructure/IDspModule.h"
#include "../ParameterUtils/SmoothedAudioParameter.h"
#include "../ParameterUtils/SmoothingMode.h"
#include <math.h>

class Synth : public IDspModule
{
public:
    Synth();
    void registerParams(AudioParameterTree& tree) override;
    void noteOn(float freq);
    void noteOff();

    void renderAudio(
        float* buffer, 
        int numSamples,
        int numChannels
    );

    SmoothedAudioParameter<float> smoothedAmplitude;
    SmoothedAudioParameter<float> smoothedFreq;

private:
    SynthParams& params;
    bool active = false;
    double phase = 0.0;
};
