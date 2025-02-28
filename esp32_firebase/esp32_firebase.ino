#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "Avyukt_2.4GHz";
const char* password = "avyukt@31";

// WebSocket server details
const char* websocket_server_host = "192.168.1.14";
const int websocket_server_port = 8080;

// Interval for sending data (500ms = 0.5 seconds)
const unsigned long sendInterval = 2000;
unsigned long previousMillis = 0;
unsigned long reconnectMillis = 0;
const unsigned long reconnectInterval = 6000; // Check connection every 3 seconds

// Connection monitoring
bool isConnected = false;
int connectionAttempts = 0;
const int maxConnectionAttempts = 10;

// Heartbeat to keep connection alive
unsigned long heartbeatMillis = 0;
const unsigned long heartbeatInterval = 10000; // Send heartbeat every 10 seconds

// Initialize WebSocket client
using namespace websockets;
WebsocketsClient client;

// Function to generate random metrics
void generateRandomMetrics(float &voltage, float &current, float &power) {
  // Generate values with slight variations
  voltage = random(215, 225) + (random(0, 100) / 100.0);
  current = random(45, 55) / 10.0 + (random(0, 100) / 100.0);
  power = voltage * current;
}

// Function to connect to WebSocket server
bool connectToWebSocket() {
  Serial.println("Connecting to WebSocket server...");
  bool connected = client.connect(websocket_server_host, websocket_server_port, "/");
  
  if (connected) {
    Serial.println("Connected to WebSocket server!");
    isConnected = true;
    connectionAttempts = 0;
    
    // Setup message handler
    client.onMessage([](WebsocketsMessage message) {
      Serial.println("Got Message: " + message.data());
    });
    
    // Setup events
    client.onEvent([](WebsocketsEvent event, String data) {
      if (event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connection opened");
      } else if (event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connection closed");
        isConnected = false;
      } else if (event == WebsocketsEvent::GotPing) {
        Serial.println("Got a ping!");
        client.pong();
      } else if (event == WebsocketsEvent::GotPong) {
        Serial.println("Got a pong!");
      }
    });
    
    return true;
  } else {
    Serial.println("Connection failed");
    isConnected = false;
    connectionAttempts++;
    
    if (connectionAttempts >= maxConnectionAttempts) {
      Serial.println("Too many failed attempts. Restarting ESP32...");
      ESP.restart();
    }
    
    return false;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Give the serial monitor time to start
  
  Serial.println("\n\n=== ESP32 WebSocket Test Client ===");
  
  // Initialize random seed
  randomSeed(analogRead(36)); // Using an unconnected analog pin for better randomness
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  
  Serial.print("Connecting to WiFi");
  int wifiAttempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    wifiAttempts++;
    
    if (wifiAttempts > 20) { // If can't connect to WiFi in 10 seconds
      Serial.println("\nWiFi connection timed out. Restarting...");
      ESP.restart();
    }
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Initial connection to WebSocket server
  connectToWebSocket();
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Handle WiFi disconnection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    
    // Wait for connection
    int wifiRetries = 0;
    while (WiFi.status() != WL_CONNECTED && wifiRetries < 20) {
      delay(500);
      Serial.print(".");
      wifiRetries++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi reconnected");
    } else {
      Serial.println("WiFi reconnection failed. Restarting ESP32...");
      ESP.restart();
    }
  }
  
  // Poll for WebSocket events (this keeps the connection alive)
  if (isConnected) {
    client.poll();
  }
  
  // Check if we need to reconnect
  if (!isConnected && (currentMillis - reconnectMillis >= reconnectInterval)) {
    reconnectMillis = currentMillis;
    connectToWebSocket();
  }
  
  // Send heartbeat to keep the connection alive
  if (isConnected && (currentMillis - heartbeatMillis >= heartbeatInterval)) {
    heartbeatMillis = currentMillis;
    client.send("ping"); // Simple ping to keep connection alive
    Serial.println("Sent heartbeat ping");
  }
  
  // Send metrics if connected and it's time to do so
  if (isConnected && (currentMillis - previousMillis >= sendInterval)) {
    previousMillis = currentMillis;
    
    // Generate random metrics
    float voltage, current, power;
    generateRandomMetrics(voltage, current, power);
    
    // Create JSON document
    StaticJsonDocument<256> doc;
    doc["device_id"] = "ESP32_TEST_DEVICE";
    doc["timestamp"] = currentMillis;
    doc["voltage"] = voltage;
    doc["current"] = current;
    doc["power"] = power;
    doc["status"] = true; // Device status (ON)
    
    // Serialize JSON to string
    String jsonString;
    serializeJson(doc, jsonString);
    
    // Send data
    bool sent = client.send(jsonString);
    
    // Debug output
    if (sent) {
      Serial.println("Sent: " + jsonString);
    } else {
      Serial.println("Failed to send data. Connection might be closed.");
      isConnected = false; // Mark for reconnection
    }
  }
  
  // Add a small delay to prevent overwhelming the ESP32
  delay(10);
}