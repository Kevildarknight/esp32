/*  
  Enhanced ESP32 Web Server with RGB LED, Single LED, and Sensor Display
  Based on Random Nerd Tutorials
*/
#include "WebServer.h"

// Username and password for web page access
const char* http_username = "admin";
const char* http_password = "admin";

// Assign output variables to GPIO pins
const int output10 = 10;
const int output17 = 17;
const int output48 = 48;
const int ledD13 = 48;  // LED D13 (GPIO48)
const int PIN_PIXEL = 6; // RGB LED pin

String output10State = "off";
String output17State = "off";
String ledD13State = "off";
String rgbLedState = "off";
int currentRGBColor = 0; // 0=Red, 1=Green, 2=Blue, 3=White, 4=Off

// Create a web server object
WebServer server(80);

// External sensor variables
extern DHT20 dht20;
extern float distance;
extern int lightValue;
extern float lightPercentage;
extern int soilMoistureValue;
extern float soilMoisturePercent;
extern Adafruit_NeoPixel neoPixel;

// Function to authenticate user
bool isAuthenticated() {
  if (!server.authenticate(http_username, http_password)) {
    server.requestAuthentication();
    return false;
  }
  return true;
}

// Function to get sensor data as HTML string
String getSensorDataHTML() {
  String html = "<div style='background-color: #f0f0f0; padding: 20px; margin: 20px 0; border-radius: 10px;'>";
  html += "<h2 style='color: #333; margin-top: 0;'>Sensor Data</h2>";
  
  // DHT20 Temperature and Humidity
  if (dht20.read() == DHT20_OK) {
    html += "<p><strong>Temperature:</strong> " + String(dht20.getTemperature(), 1) + "°C</p>";
    html += "<p><strong>Humidity:</strong> " + String(dht20.getHumidity(), 1) + "%</p>";
  } else {
    html += "<p><strong>DHT20:</strong> <span style='color: red;'>Error reading sensor</span></p>";
  }
  
  // Light Sensor
  html += "<p><strong>Light Level:</strong> " + String(lightPercentage, 1) + "% (" + String(lightValue) + ")</p>";
  
  // Soil Moisture
  html += "<p><strong>Soil Moisture:</strong> " + String(soilMoisturePercent, 1) + "% (" + String(soilMoistureValue) + ")</p>";
  
  // Distance
  if (distance > 0) {
    html += "<p><strong>Distance:</strong> " + String(distance, 1) + " cm</p>";
  } else {
    html += "<p><strong>Distance:</strong> Out of range</p>";
  }
  
  html += "</div>";
  return html;
}

// Function to set RGB LED color
void setRGBColor(int colorIndex) {
  uint32_t colors[] = {
    neoPixel.Color(255, 0, 0),    // Red
    neoPixel.Color(0, 255, 0),    // Green
    neoPixel.Color(0, 0, 255),    // Blue
    neoPixel.Color(255, 255, 255), // White
    neoPixel.Color(0, 0, 0)       // Off
  };
  
  if (colorIndex >= 0 && colorIndex < 5) {
    neoPixel.clear();
    for (int i = 0; i < 4; i++) { // NUM_PIXEL = 4
      neoPixel.setPixelColor(i, colors[colorIndex]);
    }
    neoPixel.show();
    currentRGBColor = colorIndex;
    rgbLedState = (colorIndex == 4) ? "off" : "on";
  }
}

