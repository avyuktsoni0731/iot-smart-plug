#include <WiFi.h>
#include <ArduinoWebsockets.h>

const char* ssid = "OnePlus Nord CE 3 Lite 5G";
const char* password = "12345678";
const char* websocket_server = "ws://192.168.0.101:3001";  // Update with your server IP

using namespace websockets;
WebsocketsClient client;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");

    client.onMessage([](WebsocketsMessage message) {
        Serial.println("Received: " + message.data());
    });

    bool connected = client.connect(websocket_server);
    if (connected) {
        Serial.println("WebSocket Connected!");
    } else {
        Serial.println("WebSocket Connection Failed!");
    }
}

void loop() {
    if (client.available()) {
        float voltage = analogRead(36) * (3.3 / 4095.0);  // Adjust calculation as per sensor
        float current = analogRead(39) * (3.3 / 4095.0);  

        String data = "{\"voltage\": " + String(voltage) + ", \"current\": " + String(current) + "}";
        client.send(data);
        
        Serial.println("Sent: " + data);
    }

    client.poll();
    delay(2000);  // Send data every 2 sec
}
