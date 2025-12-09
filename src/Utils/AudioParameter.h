#pragma once
#include <string>
#include <atomic>
#include "AudioParameterBase.h"

template<typename T>
class AudioParameter : public AudioParameterBase {

public:

    static_assert(
        std::is_arithmetic<T>::value,
        "AudioParameter<T> must use an arithmetic type"
    );

    AudioParameter(
        const std::string& id,
        T defaultValue,
        T minValue,
        T maxValue
    )
        : id(id),
          value(defaultValue),
          defaultValue(defaultValue),
          minValue(minValue),
          maxValue(maxValue)
    {
    }

    const std::string& getID() const override { return id; }

    void setValueFromFloat(float v) override {
        T clamped = clamp(static_cast<T>(v));
        T oldVal = value.exchange(clamped);

        if (clamped != oldVal && onChange) {
            onChange(this, clamped);
        }
    }

    float getValueAsFloat() const override {
        return static_cast<float>(value.load());
    }
    
    T get() const { return value.load(); }

    void set(T v) { 
        T clamped = clamp(v);
        T oldVal = value.exchange(clamped);
        
        if (oldVal != clamped && onChange) {
            onChange(this, static_cast<float>(clamped));
        }
    }

    float getNormalised() const {
        T v = value.load();
        return (v - minValue) / (maxValue - minValue);
    }

    void setNormalised(float norm) {
        float f = minValue + norm * (maxValue - minValue);
        setValueFromFloat(f);
    }

private:

    T clamp(T v) const {
        return v < minValue ? minValue : (v > maxValue ? maxValue : v);
    }
    std::string id;
    std::atomic<T> value;
    T defaultValue;
    T minValue;
    T maxValue;
};