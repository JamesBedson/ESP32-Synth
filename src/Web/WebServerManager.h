#pragma once
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "WebMessageHandler.h"

class WebServerManager
{
public:
    WebServerManager(WebMessageHandler& handler) : server(80), ws("/ws"), messageHandler(handler)
    {
    }

    void begin()
    {
        Serial.println("Connecting to WiFi...");

        WiFi.mode(WIFI_STA); // Station mode (ESP32 joins your network)
        WiFi.begin(WIFI_SSID, WIFI_PASS);

        // Wait for connection
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(200);
            Serial.print(".");
        }

        Serial.println("\nWiFi connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());

        // Serve webpage
        server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request)
                  { request->send(200, "text/html", htmlPage); });

        ws.onEvent(
            [this](
                AsyncWebSocket *server,
                AsyncWebSocketClient *client,
                AwsEventType type,
                void *arg,
                uint8_t *data,
                size_t len)
            {
                this->onWsEvent(server, client, type, arg, data, len);
            });

        server.addHandler(&ws);
        server.begin();
        Serial.println("Web server started.");

        messageHandler.sendJSONToUI = [&](const std::string& json) {
            ws.textAll(json.c_str());
        };
    }

    void onWsEvent(
        AsyncWebSocket *server,
        AsyncWebSocketClient *client,
        AwsEventType type,
        void *arg,
        uint8_t *data,
        size_t len)
    {

        if (type == WS_EVT_CONNECT)
        {
            Serial.println("Websocket client connection received");
            client->text("Hello from ESP32 Server");
        }

        else if (type == WS_EVT_DISCONNECT)
        {
            Serial.println("Client disconnected");
        }

        else if (type == WS_EVT_DATA) {
            std::string payload((char*)data, len);
            Serial.print("WS message received: ");
            Serial.write(data, len);
            Serial.println();

            // Forward raw data to WebMessageHandler
            messageHandler.onRawWebSocketData((char*)data, len, client->id());

        }
    }

private:

    void sendJSON(const std::string& json) {
        
    }

    AsyncWebServer server;
    AsyncWebSocket ws;
    WebMessageHandler &messageHandler;

    const char *htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>ESP32 Synth Control</title>
<style>
  body { font-family: sans-serif; padding: 20px; }
  .label { margin-top: 20px; font-weight: bold; }
</style>
</head>
<body>

<h1>ESP32 Synth Controller</h1>

<div class="label">Amplitude</div>
<input type="range" id="amp" min="0" max="1" step="0.01" value="0.5">

<script>
let socket = new WebSocket("ws://" + window.location.host + "/ws");

// When we get JSON from ESP32
socket.onmessage = function(event) {
    let msg = JSON.parse(event.data);

    if (msg.type === "param" && msg.id === "synth.amplitude") {
        document.getElementById("amp").value = msg.value;
    }
};

// When slider moves → send JSON to ESP32
document.getElementById("amp").addEventListener("input", (e) => {
    let value = parseFloat(e.target.value);

    let msg = {
        type: "set",
        id: "synth.amplitude",
        value: value
    };

    socket.send(JSON.stringify(msg));
});
</script>

</body>
</html>
)rawliteral";

};