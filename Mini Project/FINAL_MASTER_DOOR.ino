#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10  // Define the slave select pin for the RFID module
#define RST_PIN 9  // Define the reset pin for the RFID module

MFRC522 rfid(SS_PIN, RST_PIN); // Create an instance of the RFID library

LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 columns and 2 rows

#define SLAVE_ADDRESS 4  // Replace with the actual slave address of the servo Arduino

void setup() {

  //Wire.begin(); 
  Serial.begin(9600);
  SPI.begin();       // Init SPI bus
  rfid.PCD_Init();    // Init RFID module

  lcd.begin();   // Init LCD
  lcd.backlight();    // Turn on the backlight

  Wire.begin();
  
  lcd.print("Place RFID tag");
}

void loop() {
  // Look for new RFID cards
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    lcd.clear();
    Serial.print("UID:");
    
    //Display UID on LCD
    for (byte i = 0; i < rfid.uid.size; i++) {
    lcd.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    lcd.print(rfid.uid.uidByte[i], HEX);
    }
    
    // Check if the UID matches your specified UID
    if (checkUID()) {
      lcd.setCursor(0, 1);
      //lcd.print("AG");
      lcd.print(" Door Open");
      //openDoor(); //call function to open the door
      sendDataToSlave("Door ON");  // Send data to the servo Arduino
      // Add your code to turn on the washing machine here
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Access Denied");
    }

    delay(1000);  // Display the message for 2 seconds
    lcd.clear();
    lcd.print("User ID");
    delay(50);   // Delay to avoid multiple reads
  }
}

boolean checkUID() {
  // Replace this with your UID. You can find it using the example in the MFRC522 library.
  byte correctUID[] = {0xC3, 0x69, 0x16, 0x13};

  if (rfid.uid.size == sizeof(correctUID)) {
    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] != correctUID[i]) {
        return false;
      }
    }
    return true;
  }
  return false;
}

void sendDataToSlave(String data) {
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(data.c_str());  // Send the data as a C-style string
  Wire.endTransmission();
}

