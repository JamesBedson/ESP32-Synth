#pragma once
#include "AudioParameter.h"
#include "ValueSmoother.h"
#include "SmoothingMode.h"

template<typename T>
class SmoothedAudioParameter {

public:
    SmoothedAudioParameter(
        AudioParameter<T>& audioParameter,
        int sampleRate,
        float smoothingSeconds,
        SmoothingMode mode
    ) : source(audioParameter) 
    {
        smoother.reset(sampleRate, smoothingSeconds, mode);
        smoother.setCurrentAndTarget(audioParameter.get());
        lastRawValue = audioParameter.get();
    }

    T getNextValue()
    {
        T raw = source.get();

        if (raw != lastRawValue) {
            smoother.setTarget(raw);
            lastRawValue = raw;
        }

        return smoother.getNextValue();
    }

    T getRawValue() const 
    { 
        return source.get(); 
    }

private:

    AudioParameter<T>& source;
    ValueSmoother<T> smoother;
    T lastRawValue;
};
