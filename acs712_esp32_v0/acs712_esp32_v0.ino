const int analogPin = A0; // Pin where ACS712 OUT is connected
int rawValue = 0;
float voltage = 0.0;
float current = 0.0;
const float sensitivity = 0.185; // Adjust according to your ACS712 model

void setup() {
  Serial.begin(115200); // Initialize serial communication
}

void loop() {
  rawValue = analogRead(analogPin); // Read the raw value from the sensor
  voltage = (rawValue / 4096.0) * 3.3; // Convert to voltage (ESP32 ADC is 12-bit)
  current = (voltage - 2.5) / sensitivity; // Calculate current (adjusted for 2.5V zero)

  Serial.print("Raw Value: ");
  Serial.print(rawValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage);
  Serial.print(" V | Current: ");
  Serial.print(current);
  Serial.println(" A");

  delay(1000); // Wait for a second
}
