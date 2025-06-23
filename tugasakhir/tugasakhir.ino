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
#include <RTClib.h>             

MFRC522DriverPinSimple ss_pin(4);

MFRC522DriverSPI driver{ss_pin}; 

MFRC522 mfrc522(driver);        

MFRC522::MIFARE_Key key;

const String sheet_url = "SHEET_URL";
const String api_endpoint_url = "API_URL";

const int rs = 27, en = 26, d4 = 32, d5 = 25, d6 = 13, d7 = 14;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

RTC_DS3231 rtc;

//RTC_DS1302 rtc;

#define RTC_INTERRUPT_PIN 33
#define BUZZER_PIN 5

void setup() {
  lcd.begin(20,4);
  SPI.begin();
  WiFi.mode(WIFI_STA);

  if(!rtc.begin()) {
    Serial.println("Couldn't find RTC!");
    Serial.flush();
    while (1) delay(10);
  }

  if(rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  Serial.begin(9600); 
  
  WiFiManager wm;

  wm.autoConnect("Absensi", "password");

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RTC_INTERRUPT_PIN, INPUT_PULLUP);

  lcd.setCursor(1, 0);
  lcd.print("Metschoo Attendance");
  lcd.setCursor(3, 2);
  lcd.print("Tap kartu anda");

  while (!Serial);
  
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
  delay(1000);
  
}

String getDate(){
  DateTime now = rtc.now();
  String year = String(now.year(), DEC);
  String month = (now.month() < 10 ? "0" : "") + String(now.month(), DEC);
  String day = (now.day() < 10 ? "0" : "") + String(now.day(), DEC);
  return String(day) +"/"+ String(month)+"/"+String(year);
}

String getTime(){
  DateTime now = rtc.now();
  String currentHour = (now.hour() < 10 ? "0" : "") + String(now.hour(), DEC); 
  String currentMinute = (now.minute() < 10 ? "0" : "") + String(now.minute(), DEC);
  String currentSecond = (now.second() < 10 ? "0" : "") + String(now.second(), DEC);
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
         lcd.print("ERROR CODE:"+uid);
         lcd.setCursor(0,1);
         lcd.print("kartu belum terdaftar");
         lcd.setCursor(0,2);
         lcd.print("Harap hubungi teknisi");
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
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print("NAMA:"+String(doc["nama"]));
         lcd.setCursor(0,1);
         lcd.print("KELAS:"+String(doc["kelas"]) + " " + String(doc["jurusan"]) );
         lcd.setCursor(0,2);
         lcd.print("TANGGAL:"+getDate());
         lcd.setCursor(0,3);
         lcd.print("WAKTU:"+getTime());
         String nama = urlEncode(doc["nama"]);
         String kelas = doc["kelas"];
         String jurusan = doc["jurusan"];

         sendDataToSpreadsheet(nama, kelas, jurusan);
         tone(BUZZER_PIN, 1000);

         delay(2000);
         noTone(BUZZER_PIN);
         lcd.clear();
         lcd.setCursor(1,0);
         lcd.print("Metschoo Attendance");
         lcd.setCursor(3,2);
         lcd.print("Tap kartu anda");
 
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
