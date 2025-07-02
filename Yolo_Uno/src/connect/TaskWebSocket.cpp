#include "TaskWebSocket.h"
extern DHT20 dht20;

// LED control variables
bool ledState = 0;
const int ledPin = 6;  // Change this to your LED pin

// Create AsyncWebServer object on port 80
AsyncWebServer serversocket(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Json Variable to Hold Sensor Readings
JSONVar readings;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

// Get Sensor Readings and return JSON object
String getSensorReadings(){
  readings["temperature"] = String(temperature);
  readings["humidity"] = String(humidity);
  readings["ledState"] = String(ledState);
  String jsonString = JSON.stringify(readings);
  return jsonString;
}

// Initialize LittleFS
void initLittleFS() {
  if (!LittleFS.begin(true)) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");
}

void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
}

void notifyClientsLedState() {
  ws.textAll(String(ledState));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String message = (char*)data;
    
    // Check if the message is "getReadings"
    if (message == "getReadings") {
      // Send current sensor readings
      String sensorReadings = getSensorReadings();
      Serial.println("Sending readings: " + sensorReadings);
      notifyClients(sensorReadings);
    }
    // Check if the message is "toggle"
    else if (message == "toggle") {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      Serial.println("LED toggled to: " + String(ledState ? "ON" : "OFF"));
      
      // Send updated readings with new LED state
      String sensorReadings = getSensorReadings();
      notifyClients(sensorReadings);
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      {
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        // Send current state to newly connected client
        String sensorReadings = getSensorReadings();
        client->text(sensorReadings);
        break;
      }
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  serversocket.addHandler(&ws);
}

void setupWebSocket() {
  // Initialize LED pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  
  initLittleFS();
  initWebSocket();

  // Web Server Root URL
  serversocket.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });

  serversocket.serveStatic("/", LittleFS, "/");

  // Start server
  serversocket.begin();
  ElegantOTA.begin(&serversocket);
  
  Serial.println("Web Server started");
  Serial.println("LED Pin: " + String(ledPin));
}

void loopWebSocket() {
  // Update LED physical state
  digitalWrite(ledPin, ledState);
  
  // Send periodic sensor updates
  if ((millis() - lastTime) > timerDelay) {
    String sensorReadings = getSensorReadings();
    Serial.println("Periodic update: " + sensorReadings);
    notifyClients(sensorReadings);
    lastTime = millis();
  }
  ws.cleanupClients();
}