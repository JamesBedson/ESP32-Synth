#pragma once
#include "WebMessage.h"
#include "JSONWebMessage.h"
#include <functional>

class MessageBridge;

class WebMessageHandler {

public:
    WebMessageHandler() = default;

    std::function<void(const WebMessage& msg)> sendUpdateToBackend;
    std::function<void(const std::string& json)> sendJSONToUI;

    void onRawWebSocketData(
        const char* data,
        size_t len,
        uint32_t clientId)
    {
        WebMessage msg;
        if (!WebMessageJSON::parse(data, len, msg)) {
            Serial.println("JSON parse failed");
            return;
        }

        if (sendUpdateToBackend)
            sendUpdateToBackend(msg);
    }

    void sendMessageToUI(const WebMessage& msg)
    {
        if (!sendJSONToUI) return;

        std::string json = WebMessageJSON::serialize(msg);
        sendJSONToUI(json);
    }

    
private:
};