// Function to handle the root URL and show the current states
void handleRoot() {
  if (!isAuthenticated()) return;

  String html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<meta http-equiv=\"refresh\" content=\"5\">"; // Auto refresh every 5 seconds
  html += "<link rel=\"icon\" href=\"data:,\">";
  html += "<style>";
  html += "html { font-family: Arial, sans-serif; display: inline-block; margin: 0px auto; text-align: center; background-color: #f5f5f5;}";
  html += "body { max-width: 800px; margin: 0 auto; padding: 20px; }";
  html += "h1 { color: #333; margin-bottom: 30px; }";
  html += ".control-section { background-color: white; padding: 20px; margin: 20px 0; border-radius: 10px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }";
  html += ".button { background-color: #4CAF50; border: none; color: white; padding: 12px 24px; text-decoration: none; font-size: 16px; margin: 5px; cursor: pointer; border-radius: 5px; display: inline-block; }";
  html += ".button:hover { background-color: #45a049; }";
  html += ".button2 { background-color: #f44336; }";
  html += ".button2:hover { background-color: #da190b; }";
  html += ".button-rgb { background-color: #2196F3; }";
  html += ".button-rgb:hover { background-color: #1976D2; }";
  html += ".button-red { background-color: #f44336; }";
  html += ".button-green { background-color: #4CAF50; }";
  html += ".button-blue { background-color: #2196F3; }";
  html += ".button-white { background-color: #757575; }";
  html += ".sensor-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; text-align: left; }";
  html += "@media (max-width: 600px) { .sensor-grid { grid-template-columns: 1fr; } }";
  html += "</style></head>";
  html += "<body>";
  html += "<h1>🌱 ESP32 Smart Garden Controller</h1>";

  // LED Controls Section
  html += "<div class='control-section'>";
  html += "<h2>LED Controls</h2>";
  
  // Single LED D13 controls
  html += "<div style='margin: 15px 0;'>";
  html += "<p><strong>LED D13 (GPIO48):</strong> " + ledD13State + "</p>";
  if (ledD13State == "off") {
    html += "<a href=\"/led13/on\"><button class=\"button\">Turn ON</button></a>";
  } else {
    html += "<a href=\"/led13/off\"><button class=\"button button2\">Turn OFF</button></a>";
  }
  html += "</div>";

  // RGB LED controls
  html += "<div style='margin: 15px 0;'>";
  String colorNames[] = {"Red", "Green", "Blue", "White", "Off"};
  html += "<p><strong>RGB LED:</strong> " + colorNames[currentRGBColor] + "</p>";
  html += "<a href=\"/rgb/red\"><button class=\"button button-red\">Red</button></a>";
  html += "<a href=\"/rgb/green\"><button class=\"button button-green\">Green</button></a>";
  html += "<a href=\"/rgb/blue\"><button class=\"button button-blue\">Blue</button></a>";
  html += "<a href=\"/rgb/white\"><button class=\"button button-white\">White</button></a>";
  html += "<a href=\"/rgb/off\"><button class=\"button button2\">OFF</button></a>";
  html += "</div>";
  html += "</div>";

  // Sensor Data Section
  html += getSensorDataHTML();

  html += "<div style='margin-top: 30px; padding: 10px; background-color: #e0e0e0; border-radius: 5px;'>";
  html += "<small>Auto-refresh every 5 seconds | Last updated: " + String(millis()/1000) + "s</small>";
  html += "</div>";

  html += "</body></html>";
  server.send(200, "text/html", html);
}

// LED D13 control functions
void handleLED13On() {
  if (!isAuthenticated()) return;
  ledD13State = "on";
  digitalWrite(ledD13, HIGH);
  handleRoot();
}

void handleLED13Off() {
  if (!isAuthenticated()) return;
  ledD13State = "off";
  digitalWrite(ledD13, LOW);
  handleRoot();
}

// RGB LED control functions
void handleRGBRed() {
  if (!isAuthenticated()) return;
  setRGBColor(0);
  handleRoot();
}

void handleRGBGreen() {
  if (!isAuthenticated()) return;
  setRGBColor(1);
  handleRoot();
}

void handleRGBBlue() {
  if (!isAuthenticated()) return;
  setRGBColor(2);
  handleRoot();
}

void handleRGBWhite() {
  if (!isAuthenticated()) return;
  setRGBColor(3);
  handleRoot();
}

void handleRGBOff() {
  if (!isAuthenticated()) return;
  setRGBColor(4);
  handleRoot();
}

bool check = false;

void InitWebsever() {
  // Initialize the output variables as outputs
  if(check)
    return;
    
  pinMode(output10, OUTPUT);
  pinMode(output17, OUTPUT);
  pinMode(ledD13, OUTPUT);
  
  // Set outputs to LOW initially
  digitalWrite(output10, LOW);
  digitalWrite(output17, LOW);
  digitalWrite(ledD13, LOW);

  // Initialize RGB LED
  setRGBColor(4); // Start with LED off

  // Connect to Wi-Fi network
  Serial.print("Connecting to ");
  Serial.println("ACLAB");
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  publishData("webseverIP", WiFi.localIP().toString());
  
  // Set up the web server to handle different routes with authentication
  server.on("/", handleRoot);

  
  // LED controls
  server.on("/led13/on", handleLED13On);
  server.on("/led13/off", handleLED13Off);
  
  // RGB LED controls
  server.on("/rgb/red", handleRGBRed);
  server.on("/rgb/green", handleRGBGreen);
  server.on("/rgb/blue", handleRGBBlue);
  server.on("/rgb/white", handleRGBWhite);
  server.on("/rgb/off", handleRGBOff);

  // Start the web server
  server.begin();
  Serial.println("HTTP server started with LED and sensor controls");
  check = true;
}

void WebSeverloop() {
  // Handle incoming client requests
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  InitWebsever();
  server.handleClient();
}