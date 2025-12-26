#pragma once
#include "../ParameterUtils/AudioParameter.h"
#include "../ParameterUtils/ParamGenAccess.h"

class SynthParams {
public:
    static SynthParams& instance() {
        static SynthParams inst;
        return inst;
    }

    SynthParams(const SynthParams&) = delete;
    SynthParams& operator=(const SynthParams&) = delete;
    SynthParams(SynthParams&&) = delete;
    SynthParams& operator=(SynthParams&&) = delete;

private:
    SynthParams() = default;
public:
    AudioParameter<float> amplitude = __ParamGenAccess::make<float>("synth.amplitude", 0.5f, 0.0f, 1.0f);
    AudioParameter<float> frequency = __ParamGenAccess::make<float>("synth.frequency", 440.0f, 20.0f, 15000.0f);
};