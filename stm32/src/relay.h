#ifndef RELAY_H
#define RELAY_H
#include <Arduino.h>
#include "config.h"
class RelayModule {
private:
  bool state; 
public:
  RelayModule() : state(false) {} 
  void init() {
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH); 
  } 
  void control(bool turnOn) {
    digitalWrite(RELAY_PIN, turnOn ? HIGH : LOW);
    state = turnOn;
  } 
  bool isOn() { return state; }
};
#endif
