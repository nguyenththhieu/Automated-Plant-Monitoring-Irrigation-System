# Automated Plant Monitoring Irrigation System
STM32-ESP32-based automated plant monitoring and irrigation system with Wi-Fi remote control.
## 📌 Overview
An IoT-based automatic plant monitoring and irrigation system built on STM32 (main controller) combined with ESP32 (connectivity module). It monitors soil moisture, ambient temperature and humidity, and automatically triggers a water pump when soil moisture drops below a set threshold — reducing manual watering effort and water waste, with remote monitoring/control via the Blynk app.
## ✨ Features
* Real-time soil moisture, temperature, and humidity monitoring

* Automatic pump control based on soil moisture threshold (<50% → pump ON, ≥50% → pump OFF)

* Scheduler mode: pump runs on user-defined time slots, independent of moisture readings

* On-device LCD display of live sensor data and pump status

* Remote monitoring and control via the Blynk mobile app (manual pump toggle, threshold/schedule configuration)

* Time sync via NTP (through ESP32) for accurate scheduling
## 🏗️ System Architecture
<img width="658" height="479" alt="image" src="https://github.com/user-attachments/assets/bc276add-3a8f-4338-bfa7-b4e0cf2c0714" />

## 🔧 Hardware
* Main MCU: STM32F103C8T6 (ARM Cortex-M3, 72MHz, 64KB Flash, 20KB RAM)

* Connectivity MCU: ESP32 (WiFi 802.11 b/g/n + Bluetooth, up to 240MHz)

* Sensors: DHT22 (temp: -40~80°C ±0.5°C; humidity: 0-100%RH ±2%), Soil Moisture Sensor (LM393 comparator, analog + digital output)

* Display: 1.44" TFT LCD (ST7735, SPI, 128×128px, 65K colors)

* Actuator: 1-channel opto-isolated relay (5V, 10A max) driving a 3-5V DC mini submersible pump (1.1-1.6 L/min)

* Power: 18650 battery shield (3.3V logic / 5V peripherals)

* Pin mapping: DHT22 on PA8, soil sensor ADC on PB1, relay control on PB0, LCD on SPI (PA3/PA4/PA5/PA6/PA7), STM32↔ESP32 UART on PA9/PA10
## 💻 Software & Technologies
* Firmware written in C/C++ using STM32CubeIDE + HAL library, configured via CubeMX

* Sensor read → threshold comparison → relay control loop running on STM32

* ESP32 firmware handles WiFi connection, NTP time sync, and Blynk library integration

* Blynk platform (App + Cloud + Library) used as the IoT layer for the UI and remote command relay
## 📡 Communication
* STM32 ↔ ESP32: UART, bidirectional, continuous — STM32 sends sensor/pump-status strings; ESP32 sends back control commands from the app

* ESP32 ↔ Blynk Cloud: WiFi, publishes sensor data and receives user commands (e.g., manual pump toggle) which are forwarded to STM32

* Framed as a two-way, real-time link so the app UI and physical pump state stay in sync
## 🎛️ Control Logic
* AUTO mode: STM32 continuously compares soil moisture to a 50% threshold — below it, relay activates the pump; at/above it, pump turns off

* SCHEDULER mode: pump is driven purely by pre-set time schedules (synced via NTP); soil moisture is ignored entirely, even at 0% or 100%

* Mode and schedule/threshold settings are configurable remotely from the Blynk app
## 🌐 Blynk Dashboard
Live display of soil moisture, temperature, humidity, and pump status; manual ON/OFF button; and configuration of watering schedule/thresholds, all synced over WiFi via ESP32.
## 📊 Results
* AUTO mode verified: pump activates reliably when soil moisture <50%, stays off when ≥50%

* Scheduler mode verified: pump ignores moisture level entirely and only runs at scheduled times

* Sensor data and pump state confirmed consistent between the LCD and the Blynk app in real time

* System validated as a small-scale working prototype; not yet tested outdoors or at production scale
