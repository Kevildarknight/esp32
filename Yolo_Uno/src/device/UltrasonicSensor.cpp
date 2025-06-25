#include "UltrasonicSensor.h"

#define TRIG_PIN 18     // GPIO18 (D9)
#define ECHO_PIN 21     // GPIO21 (D10)
#define DELAY_TIME 1000 // Đọc mỗi 1 giây
#define TIMEOUT 30000   // Timeout 30ms (~5m)

float distance = 0;

void TaskUltrasonic(void *pvParameters)
{
    while (true)
    {
        getValueUltrasonic();
        vTaskDelay(DELAY_TIME / portTICK_PERIOD_MS);
    }
}

void getValueUltrasonic()
{
    // Tạo xung trigger
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    // Đo thời gian phản hồi
    long duration = pulseIn(ECHO_PIN, HIGH, TIMEOUT);
    
    // Tính khoảng cách (cm)
    // Tốc độ âm thanh = 343 m/s = 0.0343 cm/us
    // Chia 2 vì sóng đi và về
    distance = duration * 0.0343 / 2;
    
    // Kiểm tra giá trị hợp lệ
    if (distance == 0 || distance > 500) {
        distance = -1; // Giá trị lỗi
        Serial.println("Ultrasonic: Out of range or error");
    } else {
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" cm");
        
        // Gửi qua MQTT
        String data = String(distance, 1) + "cm";
        publishData("feed_distance", data);
    }
}

void initUltrasonic()
{
    // Cấu hình chân
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    // Test đọc giá trị
    getValueUltrasonic();
    
    // Tạo task
    xTaskCreate(TaskUltrasonic, "Task_Ultrasonic", 2048, NULL, 1, NULL);
    
    Serial.println("Ultrasonic Sensor initialized (Trig=D9, Echo=D10)");
}