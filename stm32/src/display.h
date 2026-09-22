#ifndef DISPLAY_H
#define DISPLAY_H
#include <TFT_eSPI.h>
#include "config.h"
class DisplayModule {
private:
  TFT_eSPI tft; 
  void drawRoundedBox(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    tft.fillRoundRect(x, y, w, h, 8, color);
  } 
  void drawTempIcon(int16_t x, int16_t y, uint16_t color) {
    tft.fillCircle(x+4, y+2, 3, color);
    tft.fillRect(x+2, y+5, 5, 8, color);
    tft.fillCircle(x+4, y+13, 5, color);
  } 
  void drawHumidityIcon(int16_t x, int16_t y, uint16_t color) {
    tft.fillCircle(x+5, y+12, 5, color);
    tft.fillTriangle(x+5, y, x, y+10, x+10, y+10, color);
  } 
  void drawSoilIcon(int16_t x, int16_t y, uint16_t color) {
    tft.fillRoundRect(x, y+8, 12, 8, 2, color);
    tft.fillCircle(x+2, y+5, 2, TFT_DARKGREEN);
    tft.fillCircle(x+6, y+3, 2, TFT_DARKGREEN);
    tft.fillCircle(x+10, y+5, 2, TFT_DARKGREEN);
  } 
public:
  DisplayModule() {} 
  void init() {
    tft.init();
    tft.setRotation(0);
  } 
  void showStartupScreen() {
    tft.fillScreen(TFT_NAVY);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.setCursor(15, 50);
    tft.println("SMART");
    tft.setCursor(10, 80);
    tft.println("GARDEN"); 
    tft.setTextSize(1);
    tft.setCursor(25, 120);
    tft.println("Initializing...");
  } 
  // ✅ SỬA: Bỏ 3 tham số không dùng (mode, hour, minute)
  void updateDisplay(float temp, float hum, bool soilDry, uint8_t soilPercent, 
                     bool pumpOn, bool dhtSuccess) {
    tft.fillScreen(TFT_BLACK); 
    // NHIỆT ĐỘ
    drawRoundedBox(5, 5, 118, 27, COLOR_TEMP_BG);
    drawTempIcon(10, 9, COLOR_TEMP_ICON);
    tft.setTextColor(COLOR_WHITE);
    tft.setTextSize(1);
    tft.setCursor(28, 8);
    tft.print("TEMP"); 
    if (dhtSuccess) {
      tft.setTextColor(COLOR_TEMP_TEXT);
      tft.setTextSize(2);
      tft.setCursor(28, 17);
      tft.print(temp, 1);
      tft.setTextSize(1);
      tft.print("C");
    } else {
      tft.setTextColor(COLOR_ERROR);
      tft.setTextSize(1);
      tft.setCursor(28, 20);
      tft.print("ERROR");
    }
    // ĐỘ ẨM KHÔNG KHÍ
    drawRoundedBox(5, 37, 118, 27, COLOR_HUM_BG);
    drawHumidityIcon(10, 41, COLOR_HUM_ICON);
    tft.setTextColor(COLOR_WHITE);
    tft.setTextSize(1);
    tft.setCursor(28, 40);
    tft.print("HUMIDITY"); 
    if (dhtSuccess) {
      tft.setTextColor(COLOR_HUM_TEXT);
      tft.setTextSize(2);
      tft.setCursor(28, 49);
      tft.print(hum, 1);
      tft.setTextSize(1);
      tft.print("%");
    } else {
      tft.setTextColor(COLOR_ERROR);
      tft.setTextSize(1);
      tft.setCursor(28, 52);
      tft.print("ERROR");
    }
    // ĐỘ ẨM ĐẤT
    uint16_t soilBgColor = soilDry ? COLOR_SOIL_DRY : COLOR_SOIL_WET;
    drawRoundedBox(5, 69, 118, 27, soilBgColor);
    drawSoilIcon(10, 73, COLOR_SOIL_ICON);
    tft.setTextColor(COLOR_WHITE);
    tft.setTextSize(1);
    tft.setCursor(28, 72);
    tft.print("SOIL"); 
    if (soilDry) {
      tft.setTextColor(TFT_ORANGE);
    } else {
      tft.setTextColor(TFT_GREENYELLOW);
    }
    tft.setTextSize(2);
    tft.setCursor(28, 81);
    tft.print(soilPercent);
    tft.setTextSize(1);
    tft.print("%");
    // TRẠNG THÁI RELAY
    if (pumpOn) {
      drawRoundedBox(5, 101, 118, 27, COLOR_PUMP_ON);
      tft.setTextColor(COLOR_WHITE);
      tft.setTextSize(2);
      tft.setCursor(18, 108);
      tft.print("PUMP:ON");
    } else {
      uint16_t gray = tft.color565(128, 128, 128);
      drawRoundedBox(5, 101, 118, 27, gray);
      tft.setTextColor(COLOR_WHITE);
      tft.setTextSize(2);
      tft.setCursor(16, 108);
      tft.print("PUMP:OFF");
    }
  }
};
#endif
