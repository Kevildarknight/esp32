#include "Relay.h"
#define delay_time 3000
int relayPin=8;
void Relay(void *pvParameters)
{
    while (true)
    {
        getValueRelay();
        vTaskDelay(delay_time / portTICK_PERIOD_MS);
    }
}
void getValueRelay(){
    digitalWrite(relayPin, HIGH);
    delay(1000);
    digitalWrite(relayPin, LOW);
    delay(1000);
}
void initRelay(){
    pinMode(relayPin, OUTPUT);
    xTaskCreate(Relay, "Task_Relay", 2048, NULL, 1, NULL);

}