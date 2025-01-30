#define RELAY_PIN 25  // Pin connected to the relay IN pin

void setup() {
  pinMode(RELAY_PIN, OUTPUT);  // Set the relay pin as an output
  digitalWrite(RELAY_PIN, LOW); // Initially, turn the relay OFF
  Serial.begin(115200);  // Initialize serial communication
  Serial.println("Relay Test Starting...");
}

void loop() {
  Serial.println("Turning ON the Relay (Bulb ON)");
  digitalWrite(RELAY_PIN, HIGH);  // Turn the relay ON
  delay(2000);  // Wait for 2 seconds

  Serial.println("Turning OFF the Relay (Bulb OFF)");
  digitalWrite(RELAY_PIN, LOW);  // Turn the relay OFF
  delay(2000);  // Wait for 2 seconds
}
