#include <WiFi.h>
#include <AsyncTCP.h>       //異步處理函數庫 download https://github.com/me-no-dev/ESPAsyncTCP
#include <SPIFFS.h>                //檔案系統函數庫  
#include <Servo.h>             //伺服馬達函數庫
#include <ESPAsyncWebServer.h> //異步處理網頁伺服器函數庫 download https://github.com/me-no-dev/ESPAsyncWebServer

//PWM設置
#define RCFPin 32
#define RCBPin 33
#define RCLPin 25
#define RCRPin 26
Servo RCF;
Servo RCB;
Servo RCL;
Servo RCR;

//超音波設置
#define TRIG 23
#define ECHO 22

//WiFi設置
const char *ssid = "Lavender";
const char *password = "12345678";

//WebServer設置
AsyncWebServer server(80);

// Decode HTTP GET 設置
String Car = String(2); //網站請求的方向變數
String Pow = String(45); //網站請求的出力變數
String Stop = String(0);
//將 String轉換成int 
int CarValue;
int PowValue;
int StopValue;

int val = 0; //加速度變數
uint8_t status = 1; //為了loop不要重複運行設定狀態變數只運行一次

void setup()
{
  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  Serial.begin(115200); //設定鮑率
  RCF.attach(RCFPin, 1000, 2000);
  RCB.attach(RCBPin, 1000, 2000);
  RCF.write(90);
  RCB.write(90);
  RCL.attach(RCLPin);
  RCR.attach(RCRPin);
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
      if (request->argName(i) == "car")//GET網站方向狀態
      {
        Car = request->arg(i);
        CarValue = Car.toInt();//將 String轉換成int
        StopValue = 0;
        status = 0;//更新狀態
      }
      else if (request->argName(i) == "pow")//GET網站出力狀態
      {
        Pow = request->arg(i);
        PowValue = Pow.toInt();//將 String轉換成int
      }
      else if (request->argName(i) == "stop")//GET網站手煞車狀態
      {
        Stop = request->arg(i);
        StopValue = Stop.toInt();//將 String轉換成int
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
  if (status == 0)//讀取狀態
  {
    moto();
  }
  else if (StopValue == 1 || Ultrasound(TRIG,ECHO) <= 40)//如果接收P檔或超音波小於40cm就停止
  {
    STOP();
  }
}

void moto()//馬達控制 目前還要修正操作順暢度 可能會使用ESP32的第二核心運行減少延遲
{
  RCR.write(180);//釋放煞車
  RCL.write(180);//釋放煞車
  int pow = PowValue;//加速度值
  int RC = RCF.read();//初始讀取馬達狀態
  int RCS = RCF.read();//停止中時持續讀取馬達狀態
  int time = 10;//delay時間
  while (CarValue == 1)//前
  {
    if (RC == 90)//如果停就加速度前進
    {
      RCF.write(90 + val);//加速度
      RCB.write(90 + val);//加速度
      Serial.print("SetValue:");
      Serial.println(RCF.read());
      delay(time);
    }
    if (val == pow || RC == (90 + pow))//如果速度達到要求就跳出
    {
      Serial.println("close1");
      break;
    }
    val++;//累加
  }
  while (CarValue == 2)//停
  {
    if (RC > 90)//如果是前進就減速度停止
    {
      RCF.write(90 + val);//減速度
      RCB.write(90 + val);//減速度
      Serial.print("SetValue:");
      Serial.println(RCF.read());
      delay(time);
    }
    else if (RC < 90)//如果是後退就減速度停止
    {
      RCF.write(90 - val);//減速度
      RCB.write(90 - val);//減速度
      Serial.print("SetValue:");
      Serial.println(RCF.read());
      delay(time);
    }
    RCS = RCF.read();
    if (val == 0 || RCS == 90)//如果停止達到要求就跳出
    {
      val = 0;//重置累加
      Serial.println("close2");
      break;
    }
    val--;//累加
  }
  while (CarValue == 3)//後
  {
    if (RC == 90)//如果停就加速度後退
    {
      RCF.write(90 - val);//加速度
      RCB.write(90 - val);//加速度
      Serial.print("SetValue:");
      Serial.println(RCF.read());
      delay(time);
    }
    if (val == pow || RC == (90 - pow))//如果速度達到要求就跳出
    {
      Serial.println("close3");
      break;
    }
    val++;//累加
  }
  status = 1;//更新狀態
}

void STOP()//P檔煞車動作
{
  moto();//減速停止
  RCR.write(0);//加上煞車
  RCL.write(0);
}

int Ultrasound(int trigPin, int echoPin)
{
  long duration;
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(1);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  duration = duration / 28 / 2;
  return duration;
}