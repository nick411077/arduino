#include <WiFi.h>
#include <SPIFFS.h>
#include <Servo.h>

#include <ESPAsyncWebServer.h>
#include <MFRC522.h>
#include <SPI.h>
int r1=4, g1=0, b1=2;
int r2=33, g2=25, b2=26;
int  RST_PIN =22, SS_PIN=21;
int m=12;

const char* ssid     = "Lavender";
const char* password = "12345678";

//WebServer設置
AsyncWebServer server(80);
MFRC522 mfrc522;
Servo RCmoto;
const int RCPinmoto = 27;


// Decode HTTP GET 設置
String RGB1 = String(0);
String RGB2 = String(0);
String R = String(255);
String G = String(255);
String B = String(255);

byte uid[]={0xB9, 0x89, 0xF6, 0x62};

const int freq = 5000; 
const int resolution = 8;

void setup() {
    ledcSetup(1, freq, resolution);
    ledcSetup(2, freq, resolution);
    ledcSetup(3, freq, resolution);
    ledcSetup(4, freq, resolution);
    ledcSetup(5, freq, resolution);
    ledcSetup(6, freq, resolution);
    ledcSetup(10, freq, resolution);
    ledcAttachPin(r1,1);
    ledcAttachPin(g1,2);
    ledcAttachPin(b1,3);
    ledcAttachPin(r2,4);
    ledcAttachPin(g2,5);
    ledcAttachPin(b2,6);
    ledcAttachPin(m,10);
    if(!SPIFFS.begin()){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
        }
    Serial.begin(115200);
    SPI.begin(); 
    mfrc522.PCD_Init(SS_PIN, RST_PIN);
    Serial.print(F("Reader "));
    Serial.print(F(": "));
    mfrc522.PCD_DumpVersionToSerial();
    RCmoto.attach(RCPinmoto,8,0,180,1000,2000);
    RCmoto.write(0);
    // Connect to Wi-Fi network with SSID and password
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_AP); //WIFI模式
    WiFi.softAP(ssid, password);
    IPAddress myIP = WiFi.softAPIP();
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    int args = request->args();
    Serial.println(args);
    for (int i = 0; i < args; i++)
    {
        Serial.print("Param name: ");
        Serial.println(request->argName(i));
        Serial.print("Param value: ");
        Serial.println(request->arg(i));
        Serial.println("------");
        if (request->argName(i) == "Rgb1")
        {
            RGB1 = request->arg(i);
            switch (RGB1.toInt())
            {       //控制
            case 1: // 左前
                ledcWrite(1, R.toInt());
                ledcWrite(2, G.toInt());
                ledcWrite(3, B.toInt());
                break;
            case 2: // 前
                ledcWrite(1, 0);
                ledcWrite(2, 0);
                ledcWrite(3, 0);
                break;
            }
        }
        else if (request->argName(i) == "Rgb2")
        {
            RGB2 = request->arg(i);
            switch (RGB2.toInt())
            {       //控制
            case 1: // 前
                ledcWrite(4, R.toInt());
                ledcWrite(5, G.toInt());
                ledcWrite(6, B.toInt());
                
                break;
            case 2: // 左
                ledcWrite(4, 0);
                ledcWrite(5, 0);
                ledcWrite(6, 0);
                break;
            }
        }
        else if (request->argName(i) == "Rcolor")
        {
            R = request->arg(i);
        }
        else if (request->argName(i) == "Gcolor")
        {
            G = request->arg(i);
        }
        else if (request->argName(i) == "Bcolor")
        {
            B = request->arg(i);
        }
    }
    request->send(SPIFFS, "/index.html", "text/html");
  });
  //server.onRequestBody([](AsyncWebServerRequest *request));
  server.on("/jquery-3.3.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/jquery-3.3.1.min.js", "text/javascript");
  });
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  //Serial.print("reading...");
  // 檢查是不是偵測到新的卡
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      // 顯示卡片的UID
      Serial.print(F("Card UID:"));
      dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size); // 顯示卡片的UID
      Serial.println();
      Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
      Serial.println(mfrc522.PICC_GetTypeName(piccType));  //顯示卡片的類型
      
      //把取得的UID，拿來比對我們指定好的UID
      bool they_match = true; // 初始值是假設為真 
      for ( int i = 0; i < 4; i++ ) { // 卡片UID為4段，分別做比對
        if ( uid[i] != mfrc522.uid.uidByte[i] ) { 
          they_match = false; // 如果任何一個比對不正確，they_match就為false，然後就結束比對
          break; 
        }
      }
      
      //在監控視窗中顯示比對的結果
      if(they_match){
        Serial.print(F("Access Granted!"));
        RCmoto.write(90);
        //ledcWriteTone(m,1730);
        delay(2000);
        RCmoto.write(0);
      }else{
        Serial.print(F("Access Denied!"));
      }
      mfrc522.PICC_HaltA();  // 卡片進入停止模式
    }
}

void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}