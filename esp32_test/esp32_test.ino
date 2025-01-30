#include <WiFi.h>
#include <PubSubClient.h>

// Replace with your network credentials
const char* ssid = "Saad";
const char* password = "esp32saad";

// MQTT Broker IP
const char* mqtt_server = "34.16.11.88"; // Replace with your VM's public IP
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  client.setServer(mqtt_server, mqtt_port);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    setup();
  }
  client.loop();

  // Publish message to the MQTT broker
  const char* message = "Hello from ESP32!";
  client.publish("esp32/message", message);
  Serial.println("Message sent: Hello from ESP32!");

  delay(2000); // Send every 2 seconds
}
