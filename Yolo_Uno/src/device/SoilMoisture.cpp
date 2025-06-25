#include "SoilMoisture.h"

#define SOIL_MOISTURE_PIN 1     // GPIO1 (A0 trên Yolo Uno)
#define DELAY_TIME 10000        // Đọc mỗi 10 giây
#define ADC_MAX_VALUE 4095      // ESP32 có ADC 12-bit
#define DRY_VALUE 4095         // Giá trị khi khô hoàn toàn
#define WET_VALUE 1000          // Giá trị khi ướt hoàn toàn

int soilMoistureValue = 0;
float soilMoisturePercent = 0;

void TaskSoilMoisture(void *pvParameters)
{
    while (true)
    {
        getValueSoilMoisture();
        vTaskDelay(DELAY_TIME / portTICK_PERIOD_MS);
    }
}

void getValueSoilMoisture()
{
    // Đọc giá trị ADC thô
    soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);
    
    // Chuyển đổi sang phần trăm độ ẩm
    // Lưu ý: Cảm biến độ ẩm đất thường cho giá trị cao khi khô, thấp khi ướt
    soilMoisturePercent = map(soilMoistureValue, DRY_VALUE, WET_VALUE, 0, 100);
    
    // Giới hạn trong khoảng 0-100%
    soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);
    
    // In ra Serial
    Serial.print("Soil Moisture - Raw: ");
    Serial.print(soilMoistureValue);
    Serial.print(" | Moisture: ");
    Serial.print(soilMoisturePercent);
    Serial.println("%");
    
    // Gửi dữ liệu qua MQTT
    String data = String(soilMoisturePercent, 1) + "%";
    publishData("feed_soil", data);
    
    // Phân loại mức độ ẩm
    if (soilMoisturePercent < 20) {
        Serial.println("Status: Very Dry - Need water!");
        publishData("feed_soil_status", "Very Dry");
    } else if (soilMoisturePercent < 40) {
        Serial.println("Status: Dry");
        publishData("feed_soil_status", "Dry");
    } else if (soilMoisturePercent < 60) {
        Serial.println("Status: Normal");
        publishData("feed_soil_status", "Normal");
    } else if (soilMoisturePercent < 80) {
        Serial.println("Status: Moist");
        publishData("feed_soil_status", "Moist");
    } else {
        Serial.println("Status: Very Wet");
        publishData("feed_soil_status", "Very Wet");
    }
}

void initSoilMoisture()
{
    // Cấu hình chân analog
    pinMode(SOIL_MOISTURE_PIN, INPUT);
    
    // Cấu hình độ phân giải ADC
    analogReadResolution(12);  // 12-bit (0-4095)
    
    // Đọc giá trị ban đầu
    getValueSoilMoisture();
    
    // Tạo task
    xTaskCreate(TaskSoilMoisture, "Task_SoilMoisture", 2048, NULL, 1, NULL);
    
    Serial.println("Soil Moisture Sensor initialized on pin A0 (GPIO1)");
}