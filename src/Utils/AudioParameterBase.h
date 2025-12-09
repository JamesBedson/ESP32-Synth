#pragma once
#include <string>
#include <functional>

class AudioParameterBase {

public:
    virtual ~AudioParameterBase() = default;

    virtual const std::string& getID() const = 0;

    virtual void setValueFromFloat(float v) = 0;

    virtual float getValueAsFloat() const = 0;

    std::function<void(AudioParameterBase*, float)> onChange;
};