#pragma once
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "WebMessageHandler.h"

class WebServerManager 
{    
public:
    WebServerManager(WebMessageHandler& handler);

    void begin();
    void handleWebSocketMessage(
        void* arg,
        uint8_t* data,
        size_t len 
    );

private:

    AsyncWebServer server;
    AsyncWebSocket ws;
    WebMessageHandler& messageHandler;
    
    const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head><title>ESP32 Synth</title></head>
<body>
<h1>ESP32 Synth is Running!</h1>
<p>If you see this, your web server works.</p>
</body>
</html>
)rawliteral";

};