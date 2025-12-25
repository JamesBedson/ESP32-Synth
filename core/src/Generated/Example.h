#pragma once
#include "AudioParameter.h"
#include "ParamGenAccess.h"

struct Example {
    AudioParameter<bool> isPlaying = __ParamGenAccess::make<bool>("isPlaying", false, 0, 1);
    AudioParameter<float> bpm = __ParamGenAccess::make<float>("bpm", 120.0f, 20.0f, 300.0f);
    AudioParameter<int> voices = __ParamGenAccess::make<int>("voices", 8, 1, 64);
};