#include "main.h"

const char* ssid = "WalterWifi";
const char* password = "UI2024";
const char* otaHostname = "ESP32-OTA";
const char* otaPassword = "DaCar2425";

bq769x0 bms(bq76920, BMS_ADDRESS);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_BMP280 temp_sensor(&Wire);
AsyncWebServer server(80);

unsigned long long lasttime_send = 0, lasttime_update = 0;
bool bms_found = false, temp_sensor_found = false, display_found = false;
String input_buffer;

typedef struct message_t {
  char status[20];
} message_t;
message_t myMessage;
esp_now_peer_info_t peerInfo;
uint8_t chargerAddress[] = {0x08, 0x3A, 0xF2, 0xA8, 0xD9, 0x28};

struct measurement_t {
  String status;
  int soc;
  bool soc_valid;
  int target_soc;
  bool target_soc_valid;
  float voltage;
  bool voltage_valid;
  float current;
  bool current_valid;
  float temperature;
  bool temperature_valid;
  float voltage_cell[5];
  bool voltage_cell_valid[5];
};
struct measurement_t measurement;

void I2C_scanner();
void display_display();
void send_json();

void setup() {
    DEBUG_SERIAL.begin(115200);
    JSON_SERIAL.begin(115200);

    //WiFi.mode(WIFI_STA);

    /*if (esp_now_init() != ESP_OK) {
      ERROR_MAIN("Error initializing ESP-NOW");
    }
    memcpy(peerInfo.peer_addr, chargerAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      ERROR_MAIN("Failed to add peer for ESP-NOW");
    }*/

    #ifdef ACTIVATE_OTA
      setupWiFi(ssid, password);
      setupOTA(otaHostname, otaPassword);
    #endif

    // Check if BMS (BQ76920) is available
    /*if(bms.begin(ALERT_PIN, BOOT_PIN) == 0) {
      bms_found = true;
      DEBUG_BMS("BMS (BQ76920) found");
      bms.setTemperatureLimits(-20, 45, 0, 45);
      bms.setShuntResistorValue(5);
      bms.setShortCircuitProtection(14000, 200);  // delay in us
      bms.setOvercurrentChargeProtection(8000, 200);  // delay in ms
      bms.setOvercurrentDischargeProtection(8000, 320); // delay in ms
      bms.setCellUndervoltageProtection(2600, 2); // delay in s
      bms.setCellOvervoltageProtection(3650, 2);  // delay in s

      bms.setBalancingThresholds(0, 3300, 20);  // minIdleTime_min, minCellV_mV, maxVoltageDiff_mV
      bms.setIdleCurrentThreshold(100);
      bms.enableAutoBalancing();
      bms.enableDischarging();
    }
    else {
      ERROR_BMS("Failed to find BMS (BQ76920)");
    }*/

    // Activate PWR Pin for Display (SSD1306) and Temp Sensor (BMP280)
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, HIGH);
    delay(10);

    // Check if Temp Sensor (BMP280) is available
    if (temp_sensor.begin(TEMP_SENS_ADDRESS, 88)) {
      temp_sensor_found = true;
      DEBUG_TEMP("Temp Sensor (BMP280) found");
    }
    else {
      ERROR_TEMP("Failed to find Temp Sensor (BMP280)");
    }

    // Check if Display (SSD1306) is available
    if(display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS)) {
      Wire.beginTransmission(DISPLAY_ADDRESS);
      if(Wire.endTransmission() == 0) {
        display_found = true;
        DEBUG_DISPLAY("Display (SSD1306) found");
        display.clearDisplay();
      }
      else {
        ERROR_DISPLAY("Failed to find Display (SSD1306)");
      }
    }
    else {
      ERROR_DISPLAY("Failed to find Display (SSD1306)");
    }

    measurement.status = "Discharging";
    measurement.voltage_cell[0] = 2.98;
    measurement.voltage_cell[1] = 3.07;
    measurement.voltage_cell[2] = 3.01;
    measurement.voltage_cell[3] = 3.13;
    measurement.voltage_cell[4] = 3.04;
    measurement.voltage_cell_valid[0] = true;
    measurement.voltage_cell_valid[1] = true;
    measurement.voltage_cell_valid[2] = true;
    measurement.voltage_cell_valid[3] = true;
    measurement.voltage_cell_valid[4] = true;
    measurement.soc = 5;
    measurement.soc_valid = true;
    measurement.target_soc = 0;
    measurement.target_soc_valid = true;
    measurement.voltage = 18.65;
    measurement.voltage_valid = true;
    measurement.current = 12;
    measurement.current_valid = true;
}

