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
#include <WiFiUdp.h>
#include <NTPClient.h>               

MFRC522DriverPinSimple ss_pin(4);

MFRC522DriverSPI driver{ss_pin}; 

MFRC522 mfrc522{driver};        

MFRC522::MIFARE_Key key;

const String sheet_url = "SHEET_URL";
const String api_endpoint_url = "ENDPOINT_URL";

const int rs = 27, en = 26, d4 = 32, d5 = 25, d6 = 13, d7 = 14;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define BUZZER_PIN 5

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 25200, 60000);

void setup() {
  lcd.begin(16,2);
  SPI.begin();
  WiFi.mode(WIFI_STA);
  Serial.begin(9600); 
  
  WiFiManager wm;

  wm.autoConnect("Absensi", "password");
  pinMode(BUZZER_PIN, OUTPUT);
  lcd.setCursor(0, 0);
  lcd.print(" Tap kartu anda");

  while (!Serial);       // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).
  
  mfrc522.PCD_Init();    // Init MFRC522 board.
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);  // Show details of PCD - MFRC522 Card Reader details.
  
  timeClient.begin();
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
  delay(1000);
  
}

String getDate(){
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime(); 
  
  int days = epochTime / 86400L;
  int year = 1970 + (days / 365);
  days %= 365;
  int month = 1 + (days / 30);
  int day = days % 30 + 1;

  return String(day) +"/"+ String(month)+"/"+String(year);
}

String getTime(){
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime(); 
  
  int currentHour = (epochTime % 86400L) / 3600;  // Extract hour
  int currentMinute = (epochTime % 3600) / 60;    // Extract minute
  int currentSecond = epochTime % 60;              // Extract second

  return String(currentHour) +":"+ String(currentMinute)+":"+String(currentSecond);
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
         lcd.setCursor(0,0);
         lcd.print(uid);
         lcd.setCursor(0,1);
         lcd.print("belum terdaftar");
         delay(4000);
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print(" Tap kartu anda");
         Serial.print("Failed to parse JSON: ");
         Serial.println(error.f_str());
         return;
       }
       else{
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print("DATE:"+getDate());
         lcd.setCursor(0,1);
         lcd.print("TIME:"+getTime());
         String nama = urlEncode(doc["nama"]);
         String kelas = doc["kelas"];
         String jurusan = doc["jurusan"];

         sendDataToSpreadsheet(nama, kelas, jurusan);


         delay(2000);
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print(" Tap kartu anda");
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
