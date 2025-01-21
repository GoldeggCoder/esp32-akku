#include "ota.h"

void setupWiFi(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);

    DEBUG_WIFI("Connecting to Wi-Fi...");

    while (WiFi.status() != WL_CONNECTED);

    DEBUG_WIFI("Wi-Fi connected!");
    DEBUG_WIFI("SSID: %s", WiFi.SSID().c_str());
    DEBUG_WIFI("IP address: %s", WiFi.localIP().toString());
    DEBUG_WIFI("MAC address: %s", WiFi.macAddress().c_str());
}

void setupOTA(const char* hostname, const char* password) {
    ArduinoOTA.setHostname(hostname);
    ArduinoOTA.setPassword(password);

    ArduinoOTA.onStart([]() {
        String type = ArduinoOTA.getCommand() == U_FLASH ? "sketch" : "filesystem";
        DEBUG_OTA("OTA Update Start: %s", type);
    });
    ArduinoOTA.onEnd([]() {
        DEBUG_OTA("OTA Update Complete");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        DEBUG_OTA("Progress: %u%%\r", (progress * 100) / total);
    });
    ArduinoOTA.onError([](ota_error_t error) {
        ERROR_OTA("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) ERROR_OTA("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) ERROR_OTA("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) ERROR_OTA("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) ERROR_OTA("Receive Failed");
        else if (error == OTA_END_ERROR) ERROR_OTA("End Failed");
    });

    ArduinoOTA.begin();

    DEBUG_OTA("OTA Ready");
}

void handleOTA() {
    ArduinoOTA.handle();
}