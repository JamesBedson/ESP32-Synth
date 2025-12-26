#pragma once
#include <string>

class IParameterListener {

public:
    bool suppressUIFeedback = false;
    virtual void onParameterChanged(int index, float value) = 0;
};