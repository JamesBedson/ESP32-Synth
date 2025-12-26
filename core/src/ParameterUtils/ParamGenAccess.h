#pragma once
#include "AudioParameter.h"

struct __ParamGenAccess
{
    template<typename T>
    static AudioParameter<T> make(
        const char* id,
        T def,
        T min,
        T max
    )
    {
        return AudioParameter<T>(id, def, min, max);
    }
};
