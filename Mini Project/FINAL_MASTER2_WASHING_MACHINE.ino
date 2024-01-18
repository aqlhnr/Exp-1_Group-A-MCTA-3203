#include <Wire.h>
#include <SoftwareSerial.h>
SoftwareSerial btSerial(0, 1);  // RX, TX pins for Bluetooth module

int sensorpin1 = A0;
int mode = 0;  // Variable to track the current mode
boolean btConnected = false;
char key;

const int buttonPin = 2;  // Pin to connect the push button
int countdown = 9;        // Initial countdown time in seconds

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // Set button pin as input with internal pull-up resistor
  pinMode(0, INPUT);
  pinMode(1, OUTPUT);
  btSerial.begin(9600);
  Wire.begin();  // Initialize I2C communication
  Serial.begin(9600);
  Serial.println("Master Arduino - Push Button");
}

void loop() {
  int value = analogRead(sensorpin1);

  Serial.println(value);
  // Determine mode based on potentiometer value
  if (value < 100) {
    mode = 0;
  } else {
    mode = 1;
  }
  // Perform actions based on the mode
  if (mode == 0) {
    // Actions for mode 0
    Serial.println("Mode 0: manual");
    // delay(1000);
    // Wait for the button press with debounce
    if (digitalRead(buttonPin) == LOW) {
      Serial.println("Button Pressed");
      Wire.beginTransmission(9);  // Slave address
      Wire.write(1);              // Send a byte indicating button press
      Wire.endTransmission();
      Serial.println("Pushbutton Value");
      delay(200);  // Debounce delay
    }

    //Check if the countdown has started
    // if (countdown > 0) {
    //   Serial.print("Time Left: ");
    //   Serial.println(countdown);
    //   delay(1000);  // Wait for 1 second
    //   countdown--;
    // }
    delay(1000);
  }

  else {
    // Actions for mode 1
    Serial.println("Mode 1: auto");
    if (btSerial.available()) {

      Serial.write(btSerial.read());


      if (Serial.available())
        btSerial.write(Serial.read());

      char koi = Serial.read();
      Serial.println(koi);

      key = koi;
      switch (key) {
        case 's':
          Wire.beginTransmission(9);  // Slave address
          Wire.write(1);              // Send a byte indicating button press
          Wire.endTransmission();
          Serial.println("Pushbutton Value");
          delay(200);  // Debounce delay
      }
    }delay(1000);
  }
}
