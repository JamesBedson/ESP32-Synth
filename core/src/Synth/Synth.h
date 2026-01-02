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

private:
    SynthParams& params;
    SmoothedAudioParameter<float> smoothedAmplitude;
    SmoothedAudioParameter<float> smoothedFreq;
    bool active = false;
    double phase = 0.0;
};
