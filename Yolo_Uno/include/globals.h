#ifndef GLOBALS_H
#define GLOBALS_H

// Include libraries
#include <Wire.h>
#include <WiFi.h>
#include <DHT20.h>
#include <PIR.h>
#include <PubSubClient.h>
#include <I2C_LCD.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <WebServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <ElegantOTA.h>
// Include connection modules
#include "../src/connect/TaskWifi.h"
#include "../src/connect/TaskMQTT.h"
// #include "../src/connect/Webserver.h"
#include "../src/connect/TaskWebSocket.h"

// Include device modules
#include "../src/device/TaskDHT20.h"
#include "../src/device/PIRSensor.h"
#include "../src/device/LCD.h"
#include "../src/device/UltrasonicSensor.h"
#include "../src/device/SoilMoisture.h"
#include "../src/device/LightSensor.h"
#include "../src/device/TaskLedRGB.h"
#include "../src/device/Relay.h"
#include "../src/device/TaskMiniMotor.h"
#include "../src/device/TaskES35.h"
#endif