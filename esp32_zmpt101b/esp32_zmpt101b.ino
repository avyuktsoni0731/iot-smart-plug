#define SENSOR_PIN 36  // GPIO36 (ADC1_CH0)

void setup() {
  Serial.begin(115200); // Initialize serial communication
  analogReadResolution(12); // Set ADC resolution to 12 bits
}

void loop() {
  int rawValue = analogRead(SENSOR_PIN); // Read the analog value
  float voltage = (rawValue / 4095.0) * 3.3; // Convert to voltage (assuming 3.3V reference)
  Serial.print("Raw Value: ");
  Serial.print(rawValue);
  Serial.print(" | Voltage: ");
  Serial.println(voltage);
  delay(1000); // Delay for a second
}
