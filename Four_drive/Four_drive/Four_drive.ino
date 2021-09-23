#include <WiFi.h>
#include <AsyncTCP.h>       //異步處理函數庫 download https://github.com/me-no-dev/AsyncTCP
#include <SPIFFS.h>                //檔案系統函數庫  
#include <Servo.h>             //伺服馬達函數庫 https://github.com/RoboticsBrno/ServoESP32
#include <ESPAsyncWebServer.h> //異步處理網頁伺服器函數庫 download https://github.com/me-no-dev/ESPAsyncWebServer
#include <AccelStepper.h>      //步進馬達函數庫
#include <Stepper.h>           //步進馬達函數庫

//#define DEBUG //除錯測試模式

//步進設置
#define ENB 5
#define DIR 18
#define PUL 19 //Arduino給驅動器的腳位
AccelStepper stepper(1, PUL, DIR);
int MaxSpeed = 2000;    //最高速 空載2000
int Acceleration = 200; //加速度 空載200
//int Max = 1300;         //1:80= 16000轉

//Servo設置
#define RCFPin 32
#define RCBPin 33
#define RCLPin 25
#define RCRPin 26
Servo RCF;
Servo RCB;
Servo RCL;
Servo RCR;

//雙核運行
TaskHandle_t Task1;

//超音波設置
#define TRIG1 22
#define ECHO1 23
#define TRIG2 16
#define ECHO2 17

int counts;
int counts_run = 20;//超音波確認次數
int Distance = 40;
uint8_t UCstatus = 1; //為了loop不要重複運行設定狀態變數只運行一次

//碰撞感應器
#define SL 36
#define SR 39
char released[2] = {0,0}; //左右開關狀態
uint8_t Stepstatus = 1; //為了loop不要重複運行設定狀態變數只運行一次

//WiFi設置
const char *ssid = "Lavender";
const char *password = "12345678";

//WebServer設置
AsyncWebServer server(80);

// Decode HTTP GET 設置
String Ste = String(5); //網站請求的方向變數
String Car = String(2); //網站請求的方向變數
String Pow = String(30); //網站請求的出力變數
String Stop = String(0);
//將 String轉換成int 
int StepValue;
int CarValue;
int PowValue;
int StopValue;

//直流馬達加速度配置
int val = 90; //加速度變數
int saveval = 90; //加速度變數
int powertime = 20;//delay時間
uint8_t status = 0; //為了loop不要重複運行設定狀態變數只運行一次

