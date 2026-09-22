#include <Arduino.h>
#include "config.h"
#include "sensor.h"
#include "relay.h"
#include "display.h"
#include "scheduler.h"
#include "uart_protocol.h"
SensorModule sensor;
RelayModule relay;
DisplayModule display; 
SchedulerModule scheduler;  
UARTProtocol uart(&Serial); 
unsigned long lastRead = 0;
unsigned long lastUARTSend = 0;
const unsigned long UART_SEND_INTERVAL = 2000;
void setup() {
  Serial.begin(115200);
  uart.init(); 
  display.init();
  display.showStartupScreen();
  delay(STARTUP_DELAY); 
  relay.init();
  sensor.init();
  scheduler.init(); 
  Serial.println("=== SMART GARDEN SYSTEM ===");
  Serial.println("System Ready!");
  Serial.println("USART1 (PA9=TX, PA10=RX) connected to ESP32");
  Serial.println("✅ Schedule editing via Blynk enabled");
  Serial.println();
}
void loop() {
  unsigned long now = millis(); 
  // ===== NHẬN LỆNH TỪ ESP32 =====
  String cmd = uart.receiveCommand();
  if (cmd.length() > 0) {
    char cmdType;
    String value; 
    if (uart.parseCommand(cmd, &cmdType, &value)) { 
      // Lệnh M: Đổi mode
      if (cmdType == CMD_SET_MODE) {
        if (value == "0") {
          scheduler.setMode(MODE_AUTO);
          Serial.println("📱 Blynk: Switched to AUTO mode");
        } else if (value == "1") {
          scheduler.setMode(MODE_SCHEDULE);
          Serial.println("📱 Blynk: Switched to SCHEDULE mode");
        }
        Serial.printf("✅ Mode confirmed: %d\n", scheduler.getMode());
      } 
      // Lệnh T: Set thời gian
      else if (cmdType == CMD_SET_TIME && value.length() == 4) {
        uint8_t h = value.substring(0, 2).toInt();
        uint8_t m = value.substring(2, 4).toInt();
        if (h < 24 && m < 60) {
          scheduler.setTime(h, m);
          Serial.printf("📱 Blynk: Time set to %02d:%02d\n", h, m);
        }
      } 
      // Lệnh P: Điều khiển bơm thủ công
      else if (cmdType == CMD_PUMP_MANUAL) {
        if (value == "1") {
          relay.control(true);
          Serial.println("📱 Blynk: Manual pump ON");
        } else if (value == "0") {
          relay.control(false);
          Serial.println("📱 Blynk: Manual pump OFF");
        }
      } 
      // Lệnh L: Set lịch tưới
      else if (cmdType == CMD_SET_SCHEDULE) {
        uint8_t index, hour, minute;
        uint16_t duration;
        bool enabled; 
        if (uart.parseScheduleCommand(value, &index, &hour, &minute, &duration, &enabled)) {
          if (scheduler.setSchedule(index, hour, minute, duration, enabled)) {
            Serial.printf("📅 Schedule %d updated: %02d:%02d, %ds, %s\n", 
                         index, hour, minute, duration, enabled ? "ON" : "OFF");
          } else {
            Serial.printf("⚠️ Failed to set schedule %d\n", index);
          }
        }
      } 
      // Lệnh S: Yêu cầu status
      else if (cmdType == CMD_GET_STATUS) {
        Serial.println("📱 Blynk: Status requested");
      }
    }
  } 
  // ===== CẬP NHẬT THỜI GIAN VÀ LỊCH TƯỚI =====
  scheduler.updateTime();
  scheduler.shouldWaterBySchedule();
  scheduler.updateWatering(); 
  // ===== ĐỌC CẢM BIẾN VÀ ĐIỀU KHIỂN =====
  if (now - lastRead >= READ_INTERVAL) {
    bool dhtSuccess = sensor.readDHT22();
    sensor.readSoilMoisture(); 
    bool shouldPumpOn = false; 
    if (scheduler.getMode() == MODE_AUTO) {
      shouldPumpOn = sensor.isSoilDry();
    } 
    else if (scheduler.getMode() == MODE_SCHEDULE) {
      shouldPumpOn = scheduler.isScheduleWatering();
    } 
    relay.control(shouldPumpOn); 
    display.updateDisplay(
      sensor.getTemperature(),
      sensor.getHumidity(),
      sensor.isSoilDry(),
      sensor.getSoilMoisturePercent(),
      relay.isOn(),
      dhtSuccess
    ); 
    Serial.println("=========================");
    Serial.print("Mode: "); 
    Serial.println(scheduler.getMode() == MODE_AUTO ? "AUTO" : "SCHEDULE");
    Serial.print("Time: "); 
    Serial.printf("%02d:%02d\n", scheduler.getHour(), scheduler.getMinute());
    Serial.print("Temp: "); 
    Serial.print(sensor.getTemperature()); 
    Serial.println(" C");
    Serial.print("Humidity: "); 
    Serial.print(sensor.getHumidity()); 
    Serial.println(" %");
    Serial.print("Soil: "); 
    Serial.print(sensor.getSoilMoisturePercent()); 
    Serial.println(" %");
    Serial.print("Pump: "); 
    Serial.println(relay.isOn() ? "ON" : "OFF");
    Serial.print(sensor.getSoilMoistureRaw());
    if (scheduler.getMode() == MODE_SCHEDULE) {
      Serial.print("Is Watering: ");
      Serial.println(scheduler.isScheduleWatering() ? "YES" : "NO");
      Serial.print("Next Schedule: "); 
      Serial.println(scheduler.getNextSchedule());
    } 
    if (!dhtSuccess) {
      Serial.println("⚠️ DHT22 ERROR!");
    }
    Serial.println(); 
    lastRead = now;
  } 
  // ===== GỬI DATA LÊN ESP32 =====
  if (now - lastUARTSend >= UART_SEND_INTERVAL) {
    uart.sendData(
      sensor.getTemperature(),
      sensor.getHumidity(),
      sensor.getSoilMoisturePercent(),
      relay.isOn(),
      scheduler.getMode()
    );
    lastUARTSend = now;
  }
  delay(500);
}
