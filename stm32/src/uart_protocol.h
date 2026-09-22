#ifndef UART_PROTOCOL_H
#define UART_PROTOCOL_H
#include <Arduino.h>
// ===== ĐỊNH NGHĨA LỆNH =====
// STM32 → ESP32 (Gửi dữ liệu cảm biến)
#define CMD_DATA_SEND    'D'  // D,25.5,60.2,45,1,0\n
// ESP32 → STM32 (Nhận lệnh điều khiển)
#define CMD_SET_MODE     'M'  // M0 = AUTO, M1 = SCHEDULE
#define CMD_SET_TIME     'T'  // T0630 = Set 6:30
#define CMD_PUMP_MANUAL  'P'  // P0 = OFF, P1 = ON
#define CMD_GET_STATUS   'S'  // S = Request status
#define CMD_SET_SCHEDULE 'L'  // ✅ THÊM: L0,6,30,30,1 = Schedule 0, 6:30, 30s, enabled
// ===== CẤU HÌNH UART =====
#define UART_BAUD 9600
#define UART_TIMEOUT 100
class UARTProtocol {
private:
  HardwareSerial* serial;
  String rxBuffer; 
public:
  UARTProtocol(HardwareSerial* s) : serial(s), rxBuffer("") {} 
  void init() {
    serial->begin(UART_BAUD);
    rxBuffer.reserve(64);
  } 
  // ===== STM32: GỬI DỮ LIỆU LÊN ESP32 =====
  void sendData(float temp, float hum, uint8_t soilPercent, 
                bool pumpOn, uint8_t mode) {
    serial->print(CMD_DATA_SEND);
    serial->print(',');
    serial->print(temp, 1);
    serial->print(',');
    serial->print(hum, 1);
    serial->print(',');
    serial->print(soilPercent);
    serial->print(',');
    serial->print(pumpOn ? 1 : 0);
    serial->print(',');
    serial->print(mode);
    serial->println();
    serial->flush();
  } 
  // ===== STM32: NHẬN LỆNH TỪ ESP32 =====
  String receiveCommand() {
    while (serial->available()) {
      char c = serial->read(); 
      if (c == '\n') {
        String cmd = rxBuffer;
        rxBuffer = "";
        return cmd;
      } else if (c != '\r') {
        rxBuffer += c;
        if (rxBuffer.length() > 64) {
          rxBuffer = "";
        }
      }
    }
    return "";
  } 
  // ===== PARSE LỆNH =====
  bool parseCommand(String cmd, char* cmdType, String* value) {
    if (cmd.length() < 1) return false; 
    *cmdType = cmd.charAt(0); 
    if (cmd.length() > 1) {
      *value = cmd.substring(1);
    } else {
      *value = "";
    } 
    return true;
  } 
  // ===== ✅ THÊM: PARSE LỆNH SET SCHEDULE =====
  // Format: L0,6,30,30,1 → index=0, hour=6, minute=30, duration=30, enabled=1
  bool parseScheduleCommand(String value, uint8_t* index, uint8_t* hour, 
                           uint8_t* minute, uint16_t* duration, bool* enabled) {
    int idx1 = value.indexOf(',');
    int idx2 = value.indexOf(',', idx1 + 1);
    int idx3 = value.indexOf(',', idx2 + 1);
    int idx4 = value.indexOf(',', idx3 + 1); 
    if (idx1 < 0 || idx2 < 0 || idx3 < 0 || idx4 < 0) return false; 
    *index = value.substring(0, idx1).toInt();
    *hour = value.substring(idx1 + 1, idx2).toInt();
    *minute = value.substring(idx2 + 1, idx3).toInt();
    *duration = value.substring(idx3 + 1, idx4).toInt();
    *enabled = value.substring(idx4 + 1).toInt() == 1; 
    return true;
  }
};
#endif
