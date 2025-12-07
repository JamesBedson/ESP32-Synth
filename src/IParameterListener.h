#pragma once
#include <string>

class IParameterListener {

public:
    virtual void onParameterChanged(int index, float value) = 0;
};