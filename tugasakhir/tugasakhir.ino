#include <Wire.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <HTTPClient.h>
// Learn more about using SPI/I2C or check the pin assigment for your board: https://github.com/OSSLibraries/Arduino_MFRC522v2#pin-layout
MFRC522DriverPinSimple ss_pin(4);

MFRC522DriverSPI driver{ss_pin}; // Create SPI driver
//MFRC522DriverI2C driver{};     // Create I2C driver
MFRC522 mfrc522{driver};         // Create MFRC522 instance

MFRC522::MIFARE_Key key;

byte blockAddr = 2;
byte newBlockData[17] = {"JohnDoe"};
byte len = 18;
byte blockDataRead[18];

const String sheet_url = "SHEET_URL";

const int rs = 27, en = 26, d4 = 32, d5 = 25, d6 = 13, d7 = 14;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define BUZZER_PIN 5

void setup() {
  lcd.begin(16,2);
  SPI.begin();
  WiFi.mode(WIFI_STA);
  Serial.begin(9600); 

  pinMode(BUZZER_PIN, OUTPUT);
  WiFiManager wm;

  wm.autoConnect("Absensi", "password");

  lcd.setCursor(0, 0);
  lcd.print("Tap kartu anda");

  while (!Serial);       // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).
  
  mfrc522.PCD_Init();    // Init MFRC522 board.
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);	// Show details of PCD - MFRC522 Card Reader details.

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
	return;
  }

  lcd.setCursor(0, 0);
  lcd.print("Tap kartu anda");
  
  Serial.print("Card UID: ");
  MFRC522Debug::PrintUID(Serial, (mfrc522.uid));
  Serial.println();

  //writeDataFromCard(blockAddr, newBlockData);
  readDataFromCard(blockAddr, blockDataRead);

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void readDataFromCard(byte blockAddress, byte readBlockData[]){

  if (mfrc522.PCD_Authenticate(0x60, blockAddress, &key, &(mfrc522.uid)) != 0) {
    Serial.println("Authentication failed.");
    return;
  }

  if (mfrc522.MIFARE_Read(blockAddress, readBlockData, &len) != 0) {
    Serial.println("Read failed.");
  } else {
    String data = "";
    Serial.println("Read successfully!");
    Serial.print("Data in block ");
    Serial.print(blockAddress);
    Serial.print(": ");
    for (byte i = 0; i < 16; i++) {
      Serial.print(String((char)blockDataRead[i]));  // Print as character
      data += String((char)blockDataRead[i]);
    }
    Serial.println();

    if (data == "" && data.length() <= 0){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Kartu belum");
      lcd.setCursor(0, 1);
      lcd.print("terdaftar!!");
      tone(BUZZER_PIN, 1500);
      delay(500);
      tone(BUZZER_PIN, 1500);
      delay(500);
      noTone(BUZZER_PIN);
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tap kartu anda");
    }
    else{
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("nama:");
      lcd.setCursor(0, 1);
      lcd.print(data);
      tone(BUZZER_PIN, 3000);
      delay(1000);
      noTone(BUZZER_PIN);
      sendData(data);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tap kartu anda");
    }
  }
}

void writeDataFromCard(byte blockAddress, byte writeBlockData[]){

  if (mfrc522.PCD_Authenticate(0x60, blockAddress, &key, &(mfrc522.uid)) != 0) {
    Serial.println("Authentication failed.");
    return;
  }
  if (mfrc522.MIFARE_Write(blockAddress, writeBlockData, 16) != 0) {
    Serial.println("Write failed.");
  } else {
    Serial.print("Data written successfully in block: ");
    Serial.println(blockAddress); 
  }
}

void sendData(String data){
  if (WiFi.status() == WL_CONNECTED){
    HTTPClient http;

    http.begin(sheet_url + String(data));

    int status_code = http.GET();

    if (status_code > 0){
      Serial.println("data terkirim");
    }
    else{
      Serial.printf("Error dengan status: %d\n",status_code);
    }

    http.end();
  }
}
