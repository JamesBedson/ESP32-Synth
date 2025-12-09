#pragma once
#include <string>

enum class WebMessageType {
    SetParameter,
    GetParameter,
    Command,
    Param
};

struct WebMessage {
    WebMessageType type;
    std::string paramID;
    float value = 0.0f;
    std::string command;
    std::string args;
};
