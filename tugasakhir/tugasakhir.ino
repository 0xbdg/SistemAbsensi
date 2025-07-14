#include <Wire.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "time.h"

MFRC522DriverPinSimple ss_pin(4);

MFRC522DriverSPI driver{ss_pin}; 

MFRC522 mfrc522(driver);        

MFRC522::MIFARE_Key key;

const String sheet_url = "SHEET_URL";
const String api_endpoint_url = "API_URL";

const int rs = 27, en = 26, d4 = 32, d5 = 25, d6 = 13, d7 = 14;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define BUZZER_PIN 5

void setup() {
  lcd.begin(20,4);
  WiFi.mode(WIFI_STA);
 
  Serial.begin(9600);  
  configTime( 25200, 0, "pool.ntp.org");
  WiFiManager wm;

  wm.autoConnect("Absensi", "password");
 
  pinMode(BUZZER_PIN, OUTPUT);
  lcd.setCursor(1, 0);
  lcd.print("Metschoo Attendance");
  lcd.setCursor(3, 2);
  lcd.print("Tap kartu anda");

  while (!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
        return;
  }
 
  Serial.print("Card UID: ");
  MFRC522Debug::PrintUID(Serial, (mfrc522.uid));
  Serial.println();

  String uid = getUID();
  Serial.println(uid);
  verifyData(uid);

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(1000);
  
}

String getDate(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  } 

  char year[5];
  strftime(year, sizeof(year),"%Y", &timeinfo);
  char month[3];
  strftime(month, sizeof(month), "%B", &timeinfo);
  char day[3];
  strftime(day, sizeof(day), "%d", &timeinfo);

  String yearStr = String(year);
  String monthStr = String(month);
  String dayStr = String(day);


  return dayStr+"/"+monthStr+"/"+yearStr;
}

String getTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
      Serial.println("Failed to obtain time");
  }

  char currentHour[3];
  strftime(currentHour, sizeof(currentHour), "%H", &timeinfo);
  char currentMinute[3];
  strftime(currentMinute, sizeof(currentMinute), "%M", &timeinfo);
  char currentSecond[3];
  strftime(currentSecond, sizeof(currentSecond), "%S", &timeinfo );

  String currentHourStr = String(currentHour);
  String currentMinuteStr = String(currentMinute);
  String currentSecondStr = String(currentSecond);

  return currentHourStr +":"+ currentMinuteStr+":"+currentSecondStr;
}

String getUID(){
  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uidString += "0"; 
    }
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }
  return uidString;
}

String urlEncode(String str) {
  String encoded = "";

  for (int i = 0; i < str.length(); i++) {
    if (str[i] == ' ') {
      encoded += "%20";  
    } else {
      encoded += str[i]; 
    }
  }
  
  return encoded;
}

void verifyData(String uid){
  if (WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    http.begin(api_endpoint_url + uid);

    int httpCode = http.GET();

    if (httpCode > 0){
       Serial.printf("%d\n", httpCode);

       String response = http.getString();

       DynamicJsonDocument doc(1024);
       DeserializationError error = deserializeJson(doc, response);
      
       if (error) {
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print("ERROR code:");
         lcd.setCursor(0,1);
         lcd.print(uid);
         lcd.setCursor(0,2);
         lcd.print("kartu belum terdaftar");
         lcd.setCursor(0,3);
         lcd.print("Coba hubungi teknisi");
         tone(BUZZER_PIN,3000);
         delay(2000);
         noTone(BUZZER_PIN);
         lcd.clear();
         lcd.setCursor(1,0);
         lcd.print("Metschoo Attendance");
         lcd.setCursor(3,2);
         lcd.print("Tap kartu anda");
         Serial.print("Failed to parse JSON: ");
         Serial.println(error.f_str());
         return;
       }
       else{
         String nama = doc["nama"];
         String kelas = doc["kelas"];
         String jurusan = doc["jurusan"];

         sendDataToSpreadsheet(urlEncode(nama), kelas, jurusan);

         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print("NAMA:"+String(nama));
         lcd.setCursor(0,1);
         lcd.print("KELAS:"+String(kelas) + " " + String(jurusan) );
         lcd.setCursor(0,2);
         lcd.print("TANGGAL:"+getDate());
         lcd.setCursor(0,3);
         lcd.print("WAKTU:"+getTime());
 
         tone(BUZZER_PIN, 1000);

         delay(2000);
         noTone(BUZZER_PIN);
         lcd.clear();
         lcd.setCursor(1,0);
         lcd.print("Metschoo Attendance");
         lcd.setCursor(3,2);
         lcd.print("Tap kartu anda");
         return;
       }
    }
    else {
       Serial.println(httpCode);
    }

    http.end();
  }
}

void sendDataToSpreadsheet(String nama, String kelas, String jurusan){
  if (WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    String payload = "?nama="+nama+"&kelas="+kelas+"&jurusan="+jurusan;

    http.begin(sheet_url + payload);

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
