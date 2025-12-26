#pragma once
#include <stddef.h>
#include "AudioParameter.h"
#include "ParamGenAccess.h"

// Wrapper for enum-based choice parameters backed by AudioParameter<int>.
template <typename EnumT, size_t N>
class ChoiceParameter {
public:
    ChoiceParameter(
        const char* id,
        EnumT defaultValue,
        const char* const (&choices)[N]
    )
        : param(__ParamGenAccess::make<int>(
              id,
              static_cast<int>(defaultValue),
              0,
              static_cast<int>(N - 1))),
          choices(choices)
    {}

    EnumT get() const {
        int v = param.get();
        if (v < 0 || v >= static_cast<int>(N))
            return static_cast<EnumT>(0);
        return static_cast<EnumT>(v);
    }

    void set(EnumT v) {
        param.set(static_cast<int>(v));
    }

    const char* name(EnumT v) const {
        int idx = static_cast<int>(v);
        if (idx < 0 || idx >= static_cast<int>(N))
            return "";
        return choices[idx];
    }

    AudioParameter<int>& raw() { return param; }
    const AudioParameter<int>& raw() const { return param; }

private:
    AudioParameter<int> param;
    const char* const (&choices)[N];
};
