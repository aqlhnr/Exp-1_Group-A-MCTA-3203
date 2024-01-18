#include <Wire.h>
#include <Servo.h>

Servo myServo;  // Create a Servo object

void setup()
{
  Wire.begin(4);                // join I2C bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output

  myServo.attach(9);  // Attach the servo to pin 9 or replace with your pin
}

void loop()
{
  delay(100);
}

// Function that executes whenever data is received from the master
// This function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while (1 < Wire.available()) // Loop through all but the last
  {
    char c = Wire.read(); // Receive byte as a character
    Serial.print(c);      // Print the character
  }
  
  int x = Wire.read();  // Receive byte as an integer
  Serial.println(x);    // Print the integer

  // Check if x (received from master) is a specific value indicating the correct UID
  if (x == 0xC3, 0x69, 0x16, 0x13) {  // Replace 42 with the actual value you expect

    // Add your servo control code here
    myServo.write(90);  // Assuming 90 is the angle for the desired servo position
    delay(1000);        // Wait for the servo to reach the position

    // You can add more servo control code or perform other actions here

    // Move the servo back to the initial position
    myServo.write(0);   // Assuming 0 is the angle for the initial position
    delay(1000); 
  }
}
