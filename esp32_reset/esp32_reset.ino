#include <ESP.h>

void setup() {
  Serial.begin(115200);
  Serial.println("Restarting ESP32...");
  ESP.restart();
}

void loop() {
  // Your loop code
}