#pragma once
#include "WebServerManager.h"

WebServerManager::WebServerManager(
    WebMessageHandler& handler
) : server(80), ws("/ws"), messageHandler(handler) {
    ws.onEvent(&WebServerManager::onEventStatic, this)
}

void WebServerManager::begin() {
    Serial.println("Connecting to WiFi...");

    WiFi.mode(WIFI_STA);  // Station mode (ESP32 joins your network)
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(200);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Serve webpage
    server.on("/", HTTP_GET, [&](AsyncWebServerRequest* request) {
        request->send(200, "text/html", htmlPage);
    });

    server.begin();
    Serial.println("Web server started.");
}

void WebServerManager::handleWebSocketMessage(
    void* arg,
    uint8_t* data,
    size_t len 
) {

}