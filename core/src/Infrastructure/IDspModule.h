#pragma once
#include "../ParameterUtils/AudioParameterTree.h"

class IDspModule {
public:
    virtual ~IDspModule() = default;
    virtual void registerParams(AudioParameterTree& tree) = 0;
};
