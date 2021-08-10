#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>       //異步處理函數庫 download https://github.com/me-no-dev/ESPAsyncTCP
#include <FS.h>                //檔案系統函數庫  
#include <Servo.h>             //伺服馬達函數庫
#include <ESPAsyncWebServer.h> //異步處理網頁伺服器函數庫 download https://github.com/me-no-dev/ESPAsyncWebServer
#include <AccelStepper.h>      //步進馬達函數庫
#include <Stepper.h>           //步進馬達函數庫

//步進設置
#define ENB 2
#define DIR 12
#define PUL 14 //Arduino給驅動器的腳位
AccelStepper stepper(1, PUL, DIR);
int MaxSpeed = 2000;    //最高速 空載2000
int Acceleration = 200; //加速度 空載200
int Max = 1300;         //1:80= 16000轉
//PWM設置
Servo RC1;
Servo RC2;
const int RCPin1 = 5;
const int RCPin2 = 4;

//超音波設置
#define TRIG 15
#define ECHO 13

//WiFi設置
const char *ssid = "Lavender";
const char *password = "12345678";

//WebServer設置
AsyncWebServer server(80);

// Decode HTTP GET 設置
String Slider = String(90); //網站請求的拉條變數
String Car = String(2); //網站請求的方向變數
String Set = String(45); //網站請求的出力變數
//將 String轉換成int
int SliderValue; 
int CarValue;
int SetValue;

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
  RC1.attach(RCPin1, 1000, 2000);
  RC2.attach(RCPin2, 1000, 2000);
  RC1.write(90);
  RC2.write(90);
  pinMode(PUL, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(ENB, OUTPUT);
  digitalWrite(DIR, HIGH);
  digitalWrite(ENB, LOW);
  stepper.setEnablePin(ENB);
  stepper.disableOutputs();
  stepper.setMaxSpeed(MaxSpeed);         //最高速設置
  stepper.setAcceleration(Acceleration); //加速度設置
  stepper.setCurrentPosition(650);       //步進馬達置中設置
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
      if (request->argName(i) == "value")//GET網站拉條狀態
      {
        Slider = request->arg(i);
      }
      else if (request->argName(i) == "car")//GET網站方向狀態
      {
        Car = request->arg(i);
        status = 0;//更新狀態
      }
      else if (request->argName(i) == "set")//GET網站出力狀態
      {
        Set = request->arg(i);
      }
    }
    SliderValue = map(Slider.toInt(), 0, 180, 0, Max);//換算數值
    stepper.moveTo(SliderValue);//指定步進馬達位址
    Serial.println(stepper.currentPosition());
    SetValue = Set.toInt();//將 String轉換成int
    CarValue = Car.toInt();//將 String轉換成int
    /*switch (CarValue.toInt()){ //控制
    case 1: // 左
      run(1,set);
      break;
    case 2: // 停
      RC1.write(90);
      RC2.write(90);
      break;
    case 3: // 後
      run(2,set); //
      break;
    }*/
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
  stepper.run();
  if (status == 0)//讀取狀態
  {
    moto();
  }
  Serial.print("Distance in CM: ");
  Serial.println(Ultrasound(TRIG,ECHO));
}

void moto()
{
  int set = SetValue;//加速度值
  int RC = RC1.read();//初始讀取馬達狀態
  int RCS = RC1.read();//停止中時持續讀取馬達狀態
  int time = 10;//delay時間
  while (CarValue == 1)//前
  {
    if (RC == 90)//如果停就加速度前進
    {
      RC1.write(90 + val);//加速度
      RC2.write(90 + val);//加速度
      Serial.print("SetValue:");
      Serial.println(RC1.read());
      delay(time);
    }
    if (val == set || RC == (90 + set))//如果速度達到要求就跳出
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
      RC1.write(90 + val);//減速度
      RC2.write(90 + val);//減速度
      Serial.print("SetValue:");
      Serial.println(RC1.read());
      delay(time);
    }
    else if (RC < 90)//如果是後退就減速度停止
    {
      RC1.write(90 - val);//減速度
      RC2.write(90 - val);//減速度
      Serial.print("SetValue:");
      Serial.println(RC1.read());
      delay(time);
    }
    RCS = RC1.read();
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
      RC1.write(90 - val);//加速度
      RC2.write(90 - val);//加速度
      Serial.print("SetValue:");
      Serial.println(RC1.read());
      delay(time);
    }
    if (val == set || RC == (90 - set))//如果速度達到要求就跳出
    {
      Serial.println("close3");
      break;
    }
    val++;//累加
  }
  status = 1;//更新狀態
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