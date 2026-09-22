#ifndef SENSOR_H
#define SENSOR_H
#include <Arduino.h>
#include "config.h"
class SensorModule {
private:
  float temperature;
  float humidity;
  uint16_t soilMoistureRaw;
  uint8_t soilMoisturePercent;
  bool soilIsDry;
  bool soilSensorConnected; 
public:
  SensorModule() : temperature(0), humidity(0), soilMoistureRaw(0),
                   soilMoisturePercent(0), soilIsDry(false) {} 
  void init() {
    pinMode(SOIL_MOISTURE_AO_PIN, INPUT_ANALOG);
    pinMode(DHTPIN, INPUT_PULLUP);
  } 
  bool readDHT22() {
    uint8_t data[5] = {0};
    uint8_t bit = 7;
    uint8_t idx = 0;
    pinMode(DHTPIN, OUTPUT);
    digitalWrite(DHTPIN, LOW);
    delay(2);
    digitalWrite(DHTPIN, HIGH);
    delayMicroseconds(30);
    pinMode(DHTPIN, INPUT_PULLUP);
    uint16_t timeout = 0;
    while(digitalRead(DHTPIN) == HIGH) if(++timeout > 200) return false;
    while(digitalRead(DHTPIN) == LOW) if(++timeout > 200) return false;
    while(digitalRead(DHTPIN) == HIGH) if(++timeout > 200) return false;
    for (uint8_t i = 0; i < 40; i++) {
      while(digitalRead(DHTPIN) == LOW);
      uint32_t t = micros();
      while(digitalRead(DHTPIN) == HIGH);
      if ((micros() - t) > 40) data[idx] |= (1 << bit);
      if (bit == 0) { bit = 7; idx++; } else bit--;
    }
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
      humidity = ((data[0] << 8) + data[1]) * 0.1;
      temperature = (((data[2] & 0x7F) << 8) + data[3]) * 0.1;
      if (data[2] & 0x80) temperature = -temperature;
      return true;
    }
    return false;
  }
  void readSoilMoisture() {
    soilMoistureRaw = analogRead(SOIL_MOISTURE_AO_PIN); 
    const uint16_t DRY_VALUE = 1000;
    const uint16_t WET_VALUE = 750; 
    if (soilMoistureRaw >= DRY_VALUE) {
      soilMoisturePercent = 0;
    } else if (soilMoistureRaw <= WET_VALUE) {
      soilMoisturePercent = 100;
    } else {
      soilMoisturePercent = map(soilMoistureRaw, DRY_VALUE, WET_VALUE, 0, 100);
    } 
    soilIsDry = (soilMoisturePercent < 50);
  }
  
  float getTemperature() { return temperature; }
  float getHumidity() { return humidity; }
  bool isSoilDry() { return soilIsDry; }
  bool isSoilSensorConnected() { return soilSensorConnected; }
  uint16_t getSoilMoistureRaw() { return soilMoistureRaw; }
  uint8_t getSoilMoisturePercent() { return soilMoisturePercent; }
};
#endif

