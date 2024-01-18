#include <Wire.h>

const int buttonPin = 2;  // Pin to connect the push button
int countdown = 9;  // Initial countdown time in minutes (assume seconds for easier read)

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // Set button pin as input with internal pull-up resistor
  Wire.begin();  // Initialize I2C communication
  Serial.begin(9600);  // Initialize serial communication at 9600 bps
}

void loop() {
  // Wait for the button press with debounce
  if (digitalRead(buttonPin) == LOW) {
    Serial.println("Button Pressed");  // Print a message to serial monitor when button is pressed
    Wire.beginTransmission(9);  // Start I2C communication with slave address 9
    Wire.write(1);  // Send a byte (value 1) indicating button was pressed
    Wire.endTransmission();  // End I2C communication
    delay(100);  // Debounce delay to prevent multiple readings from a single button press
  }
}
