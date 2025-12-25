#pragma once
#include "AudioParameter.h"
#include "ParamGenAccess.h"

struct Synthparams {
    AudioParameter<float> amplitude = __ParamGenAccess::make<float>("amplitude", 0.5f, 0.0f, 1.0f);
    AudioParameter<float> frequency = __ParamGenAccess::make<float>("frequency", 500.0f, 20.0f, 15000.0f);
};