
#include "globals.h"

void setup()
{
  Serial.begin(115200);
    InitWiFi();
    initMQTT();
  // Khởi tạo DHT20 (I2C)
    // initDHT20();
    
    // Khởi tạo cảm biến siêu âm
    // initUltrasonic();
    // InitWebsever();
    
    // // Khởi tạo PIR (tùy chọn)
    // Serial.println("5. Initializing PIR sensor...");
    // initPIR();
    // delay(500);
    
    // Khởi tạo LCD cuối cùng
  
  // initLCD();
  // initSoilMoisture();
  // initLightSensor();
  // initLedRGB();
  // initRelay();
  // initMiniMotor();
  setupWebSocket();
  initES35();
}

void loop()
{
  if (!Wifi_reconnect())
  {
    return;
  }
  reconnectMQTT();
  // WebSeverloop();
  loopWebSocket();
}
