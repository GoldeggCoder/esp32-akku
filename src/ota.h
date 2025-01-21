#ifndef OTA_H
#define OTA_H

#include "main.h"

void setupWiFi(const char* ssid, const char* password);
void setupOTA(const char* hostname, const char* password);
void handleOTA();

#endif