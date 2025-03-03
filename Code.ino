#include <SPI.h>
#include <MFRC522.h>
#include <SD.h>

#define RST_PIN 9      // Reset pin for RFID
#define SS_PIN 10      // SS pin for RFID
#define SD_CS 4        // Chip select for SD card

MFRC522 rfid(SS_PIN, RST_PIN);  // Create RFID instance
String uid = "";

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card initialization failed!");
    return;
  }
  Serial.println("SD Card Initialized!");

  Serial.println("Place your ID card near the reader...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) {
    return;  // No card detected
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;  // Unable to read card
  }

  // Get UID in HEX format
  uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  Serial.print("Card UID: ");
  Serial.println(uid);

  // Check if UID already registered
  if (isRegistered(uid)) {
    Serial.println("Already Registered");
  } else {
    registerRFID(uid);
    Serial.println("RFID Registered");
  }

  // Stop encryption
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

bool isRegistered(String code) {
  File file = SD.open("rfid.txt");
  if (file) {
    while (file.available()) {
      String line = file.readStringUntil('\n');
      line.trim();
      if (line == code) {
        file.close();
        return true;
      }
    }
    file.close();
  }
  return false;
}

void registerRFID(String code) {
  File file = SD.open("rfid.txt", FILE_WRITE);
  if (file) {
    file.println(code);
    file.close();
    Serial.println("Saved in SD Card");
  } else {
    Serial.println("Error saving RFID to SD Card");
  }
}
