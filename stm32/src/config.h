#ifndef CONFIG_H
#define CONFIG_H
// ===== Cấu hình chân kết nối =====
#define DHTPIN PA8
#define SOIL_MOISTURE_AO_PIN PB1
#define RELAY_PIN PB10
// ===== Cấu hình thời gian =====
#define READ_INTERVAL 2000
#define STARTUP_DELAY 2000
// ===== Cấu hình chế độ tưới =====
enum WateringMode {
MODE_AUTO,
MODE_SCHEDULE
};
// ===== Cấu hình lịch tưới =====
struct WateringSchedule {
uint8_t hour;
uint8_t minute;
uint16_t duration;
bool enabled;
};
#define MAX_SCHEDULES 3
// ===== Cấu hình màu sắc =====
#define COLOR_TEMP_BG TFT_MAROON
#define COLOR_TEMP_ICON TFT_ORANGE
#define COLOR_TEMP_TEXT TFT_YELLOW
#define COLOR_HUM_BG TFT_BLUE
#define COLOR_HUM_ICON TFT_CYAN
#define COLOR_HUM_TEXT TFT_SKYBLUE
#define COLOR_SOIL_DRY TFT_MAROON
#define COLOR_SOIL_WET TFT_DARKGREEN
#define COLOR_SOIL_ICON TFT_BROWN
#define COLOR_PUMP_ON TFT_DARKGREEN
#define COLOR_ERROR TFT_RED
#define COLOR_WHITE TFT_WHITE
#endif
