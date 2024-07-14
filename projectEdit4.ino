#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <SPI.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

#define ir_enter 2
#define ir_back  4
#define ir_car1 5
#define ir_car2 6
#define ir_car3 7
#define ir_car4 8

int S1 = 0, S2 = 0, S3 = 0, S4 = 0;
int slot = 4;

// Function to map RFID UIDs to names
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

  pinMode(ir_car1, INPUT);
  pinMode(ir_car2, INPUT);
  pinMode(ir_car3, INPUT);
  pinMode(ir_car4, INPUT);
  pinMode(ir_enter, INPUT);
  pinMode(ir_back, INPUT);

  myservo.attach(3);
  myservo.write(0);

  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Car  parking");
  lcd.setCursor(0, 1);
  lcd.print(" System ");
  delay(2000);
  lcd.clear();

  Read_Sensor();
  Serial.println("CLEARDATA"); 
  Serial.println("LABEL,Time,UID,Name,Status");

  lcd.setCursor(0, 0);
  int total = S1 + S2 + S3 + S4;
  slot = slot - total;
}

void loop() {
  Read_Sensor();
  lcd.clear();
  lcd.setCursor(0, 0);
  if (S1 == 1) {
    lcd.print("S1:full");
  } else {
    lcd.print("S1:empt");
  }
  lcd.setCursor(9, 0);
  if (S2 == 1) {
    lcd.print("S2:full");
  } else {
    lcd.print("S2:empt");
  }
  lcd.setCursor(0, 1);
  if (S3 == 1) {
    lcd.print("S3:full");
  } else {
    lcd.print("S3:empt");
  }
  lcd.setCursor(9, 1);
  if (S4 == 1) {
    lcd.print("S4:full");
  } else {
    lcd.print("S4:empt");
  }

  // Check if all parking spots are occupied
  if (S1 == 1 && S2 == 1 && S3 == 1 && S4 == 1) {
    lcd.clear();
    lcd.println("  All slots ");
    lcd.setCursor(0, 1);
    lcd.println("  occupied");
  } else {
    // Look for new cards
    if (!mfrc522.PICC_IsNewCardPresent()) {
      return;
    }
    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) {
      return;
    }

    // Show UID on serial monitor
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

    // Capture the UID in a continuous format without spaces
    String shortUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if (mfrc522.uid.uidByte[i] < 0x10) shortUID += "0";  // Add leading zero if necessary
      shortUID += String(mfrc522.uid.uidByte[i], HEX);
    }
    shortUID.toUpperCase();  // Ensure the UID is in uppercase

    // Print UID for debugging
    Serial.print("UID content: ");
    Serial.println(shortUID);

    // Get the card holder's name
    String cardHolderName = getCardHolderName(shortUID);

    // Print the name for debugging
    Serial.print("Card holder's name: ");
    Serial.println(cardHolderName);

    // Display on LCD
    lcd.clear();
    if (cardHolderName == "Unauthorised") {
      lcd.setCursor(0, 0);
      lcd.print(" Access denied");
    } else {
      lcd.setCursor(0, 0);
      lcd.print(" Authorized:");
      lcd.setCursor(0, 1);
      lcd.print(cardHolderName);
    }

    // Perform action based on authorization
    if (cardHolderName != "Unauthorised") {
      // Open the servo
      myservo.write(90);
      delay(2000);
      // Close the servo
      myservo.write(0);
      delay(2000);
    }

    // Log data to serial monitor
    Serial.print("DATA,");
    Serial.print("TIME,");
    Serial.print(shortUID);
    Serial.print(",");
    Serial.print(cardHolderName);
    Serial.print(",");
    Serial.println(cardHolderName == "Unauthorised" ? "Denied" : "Entry");
  }

  delay(2000);  // Delay before looking for next card
}
void Read_Sensor() {
  S1 = 0, S2 = 0, S3 = 0, S4 = 0;
  if (digitalRead(ir_car1) == 0) {
    S1 = 1;
  }
  if (digitalRead(ir_car2) == 0) {
    S2 = 1;
  }
  if (digitalRead(ir_car3) == 0) {
    S3 = 1;
  }
  if (digitalRead(ir_car4) == 0) {
    S4 = 1;
  }
}
