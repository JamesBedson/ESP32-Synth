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
        struct Builder : public AudioParameter<T> {
            Builder(const char* i, T d, T mn, T mx)
                : AudioParameter<T>(i, d, mn, mx) {}
        };

        return Builder(id, def, min, max);
    }
};
