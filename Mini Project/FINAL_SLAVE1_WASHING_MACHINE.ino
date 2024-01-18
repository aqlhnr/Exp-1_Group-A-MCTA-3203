#include <Wire.h>
#include <Servo.h>

const int servoPin = 9;       
const int buzzerPin = 10;     
const int redLedPin = 11;     
const int greenLedPin = 12;   
const int blueLedPin = 13;    
const int segmentPins[] = {8, 7, 4, 5, 6, 0, 3}; // segments a, b, c, d, e, f, g
const int buttonPin = 2;  // Push button pin on master Arduino

Servo myservo;
bool motorRunning = false;
bool redLedBlinking = false;
bool blueLedOn = false;
bool sequenceStarted = false;
unsigned long lastButtonPressTime = 0;
unsigned long lastBlinkTime = 0;
int countdown = 9;  // Initial countdown (assume to be in minutes but we use seconds instead because easy to read)
bool countdownEnabled = false;  // Flag to indicate if countdown is enabled
bool secondPhase = false;  // Flag to indicate if the second phase has started

void setup() {
  Wire.begin(9);                
  Wire.onReceive(receiveEvent);  
  pinMode(servoPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  
  myservo.attach(servoPin);
  myservo.writeMicroseconds(1500);

  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }

  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("Please Press the Pushbutton");
}

void loop() {
  // Wait for the button press with debounce
  if (digitalRead(buttonPin) == LOW) {
    Serial.println("Button Pressed");
    Wire.beginTransmission(9);  // Slave address
    Wire.write(1);  // Send a byte indicating button press
    Wire.endTransmission();
   // Serial.println("Command Sent to Slave");
    delay(200);  // Debounce delay
  }

  if (sequenceStarted) {
    startSequence();
  }
}

void receiveEvent() {
  int command = Wire.read();  // Read the command from the master

  // Master has sent a command
  if (command == 1) {
    Serial.println("Laundry Start");
    sequenceStarted = true;
    countdownEnabled = true;  // Start the countdown
  }
}

void startSequence() {
  // Start the continuous rotation servo
  myservo.writeMicroseconds(2000);
  motorRunning = true;
  redLedBlinking = true;
  lastButtonPressTime = millis();
  Serial.println("Washing Start : 10 minutes ");

  while (motorRunning) {
    // Check if the motor has been running for 10 seconds
    if (millis() - lastButtonPressTime >= 10000) {
      // Stop the continuous rotation servo
      myservo.writeMicroseconds(1500);
      motorRunning = false;
      Serial.println("Washing Done");

      // Turn off the red LED
      digitalWrite(redLedPin, LOW);
      redLedBlinking = false;

      // Turn on the blue LED to signal rinse mode
      digitalWrite(blueLedPin, HIGH);
      Serial.println("Rinse On : 5 minutes ");
      blueLedOn = true;

      // Record the time the blue LED turned on
      unsigned long blueLedOnTime = millis();

// Continue with the rest of the sequence after the blue LED has been on for 5 seconds
      while (blueLedOn && millis() - blueLedOnTime < 5000) {
        // Optional: You can add other tasks here if needed
        // Update the countdown during the rinse mode
        countdown = 5 - ((millis() - lastButtonPressTime - 10000) / 1000);
        displayDigit(countdown);
        delay(1000); // Delay to update the countdown every second
      }

      // Turn off the blue LED
      digitalWrite(blueLedPin, LOW);
      blueLedOn = false;
      Serial.println("Rinse Finished");

      // Turn on the green LED
      digitalWrite(greenLedPin, HIGH);
      Serial.println("Laundry Done");

      // Turn on the buzzer
      digitalWrite(buzzerPin, HIGH);
      Serial.println("Yeay");

      // Wait for 5 seconds with the buzzer and green LED on
      delay(5000);

      // Turn off the buzzer, green LED
      digitalWrite(buzzerPin, LOW);
      digitalWrite(greenLedPin, LOW);
      Serial.println("Washing Machine Off");

      // Reset sequence flag
      sequenceStarted = false;
    }

    // Blink the red LED while the servo is running
    if (redLedBlinking && millis() - lastBlinkTime >= 500) {
      digitalWrite(redLedPin, !digitalRead(redLedPin));
      lastBlinkTime = millis();
    }

    // Update the countdown during the servo and LED sequence
    countdown = 9 - ((millis() - lastButtonPressTime) / 1000);
    displayDigit(countdown);
    delay(1000); // Delay to update the countdown every second
  }

  // Display the final countdown value
  displayDigit(countdown);
}

void displayDigit(int digit) {
  // Turn off all segments initially
  turnOff();

  // Define the segments for each digit
  const int digitSegments[][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}  // 9
  };

  // Display the segments for the given digit
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], digitSegments[digit][i]);
  }
}

void turnOff() {
  // Turn off all segments
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], LOW);
  }
}