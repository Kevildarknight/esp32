#include "LightSensor.h"

#define LIGHT_SENSOR_PIN 2      // GPIO2 (A1 trên Yolo Uno)
#define DELAY_TIME 5000         // Đọc mỗi 5 giây
#define ADC_MAX_VALUE 4095      // ESP32 có ADC 12-bit
#define VOLTAGE_REF 3.3         // Điện áp tham chiếu 3.3V

int lightValue = 0;
float lightPercentage = 0;

void TaskLightSensor(void *pvParameters)
{
    while (true)
    {
        getValueLightSensor();
        vTaskDelay(DELAY_TIME / portTICK_PERIOD_MS);
    }
}

void getValueLightSensor()
{
    // Đọc giá trị ADC thô
    lightValue = analogRead(LIGHT_SENSOR_PIN);
    
    // Chuyển đổi sang phần trăm (0-100%)
    lightPercentage = (lightValue * 100.0) / ADC_MAX_VALUE;
    
    // Chuyển đổi sang điện áp nếu cần
    float voltage = (lightValue * VOLTAGE_REF) / ADC_MAX_VALUE;
    
    // In ra Serial
    Serial.print("Light Sensor - Raw: ");
    Serial.print(lightValue);
    Serial.print(" | Percentage: ");
    Serial.print(lightPercentage);
    Serial.print("% | Voltage: ");
    Serial.print(voltage);
    Serial.println("V");
    
    // Gửi dữ liệu qua MQTT
    String data = String(lightPercentage, 1) + "%";
    publishData("feed_light", data);
    
    // Phân loại mức độ ánh sáng
    if (lightPercentage < 20) {
        Serial.println("Status: Very Dark");
    } else if (lightPercentage < 40) {
        Serial.println("Status: Dark");
    } else if (lightPercentage < 60) {
        Serial.println("Status: Normal");
    } else if (lightPercentage < 80) {
        Serial.println("Status: Bright");
    } else {
        Serial.println("Status: Very Bright");
    }
}

void initLightSensor()
{
    // Cấu hình chân analog
    pinMode(LIGHT_SENSOR_PIN, INPUT);
    
    // Cấu hình độ phân giải ADC (tùy chọn)
    analogReadResolution(12);  // 12-bit (0-4095)
    
    // Đọc giá trị ban đầu
    getValueLightSensor();
    
    // Tạo task
    xTaskCreate(TaskLightSensor, "Task_LightSensor", 2048, NULL, 1, NULL);
    
    Serial.println("Light Sensor initialized on pin A1 (GPIO2)");
}