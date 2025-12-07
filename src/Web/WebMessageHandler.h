#pragma once
#include <atomic>
#include <queue>
#include <mutex>
#include "WebMessage.h"

class MessageBridge;

class WebMessageHandler {

public:
    WebMessageHandler(
        MessageBridge& bridge
    ) : bridge (bridge) {
    } 

    void onWebSocketMessage(const WebMessage& msg) {
        bridge.handle(msg);
    }

    void sendParameterUpdate(const std::string& id, float value) {

    }


private:
    MessageBridge& bridge;
};