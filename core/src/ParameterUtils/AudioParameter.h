#pragma once

#include <string>
#include <atomic>
#include <type_traits>
#include <utility>
#include "AudioParameterBase.h"

// Forward declaration of generator access
struct __ParamGenAccess;

template<typename T>
class AudioParameter : public AudioParameterBase
{
public:

    static_assert(
        std::is_arithmetic<T>::value,
        "AudioParameter<T> must use an arithmetic type"
    );

    const std::string& getID() const override { return id; }

    AudioParameter(const AudioParameter& other) = delete;
    AudioParameter& operator=(const AudioParameter& other) = delete;

    AudioParameter(AudioParameter&& other) noexcept
        : id(std::move(other.id)),
          value(),
          defaultValue(other.defaultValue),
          minValue(other.minValue),
          maxValue(other.maxValue)
    {
        value.store(other.value.load());
    }

    AudioParameter& operator=(AudioParameter&& other) noexcept
    {
        if (this != &other) {
            id = std::move(other.id);
            value.store(other.value.load());
            defaultValue = other.defaultValue;
            minValue = other.minValue;
            maxValue = other.maxValue;
        }
        return *this;
    }

    void setValueFromFloat(float v) override {
        T clamped = clamp(static_cast<T>(v));
        T oldVal = value.exchange(clamped);

        if (clamped != oldVal && onChange) {
            onChange(this, static_cast<float>(clamped));
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

    // Only generator is allowed to construct
    friend struct __ParamGenAccess;

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
    {}

    T clamp(T v) const {
        return v < minValue ? minValue : (v > maxValue ? maxValue : v);
    }

    std::string id;
    std::atomic<T> value;
    T defaultValue;
    T minValue;
    T maxValue;
};
