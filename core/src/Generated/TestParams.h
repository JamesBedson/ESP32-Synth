#pragma once
#include "../ParameterUtils/AudioParameter.h"
#include "../ParameterUtils/ParamGenAccess.h"
#include "../ParameterUtils/ChoiceParameter.h"

class TestParams {
public:
    static TestParams& instance() {
        static TestParams inst;
        return inst;
    }

    TestParams(const TestParams&) = delete;
    TestParams& operator=(const TestParams&) = delete;
    TestParams(TestParams&&) = delete;
    TestParams& operator=(TestParams&&) = delete;

private:
    TestParams() = default;
public:
    AudioParameter<float> amplitude = __ParamGenAccess::make<float>("synth.amplitude", 0.5f, 0.0f, 1.0f);
    AudioParameter<float> frequency = __ParamGenAccess::make<float>("synth.frequency", 440.0f, 20.0f, 15000.0f);
    AudioParameter<bool> enabled = __ParamGenAccess::make<bool>("synth.enabled", true, 0, 1);
    enum class waveformChoice : int {
        Sine = 0,
        Saw = 1,
        Square = 2,
        Triangle = 3,
        Noise = 4,
    };

    static constexpr const char* waveform_choices[] = {
        "Sine",
        "Saw",
        "Square",
        "Triangle",
        "Noise",
    };

    ChoiceParameter<waveformChoice, 5> waveform = {"synth.waveform", waveformChoice::Saw, waveform_choices};
};