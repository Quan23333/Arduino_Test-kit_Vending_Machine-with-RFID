#include <deprecated.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <require_cpp11.h>

#include <SPI.h>
#include <MFRC522.h>

#include <LiquidCrystal_I2C.h>
//LCD define
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
#define sensor 3
#define buzzer 4

int cylinder1 = 7;
int cylinder2 = 6;
int Dcmotor   = 5;


void setup() {
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

  pinMode(cylinder1, OUTPUT);
  pinMode(cylinder2, OUTPUT);
  pinMode(Dcmotor, OUTPUT);
  pinMode(buzzer, OUTPUT);

  digitalWrite(cylinder1, HIGH);
  digitalWrite(cylinder2, HIGH);
  digitalWrite(Dcmotor, HIGH);

  lcd.init();  // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(5, 0);
  lcd.print("PLEASE ");
  lcd.setCursor(4, 1);
  lcd.print("SCAN HERE");
}

void loop() {
  lcd.init();
  lcd.setCursor(5, 0);
  lcd.print("PLEASE ");
  lcd.setCursor(4, 1);
  lcd.print("SCAN HERE");

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  if (content.substring(1) == "A3 04 95 4D") //change here the UID of the card/cards that you want to give access
  {
    multiCallCylinder(1);
    motorRun();
  }
  else if (content.substring(1) == "51 0E 1F 22") //change here the UID of the card/cards that you want to give access
  {
    multiCallCylinder(2);
    motorRun();
  }
  else   {
    Serial.println(" Wrong Card");
    lcd.init();
    lcd.setCursor(5, 0);
    lcd.print("Wrong");
    lcd.setCursor(5, 1);
    lcd.print("Card");
    buzzerBlink(3, 500);
  }
}

void buzzerBlink(int blinkTime, int frequency) {
  for (int i = 0; i <= blinkTime; i++ ) {
    digitalWrite(buzzer, HIGH);
    delay(frequency);
    digitalWrite(buzzer, LOW);
    delay(frequency);
  }
}

void multiCallCylinder(int call) {
  lcd.init();
  switch (call) {
    case 1:
      Serial.println("Case1");
      lcd.setCursor(7, 0);
      lcd.print("1");
      lcd.setCursor(7, 1);
      lcd.print("OUT");
      callCylinder(cylinder1, 1000);
      break;
    case 2:
      Serial.println("Case2");
      lcd.setCursor(6, 0);
      lcd.print("1,2");
      lcd.setCursor(6, 1);
      lcd.print("OUT");
      callCylinder(cylinder1, 1000);
      callCylinder(cylinder2, 1000);
      break;
    default:
      break;
  }
}

void callCylinder(int nameCylinder, int delayTime) {
  Serial.print(nameCylinder);
  Serial.println(" is Active");
  digitalWrite(nameCylinder, LOW);
  delay(delayTime);
  digitalWrite(nameCylinder, HIGH);
  delay(delayTime);
}

void motorRun() {
  while (true) {
    int val = digitalRead(sensor);
    digitalWrite(Dcmotor, LOW);
    Serial.println("Motor Run");
    if (val == LOW) {
      Serial.println("Motor Stop");
      digitalWrite(Dcmotor, HIGH);
      break;
    }
  }
}
