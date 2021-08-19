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

int val = 90; //加速度變數
int saveval = 90; //加速度變數
int powertime = 50;//delay時間
uint8_t status = 1; //為了loop不要重複運行設定狀態變數只運行一次

void setup()
{
  Serial.begin(115200); //設定鮑率
  RCF.attach(RCFPin,5,0,180,1000,2000);
  RCB.attach(RCBPin,6,0,180,1000,2000);
  RCF.write(90);
  RCB.write(90);
  RCL.attach(RCLPin);
  RCR.attach(RCRPin);
  // Connect to Wi-Fi network with SSID and password
}

void loop()
{
  RCF.write(135);
}

void STOP()//P檔煞車動作
{
  Test(2,90);//減速停止
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
void Test(int Value, int Power)
{
  while (Value == 1)
  {
    if (saveval<=90)
    {
      RCF.write(val);
      RCF.write(val);
      Serial.print("SetValue:");
      Serial.println(RCF.read());
      Serial.println(val);
      delay(powertime);
    }
    if (val == (90+Power))
    {
      saveval=val;
      Serial.println("close1");
      break;
    }
    val++;
  }
  while (Value == 2)
  {
    if (val == 90)
    {
      saveval=val;
      Serial.println("close2");
      break;
    }
    if (saveval<90)
    {
      RCF.write(val);
      RCF.write(val);
      Serial.print("SetValue:");
      Serial.println(RCF.read());
      Serial.println(val);
      delay(powertime);
      val ++;
    }
    if (saveval>90)
    {
      RCF.write(val);
      RCF.write(val);
      Serial.print("SetValue:");
      Serial.println(RCF.read());
      Serial.println(val);
      delay(powertime);
      val --;
    }
  }
  while (Value == 3)
  {
    if (saveval>=90)
    {
      RCF.write(val);
      RCF.write(val);
      Serial.print("SetValue:");
      Serial.println(RCF.read());
      Serial.println(val);
      delay(powertime);
    }
    if (val == (90 - Power))
    {
      saveval=val;
      Serial.println("close3");
      break;
    }
    val --;
  }
  status = 1;//更新狀態
}