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
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>

#include "SPIFFS.h"
#include "time.h"
#include "web.h"

MFRC522DriverPinSimple ss_pin(4);

MFRC522DriverSPI driver{ss_pin}; 

MFRC522 mfrc522(driver);        

MFRC522::MIFARE_Key key;

const char* CONFIG_FILE = "/config.json";

const int rs = 27, en = 26, d4 = 32, d5 = 25, d6 = 13, d7 = 14;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define BUZZER_PIN 5

AsyncWebServer server(80);

struct Config {
  String wifi_ssid;
  String wifi_pass;
  String admin_name;
  String admin_pass;
  String bot_token;
  String chat_id;
  String sheet_url;
  String sheet_name;
  String api_endpoint_url;
} config;

WiFiClientSecure secured_client;
UniversalTelegramBot bot(config.bot_token, secured_client);

bool loadConfig() {
  if (!SPIFFS.exists(CONFIG_FILE)) return false;

  File file = SPIFFS.open(CONFIG_FILE, "r");
  if (!file) return false;

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();
  if (error) return false;

  config.wifi_ssid = doc["wifi_ssid"].as<String>();
  config.wifi_pass = doc["wifi_pass"].as<String>();

  config.admin_name = doc["admin_name"].as<String>();
  config.admin_pass = doc["admin_pass"].as<String>();

  config.bot_token = doc["bot_token"].as<String>();
  config.chat_id = doc["chat_id"].as<String>();

  config.sheet_url = doc["sheet_url"].as<String>();
  config.sheet_name = doc["sheet_name"].as<String>();

  config.api_endpoint_url = doc["api_endpoint_url"].as<String>();

  return true;
}

bool saveConfig() {
  StaticJsonDocument<512> doc;
  doc["wifi_ssid"] = config.wifi_ssid;
  doc["wifi_pass"] = config.wifi_pass;
  doc["admin_name"] = config.admin_name;
  doc["admin_pass"] = config.admin_pass;
  doc["bot_token"] = config.bot_token;
  doc["chat_id"] = config.chat_id;
  doc["sheet_url"] = config.sheet_url;
  doc["sheet_name"] = config.sheet_name;
  doc["api_endpoint_url"] = config.api_endpoint_url;

  File file = SPIFFS.open(CONFIG_FILE, "w");
  if (!file) return false;

  serializeJson(doc, file);
  file.close();
  return true;
}

String processor(const String& var) {
  if (var == "SSID") return config.wifi_ssid;
  if (var == "PASS") return config.wifi_pass;
  if (var == "USERNAME") return config.admin_name;
  if (var == "USER_PASS") return config.admin_pass;
  return String();
}

void telegram_notify(String notif){
  bool ok = bot.sendMessage(config.chat_id, notif, ""); 
}

void setup() {
  lcd.begin(20,4);
  WiFi.mode(WIFI_STA);
 
  Serial.begin(9600);  
  configTime( 25200, 0, "pool.ntp.org");
  WiFiManager wm; 

  if(!SPIFFS.begin(true)){
    Serial.println(F("An Error has occurred while mounting SPIFFS"));
    return;
  }

  if (!loadConfig()) {
    Serial.println("No config found, setting defaults...");
    config.wifi_ssid = "0xbdg";
    config.wifi_pass = "metschoo";
    config.admin_name = "admin";
    config.admin_pass = "admin";
    config.bot_token = "-";
    config.chat_id = "-";
    config.sheet_url = "-";
    config.sheet_name = "-";
    config.api_endpoint_url = "-";
    saveConfig();
  }

  bool check;
  check = wm.autoConnect(config.wifi_ssid.c_str(), config.wifi_pass.c_str());

  if(!check){
    Serial.println(F("connecting..."));
    lcd.setCursor(0,0);
    lcd.print(F("gagal terkoneksi"));
    lcd.setCursor(0,1);
    lcd.print(F("ke internet!!"));
  }

  lcd.clear();
  Serial.println(WiFi.localIP());
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  secured_client.setHandshakeTimeout(120000);
 
  pinMode(BUZZER_PIN, OUTPUT);
  lcd.setCursor(1, 0);
  lcd.print(F("Metschoo Attendance"));
  lcd.setCursor(3, 2);
  lcd.print(F("Tap kartu anda"));

  while (!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      if(!request->authenticate(config.admin_name.c_str(), config.admin_pass.c_str()))
          return request->requestAuthentication();
      request->send(200, "text/html", index_html);
  }); 

  server.on("/save-wifi", HTTP_POST, [](AsyncWebServerRequest *request){
    if(request->hasParam("ssid", true)) config.wifi_ssid = request->getParam("ssid", true)->value();
    if(request->hasParam("password", true)) config.wifi_pass = request->getParam("password", true)->value();

    if(saveConfig()) {
      request->send(200, "text/html", "<h2>WiFi Saved. Success...</h2>");
    } else {
      request->send(500, "text/plain", "Failed to save WiFi config");
    }
  });

  server.on("/save-admin", HTTP_POST, [](AsyncWebServerRequest *request){
    if(request->hasParam("username", true)) config.admin_name = request->getParam("username", true)->value();
    if(request->hasParam("password", true)) config.admin_pass = request->getParam("password", true)->value();

    if(saveConfig()) {
      request->send(200, "text/html", "<h2>Admin Saved. Success...</h2>");
         
    } else {
      request->send(500, "text/plain", "Failed to save Admin config");
    }
  });

  server.on("/save-telegram", HTTP_POST, [](AsyncWebServerRequest *request){
    if(request->hasParam("botToken", true)) config.bot_token = request->getParam("botToken", true)->value();
    if(request->hasParam("chatId", true)) config.chat_id = request->getParam("chatId", true)->value();

    if(saveConfig()) {
      request->send(200, "text/html", "<h2>Telegram Saved. Success...</h2>");
                      
    } else {
      request->send(500, "text/plain", "Failed to save Telegram config");
    }
  });

  server.on("/save-spreadsheet", HTTP_POST, [](AsyncWebServerRequest *request){
    if(request->hasParam("sheetUrl", true)) config.sheet_url = request->getParam("sheetUrl", true)->value();
    if(request->hasParam("sheetName", true)) config.sheet_name = request->getParam("sheetName", true)->value();

    if(saveConfig()) {
      request->send(200, "text/html", "<h2>Spreadsheet Saved. Success...</h2>");
                      
    } else {
      request->send(500, "text/plain", "Failed to save Spreadsheet config");
    }
  });

  server.on("/save-api", HTTP_POST, [](AsyncWebServerRequest *request){
    if(request->hasParam("apiUrl", true)) config.api_endpoint_url = request->getParam("apiUrl", true)->value();

    if(saveConfig()) {
      request->send(200, "text/html", "<h2>API Saved. Success...</h2>");
                      
    } else {
      request->send(500, "text/plain", "Failed to save API config");
    }
  });

  server.begin();
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
    http.begin(config.api_endpoint_url + uid);

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

         sendDataToSpreadsheet(urlEncode(nama), kelas, jurusan, urlEncode(config.sheet_name));
         telegram_notify(nama + " masuk pukul "+getTime());

         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print("NAMA:"+nama);
         lcd.setCursor(0,1);
         lcd.print("KELAS:"+kelas + " " + jurusan );
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

void sendDataToSpreadsheet(String nama, String kelas, String jurusan, String sheet){
  if (WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    String payload = "?sheet="+sheet+"&nama="+nama+"&kelas="+kelas+"&jurusan="+jurusan;

    http.begin(config.sheet_url + payload);

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