void loop() {
    #ifdef ACTIVATE_OTA
      handleOTA();
    #endif

    if(measurement.soc >= measurement.target_soc) {
      measurement.status = "Discharging";
      measurement.target_soc = 0;
      measurement.target_soc_valid = true;
      //TODO: end charging
      /*
      if(bms_found) {
        if(bms.enableDischarging()) {
          measurement.status = "Discharging";
          measurement.target_soc = 0;
        }
      }
      */
    }

    if(JSON_SERIAL.available()) {
      input_buffer = JSON_SERIAL.readStringUntil('!');
      DEBUG_MAIN("JSON: %s", input_buffer.c_str());
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, input_buffer);
      if(error) {
        ERROR_MAIN("JSON parse error: %s", error.c_str());
      }
      else {
        const char* status = doc["status"];
        int target_soc = doc["target_soc"];
        if(status) {
          if(String(status) == "Charging") {
            if(target_soc > measurement.soc && target_soc > 0 && target_soc < 100) {
              measurement.target_soc = target_soc;
              measurement.target_soc_valid = true;
              if(measurement.status == "Discharging") {
                /*strcpy(myMessage.status, "Charging");
                esp_err_t result = esp_now_send(chargerAddress, (uint8_t *) &myMessage, sizeof(myMessage));
                if (result == ESP_OK) {
                  DEBUG_MAIN("ESP-NOW: Sent with success");
                }
                else {
                  ERROR_MAIN("ESP-NOW: Error sending the data");
                }*/
                measurement.status = "Charging";
                /*
                if(bms_found) {
                  if(bms.enableCharging()) {
                    measurement.status = "Charging";
                  }
                }
                */
              }
            }
            
          }
          /*else if(String(status) == "Discharging") {
            if(measurement.status == "Charging") {
              measurement.status = "Discharging";
              //bms.disableCharging();
              //if(bms.enableDischarging()) {
              //  measurement.status = "Discharging";
              //}
            }
          }*/
          DEBUG_MAIN("Status: %s", measurement.status);
          DEBUG_MAIN("SOC: %d", measurement.soc);
          DEBUG_MAIN("Target SOC: %d", measurement.target_soc);
        }
      }
    }

    if(millis() - lasttime_send >= SENDING_INTERVALL) {
      lasttime_send = millis();

      if(temp_sensor_found) {
        measurement.temperature_valid = true;
        measurement.temperature = temp_sensor.readTemperature();
      }
      else {
        measurement.temperature_valid = false;
        measurement.temperature = 0;
      }

      if(bms_found) {
        bms.update();
        bms.printRegisters();
      }

      //TODO: read out real battery data
      if(measurement.status == "Charging") {
        if(measurement.soc < 100) {
          measurement.soc++;
        }
      }
      else if(measurement.status == "Discharging") {
        if(measurement.soc > 5) {
          measurement.soc--;
        }
      }
      
      send_json();

      if(display_found) {
        display_display();
      }

      /*if(measurement.temperature_valid)
        DEBUG_MEAS("Temperature = %.2f °C", measurement.temperature);
      else
        ERROR_MEAS("Temperature = NaN °C");*/
    }

    if(millis() - lasttime_update > UPDATE_INTERVALL) {
      lasttime_update = millis();
      if(bms_found) {
        bms.update();
      }
    }
}

