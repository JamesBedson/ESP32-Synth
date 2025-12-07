#pragma once
#include <string>

class AudioParameterBase {

public:
    virtual ~AudioParameterBase() = default;

    virtual std::string getID() const = 0;

    virtual void setValueFromFloat(float v) = 0;

    virtual float getValueAsFloat() const = 0;

};