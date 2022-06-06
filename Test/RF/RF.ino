#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>       //異步處理函數庫 download https://github.com/me-no-dev/AsyncTCP
#include <SPIFFS.h>                //檔案系統函數庫  
#include <ESPAsyncWebServer.h> //異步處理網頁伺服器函數庫 download https://github.com/me-no-dev/ESPAsyncWebServer
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();



//WiFi設置
const char *ssid = "Lavender";
const char *password = "12345678";

//WebServer設置
AsyncWebServer server(80);

String Car = String(2); //網站請求的方向變數

int CarValue;

bool CarTF;

void setup() 
{
  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  Serial.begin(115200); //設定鮑率
  mySwitch.enableTransmit(27);
  mySwitch.setProtocol(1);
  mySwitch.setPulseLength(268);
  mySwitch.setRepeatTransmit(15);
  CarValue = Car.toInt();//將 String轉換成int
  Serial.print("Connecting to ");
  Serial.println(ssid);             //顯示SSID
  WiFi.mode(WIFI_AP);               //WIFI AP模式
  WiFi.softAP(ssid, password);      //設定SSID 及密碼
  IPAddress myIP = WiFi.softAPIP(); //顯示ESP IP
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    int args = request->args();
    //Serial.println(args);
    for (int i = 0; i < args; i++)
    {
      Serial.print("Param name: ");//列出GET名稱
      Serial.println(request->argName(i));
      Serial.print("Param value: ");
      Serial.println(request->arg(i));
      Serial.println("------");
      if (request->argName(i) == "car")//GET網站方向狀態
      {
        Car = request->arg(i);
        CarValue = Car.toInt();//將 String轉換成int
        CarTF = 1;
      }

    }
    request->send(SPIFFS, "/index.html", "text/html");
  });
  //server.onRequestBody([](AsyncWebServerRequest *request));
  server.on("/jquery-3.3.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/jquery-3.3.1.min.js", "text/javascript");
  });
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.gatewayIP());
  server.begin();
}

void loop()
{
  if (CarTF == 1)
  {
    Serial.println(CarValue);
    switch (CarValue)
    {
    case 1:
      mySwitch.send("011111000100110011000000");
      break;
    case 2:
      mySwitch.send("011111000100110000110000");
      break;
    case 3:
      mySwitch.send("011111000100110000001100");
      break;
    }
    delay(1000);
    CarTF = 0;
  }
}
