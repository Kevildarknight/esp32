#include "TaskLedRGB.h"

#define MY_SCL 11
#define MY_SDA 12

#define NUM_PIXEL 4
#define PIN_PIXEL 6

Adafruit_NeoPixel neoPixel(NUM_PIXEL, PIN_PIXEL, NEO_GRB + NEO_KHZ800);

#define delay_time 10000

uint32_t color[] = {
    neoPixel.Color(255, 0, 0),      //Red
    neoPixel.Color(255, 127, 0),    //Orange
    neoPixel.Color(255, 255, 0),    //Yellow
    neoPixel.Color(0, 255, 0),      //Green
    neoPixel.Color(75, 0, 130),     //Violet
    neoPixel.Color(148, 0, 211),    //Purple
    neoPixel.Color(255, 255, 255),  //White
    neoPixel.Color(0, 0, 0)         //Black
};

int numColors = sizeof(color) / sizeof(color[0]);

void taskLEDRGB(void *pvParameters)
{
    while (true)
    {
        for (int i = 0; i < numColors; i++) {
            neoPixel.clear();
            for (int pixel = 0; pixel < NUM_PIXEL; pixel++) {
                neoPixel.setPixelColor(pixel, color[i]);
            }
            neoPixel.show();
            delay(2000);
        }
        vTaskDelay(delay_time / portTICK_PERIOD_MS);
    }
}

void initLedRGB()
{
    Wire.begin(MY_SCL, MY_SDA);
    neoPixel.begin();
    xTaskCreate(taskLEDRGB, "Task_name", 4096, NULL, 1, NULL);
}
