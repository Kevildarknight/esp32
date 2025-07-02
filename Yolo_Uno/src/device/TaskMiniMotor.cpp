#include "TaskMiniMotor.h"

#define MY_SCL 11
#define MY_SDA 12

#define MOTOR_1 10
#define MOTOR_2 17

#define MAX_SPEED 255
#define MIN_SPEED 0

#define delay_time 5000

void stopMotor() {
    digitalWrite(MOTOR_1, LOW);
    digitalWrite(MOTOR_2, LOW);
}

void forwardMotor(int speed) {
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);
    digitalWrite(MOTOR_1, HIGH);
    analogWrite(MOTOR_2, 255 - speed);
}

void taskMiniMotor(void *pvParameters)
{
    pinMode(MOTOR_1, OUTPUT);
    pinMode(MOTOR_2, OUTPUT);
    
    while (true)
    {
        forwardMotor(MAX_SPEED);
        delay(1000);
        stopMotor();    
        vTaskDelay(delay_time / portTICK_PERIOD_MS);
    }
}

void initMiniMotor()
{
    Wire.begin(MY_SCL, MY_SDA);
    
    xTaskCreate(taskMiniMotor, "Task_name", 4096, NULL, 1, NULL);
}
