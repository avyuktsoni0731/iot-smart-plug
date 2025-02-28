#include <WiFiManager.h>          // WiFi Manager
#include <WebSocketsClient.h>      // WebSockets
#include <ArduinoJson.h>           // JSON
#include <Preferences.h>           // Store Device ID
// #include <DHT.h>                   // Temperature Sensor
// #include <Wire.h>                  // I2C Communication
// #include <Adafruit_INA219.h>       // Voltage & Current Sensor
#include <ESP32Servo.h>            // Relay Control

// #define DHTPIN 4                  // DHT Sensor Pin
// #define DHTTYPE DHT22             // DHT Type (DHT11 or DHT22)
#define RELAY_PIN 25              // Relay Pin for On/Off Control

// DHT dht(DHTPIN, DHTTYPE);
// Adafruit_INA219 ina219;
Preferences preferences;
WebSocketsClient webSocket;

String device_id;
bool isConnected = false;
bool device_status = false; // Default: OFF

void setup() {
  Serial.begin(115200);
  
  // Start Sensors
  // dht.begin();
  ina219.begin();

  // Load stored device ID
  preferences.begin("esp32", false);
  device_id = preferences.getString("device_id", "");

  // WiFi Manager setup
  WiFiManager wm;
  if (!wm.autoConnect("ESP32-Setup")) {
    Serial.println("Failed to connect, rebooting...");
    ESP.restart();
  }
  Serial.println("WiFi Connected!");

  // WebSocket setup
  webSocket.begin("your-websocket-server.com", 8080, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // If no device_id, request from dashboard
  if (device_id == "") {
    Serial.println("Waiting for device setup...");
  }
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_TEXT:
      Serial.printf("Message Received: %s\n", payload);
      
      // Parse JSON
      DynamicJsonDocument doc(256);
      deserializeJson(doc, payload);
      
      if (doc["setup_device"] && doc["device_id"]) {
        device_id = doc["device_id"].as<String>();
        preferences.putString("device_id", device_id);
        Serial.printf("Device ID set: %s\n", device_id.c_str());
      }
      
      if (doc["command"] == "ON") {
        digitalWrite(RELAY_PIN, HIGH);
        device_status = true;
      } else if (doc["command"] == "OFF") {
        digitalWrite(RELAY_PIN, LOW);
        device_status = false;
      }
      break;
  }
}

void loop() {
  webSocket.loop();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Reconnecting...");
    WiFi.reconnect();
  }

  if (device_id != "" && isConnected) {
    sendData();
  }

  delay(5000);
}

void sendData() {
  float temperature = dht.readTemperature();
  float voltage = ina219.getBusVoltage_V();
  float current = ina219.getCurrent_mA();

  DynamicJsonDocument doc(256);
  doc["device_id"] = device_id;
  doc["status"] = device_status ? "ON" : "OFF";
  doc["voltage"] = voltage;
  doc["current"] = current;
  doc["temperature"] = temperature;

  String jsonStr;
  serializeJson(doc, jsonStr);
  webSocket.sendTXT(jsonStr);
  Serial.println("Data Sent: " + jsonStr);
}
