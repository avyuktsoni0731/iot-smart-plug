// Pin Definitions
const int voltageSensorPin = 36;  // ZMPT101B sensor (Voltage)
const int currentSensorPin = 39;  // ACS712 sensor (Current)
const int relayPin = 25;          // Relay to control the bulb

int counter = 1;

// Calibration Constants (Adjust as needed)
const float VREF = 3.3;  // ESP32 ADC Reference Voltage
const int ADC_MAX = 4095; // ESP32 ADC Resolution
const float ACS712_SENSITIVITY = 0.185; // ACS712 5A version (change if using 20A or 30A)

// Offset values (These may need calibration)
float voltageOffset = 1.65;  // ZMPT101B outputs ~1.65V at 0V AC
float currentOffset = 2.5;   // ACS712 outputs ~2.5V at 0A current

void setup() {
    Serial.begin(115200);

    // Configure relay as output
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW); // Start with relay OFF
}

void loop() {
    // Read raw ADC values
    int rawVoltage = analogRead(voltageSensorPin);
    int rawCurrent = analogRead(currentSensorPin);

    // Convert to voltage (ADC to real voltage)
    float sensorVoltage = (rawVoltage * VREF) / ADC_MAX;
    float sensorCurrent = (rawCurrent * VREF) / ADC_MAX;

    // Convert to actual values
    float voltage = (sensorVoltage - voltageOffset) * (230.0 / 2.5); // Adjust scaling
    float current = (sensorCurrent - currentOffset) / ACS712_SENSITIVITY;

    // Print values to Serial Monitor
    Serial.print("Loop No: ");
    Serial.println(counter);
    Serial.print("Voltage Sensor: ");
    Serial.print(voltage, 2);
    Serial.print(" V | Current Sensor: ");
    Serial.print(current, 2);
    Serial.println(" A");

    counter++;

    // Toggle Relay Every 5 Seconds
    digitalWrite(relayPin, HIGH); // Turn ON
    delay(5000);
    digitalWrite(relayPin, LOW);  // Turn OFF
    delay(5000);
}
