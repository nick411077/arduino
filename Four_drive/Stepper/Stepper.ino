#include <WiFi.h>
#include <AsyncTCP.h>       //異步處理函數庫 download https://github.com/me-no-dev/ESPAsyncTCP
#include <SPIFFS.h>                //檔案系統函數庫  
#include <ESPAsyncWebServer.h> //異步處理網頁伺服器函數庫 download https://github.com/me-no-dev/ESPAsyncWebServer
#include <AccelStepper.h>      //步進馬達函數庫
#include <Stepper.h>           //步進馬達函數庫

//步進設置
#define ENB 5
#define DIR 18
#define PUL 19 //Arduino給驅動器的腳位
AccelStepper stepper(1, 19, 18);
int MaxSpeed = 2000;    //最高速 空載2000
int Acceleration = 200; //加速度 空載200
//int Max = 1300;         //1:80= 16000轉

//碰撞感應器
#define SL 17
#define SR 16

//WiFi設置
const char *ssid = "Lavender";
const char *password = "12345678";

//WebServer設置
AsyncWebServer server(80);

// Decode HTTP GET 設置
String Ste = String(5); //網站請求的方向變數

//將 String轉換成int 
int StepValue;



void setup()
{
  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  Serial.begin(115200); //設定鮑率
  pinMode(PUL, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(ENB, OUTPUT);
  digitalWrite(DIR, HIGH);
  digitalWrite(ENB, LOW);
  stepper.setEnablePin(ENB);
  stepper.disableOutputs();
  stepper.setMaxSpeed(MaxSpeed);
  stepper.setAcceleration(Acceleration);
  stepper.setCurrentPosition(0);
  // Connect to Wi-Fi network with SSID and password
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
      Serial.print("Param name: ");
      Serial.println(request->argName(i));
      Serial.print("Param value: ");
      Serial.println(request->arg(i));
      Serial.println("------");
      if (request->argName(i) == "step")//GET網站方向狀態
      {
        Ste = request->arg(i);
        StepValue = Ste.toInt();//將 String轉換成int
        Step(StepValue);
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
  stepper.runSpeed();//持續旋轉
  if (digitalRead(SL) == 1 || digitalRead(SR) == 1)//如果左或右碰到微動開關離即停止
  {
    Step(2);
  }
}

void Step(int Step)
{
  switch (Step)//這個方式在灌漿車上有做過了速度都可以在調整
  {
  case 1://左轉
    stepper.setSpeed(200);//設定速度
    Serial.println("1");
    break;
  case 2://停
    stepper.setSpeed(0);
    Serial.println("2");
    break;
  case 3://右轉
    stepper.setSpeed(-200);
    Serial.println("3");
    break;
  }
}
