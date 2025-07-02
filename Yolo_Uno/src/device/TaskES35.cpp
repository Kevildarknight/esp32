#include "TaskES35.h"

#define BAUD_RATE_2 9600
#define RXD_RS485 17    // Chân RX cho RS485
#define TXD_RS485 10    // Chân TX cho RS485

HardwareSerial RS485Serial(1);

void sendRS485Command(byte *command, int commandSize, byte *response, int responseSize)
{
    RS485Serial.write(command, commandSize);
    RS485Serial.flush();
    delay(100);
    if (RS485Serial.available() >= responseSize)
    {
        RS485Serial.readBytes(response, responseSize);
    }
}

void getValueES35()
{
    float temperature = 0, humidity = 0;
    float sound = 0, pressure = 0, pm2p5 = 0, light = 0, pm10 = 0;
    
    // Các lệnh request cho ES35 sensor
    byte soundRequest[] = {0x15, 0x03, 0x01, 0xF6, 0x00, 0x01, 0x66, 0xD0};
    byte PressureRequest[] = {0x15, 0x03, 0x01, 0xF9, 0x00, 0x01, 0x56, 0xD3};
    byte pm2p5Request[] = {0x15, 0x03, 0x01, 0xF7, 0x00, 0x01, 0x37, 0x10};
    byte PM10Request[] = {0x15, 0x03, 0x01, 0xF8, 0x00, 0x01, 0x07, 0x13};
    byte AmbientLightRequest[] = {0x15, 0x03, 0x01, 0xFB, 0x00, 0x01, 0xF7, 0x13};
    byte temperatureRequest[] = {0x15, 0x03, 0x01, 0xF5, 0x00, 0x01, 0x96, 0xD0};
    byte humidityRequest[] = {0x15, 0x03, 0x01, 0xF4, 0x00, 0x01, 0xC7, 0x10};
    byte response[7];

    // Đọc áp suất
    memset(response, 0, sizeof(response));
    sendRS485Command(PressureRequest, sizeof(PressureRequest), response, sizeof(response));
    if (response[1] == 0x03)
    {
        pressure = (response[3] << 8) | response[4];
        pressure /= 10.0;
    }

    // Đọc âm thanh
    memset(response, 0, sizeof(response));
    sendRS485Command(soundRequest, sizeof(soundRequest), response, sizeof(response));
    if (response[1] == 0x03)
    {
        sound = (response[3] << 8) | response[4];
        sound /= 10.0;
    }

    // Đọc PM2.5
    memset(response, 0, sizeof(response));
    sendRS485Command(pm2p5Request, sizeof(pm2p5Request), response, sizeof(response));
    if (response[1] == 0x03)
    {
        pm2p5 = (response[3] << 8) | response[4];
        pm2p5 /= 10.0;
    }

    // Đọc ánh sáng
    memset(response, 0, sizeof(response));
    sendRS485Command(AmbientLightRequest, sizeof(AmbientLightRequest), response, sizeof(response));
    if (response[1] == 0x03)
    {
        light = (response[3] << 8) | response[4];
        light /= 10.0;
    }

    // Đọc PM10
    memset(response, 0, sizeof(response));
    sendRS485Command(PM10Request, sizeof(PM10Request), response, sizeof(response));
    if (response[1] == 0x03)
    {
        pm10 = (response[3] << 8) | response[4];
        pm10 /= 10.0;
    }

    // Đọc nhiệt độ
    memset(response, 0, sizeof(response));
    sendRS485Command(temperatureRequest, sizeof(temperatureRequest), response, sizeof(response));
    if (response[1] == 0x03)
    {
        temperature = (response[3] << 8) | response[4];
        temperature /= 10.0;
    }

    delay(2000);

    // Đọc độ ẩm
    memset(response, 0, sizeof(response));
    sendRS485Command(humidityRequest, sizeof(humidityRequest), response, sizeof(response));
    if (response[1] == 0x03)
    {
        humidity = (response[3] << 8) | response[4];
        humidity /= 10.0;
    }

    // In kết quả ra Serial
    Serial.println("ES35 Sensor Data:");
    Serial.println("Temperature: " + String(temperature, 1) + "°C, Humidity: " + String(humidity, 1) + "%");
    Serial.println("Pressure: " + String(pressure, 1) + " hPa, Light: " + String(light, 1) + " lux");
    Serial.println("PM2.5: " + String(pm2p5, 1) + " µg/m³, PM10: " + String(pm10, 1) + " µg/m³");
    Serial.println("Sound Level: " + String(sound, 1) + " dB");
    Serial.println("-----------------------------------");
    
    // Có thể publish data nếu cần
    // publishData("ES35_Temperature", String(temperature));
    // publishData("ES35_Humidity", String(humidity));
    // publishData("ES35_Pressure", String(pressure));
    // publishData("ES35_Light", String(light));
    // publishData("ES35_PM2p5", String(pm2p5));
    // publishData("ES35_PM10", String(pm10));
    // publishData("ES35_Sound", String(sound));
}

void TaskES35Sensor(void *pvParameters)
{
    while (true)
    {
        getValueES35();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void initES35()
{
    // Khởi tạo Serial cho RS485
    RS485Serial.begin(BAUD_RATE_2, SERIAL_8N1, RXD_RS485, TXD_RS485);
    
    // Đợi một chút cho Serial ổn định
    delay(1000);
    
    // Tạo task để đọc dữ liệu định kỳ
    xTaskCreate(TaskES35Sensor, "TaskES35Sensor", 4096, NULL, 1, NULL);
    
    Serial.println("ES35 RS485 Sensor initialized");
    Serial.printf("ES35 Serial: RX=%d, TX=%d, Baud=%d\n", RXD_RS485, TXD_RS485, BAUD_RATE_2);
}