//TODO: implement the target_soc as a line at the battery visualizer
void display_display() {
  // General Display Settings
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  
  // Set the status
  display.printf("Status: %s", measurement.status);

  // Display the cell voltages
  for(int i = 0; i < 5; i++) {
    display.drawCircle(80, 15+(i*10), 5, WHITE);
    display.setCursor(78, 12+(i*10));
    display.printf("%c", 49+i);
    display.setCursor(90, 12+(i*10));
    if(measurement.voltage_cell_valid[i])
      display.printf("%.2fV", measurement.voltage_cell[i]);
    else
      display.printf("NaN V");
  }

  // Display the battery visualizer
  display.drawRect(53, 16, 10, 5, WHITE);
  display.drawRoundRect(48, 20, 20, 40, 3, WHITE);
  if(measurement.target_soc != 0) {
    int height_target_soc = (int)(38.0*(measurement.target_soc/100.0));
    display.drawLine(48, 59-height_target_soc, 67, 59-height_target_soc, WHITE);
  }
  if(measurement.soc_valid) {
    int height_soc = (int)(38.0*(measurement.soc/100.0));
    display.fillRoundRect(48, 59-height_soc, 20, height_soc, 3, WHITE);
  }  

  // Display the SOC in %
  display.setCursor(0, 12);
  display.setTextSize(2);
  if(measurement.soc_valid)
    display.printf("%d%%", measurement.soc);
  else
    display.printf("NaN%%");
  display.setTextSize(1);

  // Display voltage, current and temperature of battery pack
  display.setCursor(0, 32);
  if(measurement.voltage_valid)
    display.printf("%.2fV", measurement.voltage);
  else
    display.printf("NaN V");
  display.setCursor(0, 42);
  if(measurement.current_valid) {
    if(measurement.current < 100.0)
      display.printf("%.2fmA", measurement.current);
    else if(measurement.current < 1000.0)
      display.printf("%.1fmA", measurement.current);
    else
      display.printf("%.2fA", (measurement.current/1000.0));
  }
  else
    display.printf("NaN A");
  display.setCursor(0, 52);
  if(measurement.temperature_valid)
    display.printf("%.2f%cC", measurement.temperature, 247);
  else
    display.printf("NaN %cC", 247);

  // Effect changes
  display.display();
}

void send_json() {
  JsonDocument doc;
  doc["car"] = "Car0";
  doc["status"] = measurement.status;

  doc["soc"] = measurement.soc;
  doc["soc_unit"] = "%";
  doc["soc_valid"] = measurement.soc_valid;

  doc["target_soc"] = measurement.target_soc;
  doc["target_soc_unit"] = "%";
  doc["target_soc_valid"] = measurement.target_soc_valid;

  doc["temperature"] = measurement.temperature;
  doc["temperature_unit"] = "grad C";
  doc["temperature_valid"] = measurement.temperature_valid;

  doc["voltage"] = measurement.voltage;
  doc["voltage_unit"] = "V";
  doc["voltage_valid"] = measurement.voltage_valid;

  doc["current"] = measurement.current;
  doc["current_unit"] = "mA";
  doc["current_valid"] = measurement.current_valid;

  doc["voltage_cell_0"] = measurement.voltage_cell[0];
  doc["voltage_cell_0_unit"] = "V";
  doc["voltage_cell_0_valid"] = measurement.voltage_cell_valid[0];

  doc["voltage_cell_1"] = measurement.voltage_cell[1];
  doc["voltage_cell_1_unit"] = "V";
  doc["voltage_cell_1_valid"] = measurement.voltage_cell_valid[1];

  doc["voltage_cell_2"] = measurement.voltage_cell[2];
  doc["voltage_cell_2_unit"] = "V";
  doc["voltage_cell_2_valid"] = measurement.voltage_cell_valid[2];

  doc["voltage_cell_3"] = measurement.voltage_cell[3];
  doc["voltage_cell_3_unit"] = "V";
  doc["voltage_cell_3_valid"] = measurement.voltage_cell_valid[3];

  doc["voltage_cell_4"] = measurement.voltage_cell[4];
  doc["voltage_cell_4_unit"] = "V";
  doc["voltage_cell_4_valid"] = measurement.voltage_cell_valid[4];

  String jsonString;
  serializeJson(doc, jsonString);

  //DEBUG_SERIAL.println(jsonString);

  JSON_SERIAL.println(jsonString);
}

void I2C_scanner() {
  Wire.begin();
  byte error, address;
  int nDevices;
  DEBUG_SERIAL.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      DEBUG_SERIAL.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      DEBUG_SERIAL.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      DEBUG_SERIAL.print("Unknow error at address 0x");
      if (address<16) {
        DEBUG_SERIAL.print("0");
      }
      DEBUG_SERIAL.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    DEBUG_SERIAL.println("No I2C devices found\n");
  }
  else {
    DEBUG_SERIAL.println("done\n");
  }
}
