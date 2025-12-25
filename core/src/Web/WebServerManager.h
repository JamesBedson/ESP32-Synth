#pragma once
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "WebMessageHandler.h"
#include <LittleFS.h>

class WebServerManager
{
public:
    WebServerManager(WebMessageHandler& handler) : server(80), ws("/ws"), messageHandler(handler)
    {
        // Mount LittleFS in constructor
        if (!LittleFS.begin(true)) {
            Serial.println("❌ LittleFS Mount Failed");
        } else {
            Serial.println("✅ LittleFS mounted successfully");
        }
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

        ws.onEvent(
            [this](
                AsyncWebSocket* server,
                AsyncWebSocketClient* client,
                AwsEventType type,
                void *arg,
                uint8_t *data,
                size_t len)
            {
                this->onWsEvent(server, client, type, arg, data, len);
            });

        server.addHandler(&ws);

        server.serveStatic("/", LittleFS, "/")
            .setDefaultFile("index.html")
            .setCacheControl("no-cache");

        server.onNotFound([](AsyncWebServerRequest* request) {
            Serial.printf("Not found: %s\n", request->url().c_str());
            request->send(404, "text/plain", "Not Found");
        });

        server.begin();
        Serial.println("Web server started.");
        Serial.print("Visit: http://");
        Serial.println(WiFi.localIP());

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
            //client->text("Hello from ESP32 Server");
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
};
