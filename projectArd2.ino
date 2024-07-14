#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo myServo;

String content = "";
String data = "";

String getCardHolderName(String uid) {
  if (uid == "14C357A7") {
    return "DEE";
  } else if (uid == "3323A9E4") {
    return "HARSHA";
  } else if (uid == "13F224E4") {
    return "LOL";
  } else {
    return "Unauthorised";
  }
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  myServo.attach(3);
  myServo.write(0);  // Servo in closed position
  Serial.println("CLEARDATA");
  Serial.println("LABEL,Time,UID,Name,Status");
}

void loop() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID tag :");
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Message : ");
    content.toUpperCase();
  String shortUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if (mfrc522.uid.uidByte[i] < 0x10) shortUID += "0";  // Add leading zero if necessary
      shortUID += String(mfrc522.uid.uidByte[i], HEX);
  }
  shortUID.toUpperCase();
  Serial.print("UID content: ");
    Serial.println(shortUID);
  String cardHolderName = getCardHolderName(shortUID);
  Serial.print("Card holder's name: ");
    Serial.println(cardHolderName);

     if (cardHolderName != "Unauthorised") {
      // Open the servo
      myServo.write(90);
      delay(2000);
      // Close the servo
      myServo.write(0);
      delay(2000);
    }

  // Action when card is detected
  Serial.print("DATA,");
  Serial.print("TIME,");
  Serial.print(shortUID);
  Serial.print(",");
  Serial.print(cardHolderName);
  
  Serial.print(",");
  Serial.println(cardHolderName == "Unauthorised" ? "Denied" : "Exit");
   // Close the servo

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
