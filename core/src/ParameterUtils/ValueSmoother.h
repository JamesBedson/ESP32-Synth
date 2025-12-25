#pragma once
#include <cmath>
#include <algorithm>
#include "SmoothingMode.h"

template<typename T>
class ValueSmoother
{
public:
    ValueSmoother() = default;

    void reset(int sampleRate, float smoothingSeconds, SmoothingMode mode)
    {
        this->mode = mode;

        if (smoothingSeconds <= 0.0f)
        {
            step = T(1);
            return;
        }

        const float samples = smoothingSeconds * sampleRate;

        switch (mode)
        {
            case SmoothingMode::Linear:
                step = T(1) / samples;
                break;

            case SmoothingMode::Exponential:
                coeff = std::exp(-1.0f / samples);
                break;
        }
    }

    void setCurrentAndTarget(T value)
    {
        current = value;
        target = value;
    }

    void setTarget(T value)
    {
        target = value;
    }

    T getNextValue()
    {
        if (current == target)
            return current;

        switch (mode)
        {
            case SmoothingMode::Linear:
                return processLinear();

            case SmoothingMode::Exponential:
                return processExponential();
        }

        return current;
    }

private:

    T current = 0;
    T target  = 0;

    // Linear
    T step = 0;

    // Exponential
    float coeff = 0.0f;

    SmoothingMode mode = SmoothingMode::Linear;

    T processLinear()
    {
        const T delta = target - current;

        if (std::abs(delta) <= step)
            current = target;
        else
            current += (delta > 0 ? step : -step);

        return current;
    }

    T processExponential()
    {
        current = T(coeff * current + (1.0f - coeff) * target);

        // snap when very close
        if (std::abs(current - target) < T(1e-6))
            current = target;

        return current;
    }
};
