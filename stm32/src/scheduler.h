#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <Arduino.h>
#include "config.h"
class SchedulerModule {
private:
  WateringSchedule schedules[MAX_SCHEDULES];
  WateringMode currentMode;
  bool isWatering;
  unsigned long wateringStartTime;
  uint16_t currentWateringDuration;
  int8_t lastExecutedSchedule;
  uint8_t currentHour;
  uint8_t currentMinute;
  unsigned long lastMinuteUpdate; 
public:
  SchedulerModule() : currentMode(MODE_AUTO), isWatering(false), 
                      wateringStartTime(0), currentWateringDuration(0),
                      lastExecutedSchedule(-1), currentHour(6), 
                      currentMinute(0), lastMinuteUpdate(0) {
  } 
  void init() {
    lastMinuteUpdate = millis();
  } 
  void updateTime() {
    if (millis() - lastMinuteUpdate >= 60000) {
      currentMinute++;
      if (currentMinute >= 60) {
        currentMinute = 0;
        currentHour++;
        if (currentHour >= 24) {
          currentHour = 0;
          lastExecutedSchedule = -1;
        }
      }
      lastMinuteUpdate = millis();
    }
  } 
  //Nhận thời gian từ ESP32
  void setTime(uint8_t hour, uint8_t minute) {
    currentHour = hour;
    currentMinute = minute;
    lastMinuteUpdate = millis();
    lastExecutedSchedule = -1;
  } 
  bool shouldWaterBySchedule() {
    if (currentMode != MODE_SCHEDULE) return false;
    if (isWatering) return true; 
    for (int i = 0; i < MAX_SCHEDULES; i++) {
      if (!schedules[i].enabled) continue; 
      if (schedules[i].hour == currentHour && 
          schedules[i].minute == currentMinute &&
          lastExecutedSchedule != i) { 
        isWatering = true;
        wateringStartTime = millis();
        currentWateringDuration = schedules[i].duration * 1000;
        lastExecutedSchedule = i; 
        Serial.print("Starting scheduled watering: ");
        Serial.print(schedules[i].hour);
        Serial.print(":");
        Serial.print(schedules[i].minute);
        Serial.print(" for ");
        Serial.print(schedules[i].duration);
        Serial.println("s"); 
        return true;
      }
    } 
    return false;
  } 
  void updateWatering() {
    if (isWatering) {
      if (millis() - wateringStartTime >= currentWateringDuration) {
        isWatering = false;
        Serial.println("Scheduled watering completed");
      }
    }
  } 
  WateringMode getMode() { return currentMode; }
  void setMode(WateringMode mode) { currentMode = mode; }
  bool isScheduleWatering() { return isWatering; }
  uint8_t getHour() { return currentHour; }
  uint8_t getMinute() { return currentMinute; } 
  String getNextSchedule() {
    for (int i = 0; i < MAX_SCHEDULES; i++) {
      if (!schedules[i].enabled) continue; 
      if (schedules[i].hour > currentHour || 
          (schedules[i].hour == currentHour && schedules[i].minute > currentMinute)) {
        char buffer[20];
        sprintf(buffer, "%02d:%02d (%ds)", 
                schedules[i].hour, schedules[i].minute, schedules[i].duration);
        return String(buffer);
      }
    } 
    for (int i = 0; i < MAX_SCHEDULES; i++) {
      if (schedules[i].enabled) {
        char buffer[20];
        sprintf(buffer, "%02d:%02d (%ds)", 
                schedules[i].hour, schedules[i].minute, schedules[i].duration);
        return String(buffer);
      }
    } 
    return "None";
  } 
  bool setSchedule(uint8_t index, uint8_t hour, uint8_t minute, uint16_t duration, bool enabled) {
    if (index >= MAX_SCHEDULES) return false;
    schedules[index] = {hour, minute, duration, enabled};
    return true;
  }
};
#endif
