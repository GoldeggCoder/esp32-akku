#ifndef MAIN_H
#define MAIN_H

#include "ota.h"
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include <bq769x0.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP280.h>

#define BMS_ADDRESS 0x00
#define TEMP_SENS_ADDRESS 0x76
#define DISPLAY_ADDRESS 0x3C

#define READ_TIMEOUT 100
#define SENDING_INTERVALL 1000
#define UPDATE_INTERVALL 100

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define ALERT_PIN 0
#define BOOT_PIN 0
#define PWR_PIN 23

#define DEBUG_SERIAL Serial
#define JSON_SERIAL Serial2

//#define ACTIVATE_OTA

#define ERROR_MAIN_LOG
#define DEBUG_MAIN_LOG
#define ERROR_MEAS_LOG
#define DEBUG_MEAS_LOG
#define ERROR_WIFI_LOG
#define DEBUG_WIFI_LOG
#define ERROR_OTA_LOG
#define DEBUG_OTA_LOG
#define ERROR_TEMP_LOG
#define DEBUG_TEMP_LOG
#define ERROR_DISPLAY_LOG
#define DEBUG_DISPLAY_LOG
#define ERROR_BMS_LOG
#define DEBUG_BMS_LOG

#ifdef ERROR_MAIN_LOG
#define ERROR_MAIN(fmt, ...) DEBUG_SERIAL.printf("ERROR-MAIN: " fmt "\n", ##__VA_ARGS__)
#else
#define ERROR_MAIN(fmt, ...)
#endif

#ifdef DEBUG_MAIN_LOG
#define DEBUG_MAIN(fmt, ...) DEBUG_SERIAL.printf("DEBUG-MAIN: " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_MAIN(fmt, ...)
#endif

#ifdef ERROR_MEAS_LOG
#define ERROR_MEAS(fmt, ...) DEBUG_SERIAL.printf("ERROR-MEAS: " fmt "\n", ##__VA_ARGS__)
#else
#define ERROR_MEAS(fmt, ...)
#endif

#ifdef DEBUG_MEAS_LOG
#define DEBUG_MEAS(fmt, ...) DEBUG_SERIAL.printf("DEBUG-MEAS: " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_MEAS(fmt, ...)
#endif

#ifdef ERROR_WIFI_LOG
#define ERROR_WIFI(fmt, ...) DEBUG_SERIAL.printf("ERROR-WIFI: " fmt "\n", ##__VA_ARGS__)
#else
#define ERROR_WIFI(fmt, ...)
#endif

#ifdef DEBUG_WIFI_LOG
#define DEBUG_WIFI(fmt, ...) DEBUG_SERIAL.printf("DEBUG-WIFI: " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_WIFI(fmt, ...)
#endif

#ifdef ERROR_OTA_LOG
#define ERROR_OTA(fmt, ...) DEBUG_SERIAL.printf("ERROR-OTA: " fmt "\n", ##__VA_ARGS__)
#else
#define ERROR_OTA(fmt, ...)
#endif

#ifdef DEBUG_OTA_LOG
#define DEBUG_OTA(fmt, ...) DEBUG_SERIAL.printf("DEBUG-OTA: " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_OTA(fmt, ...)
#endif

#ifdef ERROR_TEMP_LOG
#define ERROR_TEMP(fmt, ...) DEBUG_SERIAL.printf("ERROR-TEMP: " fmt "\n", ##__VA_ARGS__)
#else
#define ERROR_TEMP(fmt, ...)
#endif

#ifdef DEBUG_TEMP_LOG
#define DEBUG_TEMP(fmt, ...) DEBUG_SERIAL.printf("DEBUG-TEMP: " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_TEMP(fmt, ...)
#endif

#ifdef ERROR_DISPLAY_LOG
#define ERROR_DISPLAY(fmt, ...) DEBUG_SERIAL.printf("ERROR-DISPLAY: " fmt "\n", ##__VA_ARGS__)
#else
#define ERROR_DISPLAY(fmt, ...)
#endif

#ifdef DEBUG_DISPLAY_LOG
#define DEBUG_DISPLAY(fmt, ...) DEBUG_SERIAL.printf("DEBUG-DISPLAY: " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_DISPLAY(fmt, ...)
#endif

#ifdef ERROR_BMS_LOG
#define ERROR_BMS(fmt, ...) DEBUG_SERIAL.printf("ERROR-BMS: " fmt "\n", ##__VA_ARGS__)
#else
#define ERROR_BMS(fmt, ...)
#endif

#ifdef DEBUG_BMS_LOG
#define DEBUG_BMS(fmt, ...) DEBUG_SERIAL.printf("DEBUG-BMS: " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_BMS(fmt, ...)
#endif

#endif