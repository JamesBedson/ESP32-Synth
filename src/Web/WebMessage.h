#pragma once
#include <string>

enum WebCommandType {
    SetParameter,
    GetParameter
};

struct WebMessage {
    WebCommandType type;
    std::string paramID;
    float value;
};