void setup()
{
  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  Serial.begin(115200); //設定鮑率
  RCF.attach(RCFPin,1,0,180,1000,2000);
  RCB.attach(RCBPin,2,0,180,1000,2000);
  RCL.attach(RCLPin,3);
  RCR.attach(RCRPin,4);
  RCF.write(90);
  RCB.write(90);
  pinMode(PUL, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(SL, INPUT);
  pinMode(SR, INPUT);
  digitalWrite(DIR, HIGH);
  digitalWrite(ENB, LOW);
  stepper.setEnablePin(ENB);
  stepper.disableOutputs();
  stepper.setMaxSpeed(MaxSpeed);
  stepper.setAcceleration(Acceleration);
  stepper.setCurrentPosition(0);
  xTaskCreatePinnedToCore(//雙核運行參數
             Task1code, /* Task function. */
             "Task1",   /* name of task. */
             10000,     /* Stack size of task */
             NULL,      /* parameter of the task */
             1,         /* priority of the task */
             &Task1,    /* Task handle to keep track of created task */
             0);        /* pin task to core 0 */
  StepValue = Ste.toInt();//將 String轉換成int
  CarValue = Car.toInt();//將 String轉換成int
  PowValue = Pow.toInt();//將 String轉換成int
  StopValue = Stop.toInt();//將 String轉換成int
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
      Serial.print("Param name: ");//列出GET名稱
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
      else if (request->argName(i) == "car")//GET網站方向狀態
      {
        Car = request->arg(i);
        CarValue = Car.toInt();//將 String轉換成int
        status = 1;//更新狀態
        RCR.write(10);//釋放煞車
        RCL.write(10);//釋放煞車
        StopValue = 0;
        if (UCstatus == 0 && CarValue != 3)
        {
          status = 0;
        }
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
  counts++;
  if (ButtonPressed(SL, 0) == 1 || ButtonPressed(SR, 1) == 1) //如果左或右碰到微動開關離即停止
  {
    if (StepValue == 1)
    {
      Step(2);
      StepValue = 0;
    }
  }
  else
  {
    StepValue = 1;
  }
#ifdef DEBUG
  Serial.print("計數：");
  Serial.println(counts);
  Serial.print("超音波1：");
  Serial.println(Ultrasound(TRIG1,ECHO1));
  Serial.print("超音波2：");
  Serial.println(Ultrasound(TRIG2,ECHO2));
  
  for (size_t i = 0; i < sizeof(released); i++)
  {
    Serial.print(released[i], HEX);
  }
  Serial.println();
  #endif
  if (status == 1)//讀取狀態
  {
    moto(CarValue, PowValue);
  }
  if (StopValue == 1)//如果接收P檔
  {
    STOP();
  }
  if (Ultrasound(TRIG1,ECHO1) <= Distance) //超音波小於40公分停止
  {
    if (counts == counts_run)//達到確認次數及停止
    {
      Serial.println("counts" + String(counts_run) + "run done");
      if (UCstatus == 1)//判別使否有停止過了
      {
        Serial.println("UC STOP");
        STOP();
        UCstatus = 0;
      }
    }
  }
  else //沒有達到40公分內重新計數
  {
    #ifdef DEBUG
    Serial.println("counts restart");
    #endif
    counts = 0;
    UCstatus = 1;
  }
}

void moto(int Value, int Power) //直流馬達加速度
{
  while (Value == 1)//前進
  {
    if (saveval<=(90+Power))//如果後退或停止的話進行減到前進
    {
      RCF.write(val);
      RCB.write(val);
      #ifdef DEBUG
      Serial.print("SetValue:");
      Serial.println(RCF.read());
      Serial.println(RCB.read());
      Serial.println(val);
      #endif
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
  while (Value == 2)//停止
  {
    if (val == 90)
    {
      saveval=val;
      Serial.println("close2");
      break;
    }
    if (saveval<90)//如果狀態後退加到停止
    {
      RCF.write(val+5);
      RCB.write(val+5);
      #ifdef DEBUG
      Serial.print("SetValue:");
      Serial.println(RCF.read());
      Serial.println(RCB.read());
      Serial.println(val);
      #endif
      delay(powertime);
      val ++;
    }
    if (saveval>90)//如果狀態前進減到停止
    {
      RCF.write(val);
      RCB.write(val);
      #ifdef DEBUG
      Serial.print("SetValue:");
      Serial.println(RCF.read());
      Serial.println(RCB.read());
      Serial.println(val);
      #endif
      delay(powertime);
      val --;
    }
  }
  while (Value == 3)//後退
  {
    if (saveval>=(90 - Power))//如果前進或停止的話進行減到後退
    {
      RCF.write(val+5);
      RCB.write(val+5);
      #ifdef DEBUG
      Serial.print("SetValue:");
      Serial.println(RCF.read());
      Serial.println(RCB.read());
      Serial.println(val);
      #endif
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
  status = 0;//更新狀態
}

void Step(int Step)
{
  switch (Step)
  {
  case 1://左轉
    if (ButtonPressed(SL, 0) == 0 )
    {
      stepper.setSpeed(200);//設定速度
      Serial.println("Step1");
    }
    
    break;
  case 2://停
    stepper.setSpeed(0);
    Serial.println("Step2");
    break;
  case 3://右轉
    if (ButtonPressed(SR, 1) == 0 )
    {
      stepper.setSpeed(-200);
      Serial.println("Step3");
    }
    
    break;
  }
}

void STOP()//P檔煞車動作
{
  Serial.println("STOP run");
  moto(2,90);
  RCR.write(150);//加上煞車
  RCL.write(150);
  Serial.println(RCR.read());
  Serial.println(RCL.read());
  StopValue = 0;
}

int Ultrasound(int trigPin, int echoPin)//超音波
{
  long duration;
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  duration = duration / 28 / 2;
  if (duration > 80) return 100;
  return duration;
}

boolean ButtonPressed(uint8_t pin, int numder)//微動開關按下反應
{
  int i = 0;
  if (digitalRead(pin) == HIGH)
  {
    i = 1;
    if (released[numder] < 1)
    {
      released[numder] = 1;
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    i = 0;
    released[numder] = 0;
    return false;
  }
}

void Task1code(void *pvParameters)//雙核運行
{
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;)
  {
    stepper.runSpeed();//步進馬達運行防止loop進入迴圈導致停止運行
    delay(1);
  }
}