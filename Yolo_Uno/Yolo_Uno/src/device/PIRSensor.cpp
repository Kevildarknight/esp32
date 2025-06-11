#include "PIRSensor.h"
#define delay_time 3000
int ledPin = 8;        // chọn chân 13 báo hiệu LED
int inputPin = 6;       // chọn ngõ tín hiệu vào cho PIR
int pirState = LOW;     // Bắt đầu với không có báo động
int val = 0;
void PIR(void *pvParameters)
{
    while (true)
    {
        getValuePIR();
        vTaskDelay(delay_time / portTICK_PERIOD_MS);
    }
}
void getValuePIR()
{
    val = digitalRead(inputPin);    // đọc giá trị đầu vào.
	if (val == HIGH)                // nếu giá trị ở mức cao.(1)
	{
		digitalWrite(ledPin, HIGH); // LED On
		delay(150);
 
		if (pirState == LOW)
		{
			Serial.println("Motion detected!");
            publishData("feed_PIR","Motion detected");
			pirState = HIGH;
		}
	}
	else
	{
		digitalWrite(ledPin, LOW);
		delay(300);
		if (pirState == HIGH)
		{
			Serial.println("Motion ended!");
            publishData("feed_PIR","Motion ended!");
			pirState = LOW;
		}
	}
  delay(1000);
}
void initPIR()
{
    pinMode(ledPin, OUTPUT);
	pinMode(inputPin, INPUT);
    xTaskCreate(PIR, "Task_name", 2048, NULL, 1, NULL);
}
