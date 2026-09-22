// ===== PHẢI DEFINE TRƯỚC KHI INCLUDE =====
#define BLYNK_TEMPLATE_ID "TMPL6IMEj47tl"
#define BLYNK_TEMPLATE_NAME "Auto water"
#define BLYNK_AUTH_TOKEN "b6HVsJ6QuuhD2XhcC6v3vPXLbOqj1210"
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <time.h>
// ===== CẤU HÌNH WIFI =====
char ssid[] = "Cong Buu";
char pass[] = "0903817214";
// ===== CẤU HÌNH NTP =====
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;
// ===== CẤU HÌNH UART =====
#define RXD1 9
#define TXD1 10
// ===== BLYNK VIRTUAL PINS =====
#define VPIN_TEMP        V0
#define VPIN_HUMIDITY    V1
#define VPIN_SOIL        V2
#define VPIN_PUMP_STATUS V3
#define VPIN_MODE_SWITCH V4
#define VPIN_CURRENT_HOUR   V5  // ✅ Đổi từ PUMP_BUTTON
#define VPIN_CURRENT_MINUTE V6  // ✅ Đổi từ CURRENT_TIME
#define VPIN_NEXT_SCHEDULE V7
// ✅ THÊM: Virtual pins cho chỉnh sửa lịch tưới (Schedule 1)
#define VPIN_SCHEDULE1_HOUR     V10  // Input giờ (0-23)
#define VPIN_SCHEDULE1_MINUTE   V11  // Input phút (0-59)
#define VPIN_SCHEDULE1_DURATION V12  // Input thời gian tưới (giây)
#define VPIN_SCHEDULE1_ENABLE   V13  // Switch bật/tắt
#define VPIN_SCHEDULE1_SET      V14  // Button "Lưu lịch 1"
// Schedule 2
#define VPIN_SCHEDULE2_HOUR     V15
#define VPIN_SCHEDULE2_MINUTE   V16
#define VPIN_SCHEDULE2_DURATION V17
#define VPIN_SCHEDULE2_ENABLE   V18
#define VPIN_SCHEDULE2_SET      V19
// Schedule 3
#define VPIN_SCHEDULE3_HOUR     V20
#define VPIN_SCHEDULE3_MINUTE   V21
#define VPIN_SCHEDULE3_DURATION V22
#define VPIN_SCHEDULE3_ENABLE   V23
#define VPIN_SCHEDULE3_SET      V24
// ===== BIẾN TOÀN CỤC =====
float temperature = 0;
float humidity = 0;
uint8_t soilPercent = 0;
bool pumpOn = false;
uint8_t currentMode = 0;
String rxBuffer = "";
BlynkTimer timer;
unsigned long lastTimeSyncToSTM32 = 0;
const unsigned long TIME_SYNC_INTERVAL = 60000;
// ✅ BIẾN LƯU TRỮ GIÁ TRỊ TỪ BLYNK CHO SCHEDULE
struct Schedule {
  uint8_t hour;
  uint8_t minute;
  uint16_t duration;
  bool enabled;
};
Schedule schedule1 = {6, 0, 30, false};
Schedule schedule2 = {12, 0, 30, false};
Schedule schedule3 = {18, 0, 30, false};
// ===== KHAI BÁO HÀM =====
void parseData(String data);
void requestStatus();
void syncTimeToSTM32();
void displayCurrentTime();
void sendScheduleToSTM32(uint8_t index, uint8_t hour, uint8_t minute, uint16_t duration, bool enabled);
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD1, TXD1); 
  Serial.println("ESP32 Blynk Gateway with Schedule Editor");
  Serial.println("Connecting to WiFi..."); 
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Waiting for NTP time sync..."); 
  struct tm timeinfo;
  int retry = 0;
  while (!getLocalTime(&timeinfo) && retry < 10) {
    Serial.print(".");
    delay(1000);
    retry++;
  } 
  if (retry < 10) {
    Serial.println("\n✅ NTP Time synchronized!");
    Serial.println(&timeinfo, "Current time: %A, %B %d %Y %H:%M:%S");
    syncTimeToSTM32();
  } else {
    Serial.println("\n⚠️ Failed to sync NTP time");
  } 
  timer.setInterval(2000L, requestStatus);
  timer.setInterval(TIME_SYNC_INTERVAL, syncTimeToSTM32);
  timer.setInterval(10000L, displayCurrentTime); 
  Serial.println("Ready!");
}
void loop() {
  Blynk.run();
  timer.run(); 
  while (Serial2.available()) {
    char c = Serial2.read(); 
    if (c == '\n') {
      parseData(rxBuffer);
      rxBuffer = "";
    } else if (c != '\r') {
      rxBuffer += c;
    }
  }
}
void syncTimeToSTM32() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("⚠️ Failed to get time");
    return;
  } 
  char timeCmd[10];
  sprintf(timeCmd, "T%02d%02d", timeinfo.tm_hour, timeinfo.tm_min);
  Serial2.println(timeCmd);
  
  Serial.printf("⏰ Time synced to STM32: %02d:%02d\n", 
                timeinfo.tm_hour, timeinfo.tm_min);
}
void displayCurrentTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  } 
  // ✅ Gửi giờ và phút riêng biệt lên Blynk
  Blynk.virtualWrite(VPIN_CURRENT_HOUR, timeinfo.tm_hour);
  Blynk.virtualWrite(VPIN_CURRENT_MINUTE, timeinfo.tm_min);
}
void parseData(String data) {
  if (data.charAt(0) != 'D') return; 
  int idx1 = data.indexOf(',', 2);
  int idx2 = data.indexOf(',', idx1 + 1);
  int idx3 = data.indexOf(',', idx2 + 1);
  int idx4 = data.indexOf(',', idx3 + 1); 
  if (idx1 > 0 && idx2 > 0 && idx3 > 0 && idx4 > 0) {
    temperature = data.substring(2, idx1).toFloat();
    humidity = data.substring(idx1 + 1, idx2).toFloat();
    soilPercent = data.substring(idx2 + 1, idx3).toInt();
    pumpOn = data.substring(idx3 + 1, idx4).toInt();
    currentMode = data.substring(idx4 + 1).toInt(); 
    Blynk.virtualWrite(VPIN_TEMP, temperature);
    Blynk.virtualWrite(VPIN_HUMIDITY, humidity);
    Blynk.virtualWrite(VPIN_SOIL, soilPercent);
    Blynk.virtualWrite(VPIN_PUMP_STATUS, pumpOn ? 255 : 0);
    Blynk.virtualWrite(VPIN_MODE_SWITCH, currentMode); 
    Serial.printf("📊 Temp: %.1f°C | Hum: %.1f%% | Soil: %d%% | Pump: %s | Mode: %s\n",
                  temperature, humidity, soilPercent, 
                  pumpOn ? "ON" : "OFF",
                  currentMode == 0 ? "AUTO" : "SCHEDULE");
  }
}
void requestStatus() {
  Serial2.println("S");
}
// ✅ HÀM GỬI LỊCH TƯỚI XUỐNG STM32
void sendScheduleToSTM32(uint8_t index, uint8_t hour, uint8_t minute, uint16_t duration, bool enabled) {
  char cmd[30];
  sprintf(cmd, "L%d,%d,%d,%d,%d", index, hour, minute, duration, enabled ? 1 : 0);
  Serial2.println(cmd); 
  Serial.printf("📅 Schedule %d set: %02d:%02d, %ds, %s\n", 
                index, hour, minute, duration, enabled ? "ON" : "OFF");
}
BLYNK_WRITE(VPIN_MODE_SWITCH) {
  int mode = param.asInt();
  Serial2.print("M");
  Serial2.println(mode);
  Serial.printf("📱 Mode changed to: %s\n", mode == 0 ? "AUTO" : "SCHEDULE");
}
BLYNK_WRITE(V8) {
  if (param.asInt() == 1) {
    syncTimeToSTM32();
    Blynk.virtualWrite(V8, 0);
  }
}
// ===== ✅ SCHEDULE 1 - LƯU GIÁ TRỊ VÀO BIẾN =====
BLYNK_WRITE(VPIN_SCHEDULE1_HOUR) {
  schedule1.hour = param.asInt();
}
BLYNK_WRITE(VPIN_SCHEDULE1_MINUTE) {
  schedule1.minute = param.asInt();
}
BLYNK_WRITE(VPIN_SCHEDULE1_DURATION) {
  schedule1.duration = param.asInt();
}
BLYNK_WRITE(VPIN_SCHEDULE1_ENABLE) {
  schedule1.enabled = param.asInt();
}
BLYNK_WRITE(VPIN_SCHEDULE1_SET) {
  if (param.asInt() == 1) {
    sendScheduleToSTM32(0, schedule1.hour, schedule1.minute, schedule1.duration, schedule1.enabled);
    Blynk.virtualWrite(VPIN_SCHEDULE1_SET, 0);
  }
}
// ===== ✅ SCHEDULE 2 =====
BLYNK_WRITE(VPIN_SCHEDULE2_HOUR) {
  schedule2.hour = param.asInt();
}
BLYNK_WRITE(VPIN_SCHEDULE2_MINUTE) {
  schedule2.minute = param.asInt();
}
BLYNK_WRITE(VPIN_SCHEDULE2_DURATION) {
  schedule2.duration = param.asInt();
}
BLYNK_WRITE(VPIN_SCHEDULE2_ENABLE) {
  schedule2.enabled = param.asInt();
}
BLYNK_WRITE(VPIN_SCHEDULE2_SET) {
  if (param.asInt() == 1) {
    sendScheduleToSTM32(1, schedule2.hour, schedule2.minute, schedule2.duration, schedule2.enabled);
    Blynk.virtualWrite(VPIN_SCHEDULE2_SET, 0);
  }
}
// ===== ✅ SCHEDULE 3 =====
BLYNK_WRITE(VPIN_SCHEDULE3_HOUR) {
  schedule3.hour = param.asInt();
}
BLYNK_WRITE(VPIN_SCHEDULE3_MINUTE) {
  schedule3.minute = param.asInt();
}
BLYNK_WRITE(VPIN_SCHEDULE3_DURATION) {
  schedule3.duration = param.asInt();
}
BLYNK_WRITE(VPIN_SCHEDULE3_ENABLE) {
  schedule3.enabled = param.asInt();
}
BLYNK_WRITE(VPIN_SCHEDULE3_SET) {
  if (param.asInt() == 1) {
    sendScheduleToSTM32(2, schedule3.hour, schedule3.minute, schedule3.duration, schedule3.enabled);
    Blynk.virtualWrite(VPIN_SCHEDULE3_SET, 0);
  }
}
