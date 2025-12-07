#pragma once
#include <string>
#include <atomic>
#include "AudioParameterBase.h"

class AudioParameterTree;

template<typename T>
class AudioParameter : public AudioParameterBase {

public:

    static_assert(
        std::is_arithmetic<T>::value,
        "AudioParameter<T> must use an arithmetic type"
    );

    AudioParameter(
        const std::string& id, 
        T defaultValue
    ) : id(id), value(defaultValue) {
    }

    const std::string& getID() const override { return id; }

    void setValueFromFloat(float v) override {

        T newVal = static_cast<T>(v);
        T oldVal = value.exchange(newVal);

        if (newVal != oldVal && tree) {
            tree->notifyParameterChanged(this, v);
        }
    }

    float getValueAsFloat() const override {
        return static_cast<float>(value.load());
    }
    
    T get() const { return value.load(); }

    void set(T v) { 
        T oldVal = value.exchange(v);
        
        if (oldVal != v && tree) {
            tree->notifyParameterChanged(this, static_cast<float>(v));
        }
    }

    void attachTree(AudioParameterTree* t) {
        tree = t;
    }


private:
    std::string id;
    std::atomic<T> value;

    AudioParameterTree* tree = nullptr;
};