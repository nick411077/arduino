
#include <WiFi.h>
#include <SPIFFS.h>
#include <Servo.h>
#include <ESPAsyncWebServer.h>
#include <AccelStepper.h>
#include <Stepper.h>
#include <HardwareSerial.h>

//雙核運行
TaskHandle_t Task1;

//雲台設置
const PROGMEM byte UpV = 0x30, DownV = 0x30, LeftV = 0x30, RightV = 0x30; //數值從$00(停止)到$3F(高速)

//byte UpCMD[7]={0xFF,0x01,0x00,0x08,0x00,0x38,0x41};
const PROGMEM byte UpCMD[7] = {0xFF, 0x01, 0x00, 0x08, 0x00, UpV, 0x09 + UpV};
const PROGMEM byte UpStopCMD[7] = {0xFF, 0x01, 0x00, 0x08, 0x00, 0x00, 0x09};

//byte DownCMD[7]={0xFF,0x01,0x00,0x10,0x00,0x38,0x49};
const PROGMEM byte DownCMD[7] = {0xFF, 0x01, 0x00, 0x10, 0x00, DownV, 0x11 + DownV};
const PROGMEM byte DownStopCMD[7] = {0xFF, 0x01, 0x00, 0x10, 0x00, 0x00, 0x11};

//byte LeftCMD[7]={0xFF,0x01,0x00,0x04,0x18,0x00,0x1D};
const PROGMEM byte LeftCMD[7] = {0xFF, 0x01, 0x00, 0x04, LeftV, 0x00, 0x05 + LeftV};
const PROGMEM byte LeftStopCMD[7] = {0xFF, 0x01, 0x00, 0x04, 0x00, 0x00, 0x05};

//byte RightCMD[7]={0xFF,0x01,0x00,0x02,0x18,0x00,0x1B};
const PROGMEM byte RightCMD[7] = {0xFF, 0x01, 0x00, 0x02, RightV, 0x00, 0x03 + RightV};
const PROGMEM byte RightStopCMD[7] = {0xFF, 0x01, 0x00, 0x02, 0x00, 0x00, 0x03};



//光遮設置
char released[2] = {0,0}; //光遮的狀態
int light1=35, light2=34;
//步進設置
int pulse = 19, dir = 18, enable = 5; //Arduino給驅動器的腳位
AccelStepper stepper(1,pulse,dir);
int ReMaxSpeed = 20000;
int ReAcceleration = 2000;
int MaxSpeed = 20000;
int Acceleration = 5000;
int Max = -200000;
//PWM設置
Servo RCmoto;
Servo RC1;
Servo RC2;
const int RCPinmoto = 27;
const int RCPin1 = 26;
const int RCPin2 = 25; 
//ZigBee設置

byte SWITCH[3] = {0x22, 0x02, 0x00};


//WiFi設置
const char* ssid     = "Lavender";
const char* password = "12345678";

//WebServer設置
AsyncWebServer server(80);


// Decode HTTP GET 設置
String valueString = String(0);
String CarValue = String(5);
String MotoValue = String(2);
String YuntaiValue = String(5);
String ModeValue = String(0);
int value;
int yuntai = 5;
boolean mode = 0;

uint8_t status = 0; //為了loop不要重複運行設定狀態變數只運行一次

void setup() {
  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }
  Serial.begin(115200);
  Serial1.begin(115200,SERIAL_8N1,33,32);
  Serial2.begin(9600);
  xTaskCreatePinnedToCore(Task1code,"Task1",10000,NULL,1,&Task1,0);
  RCmoto.attach(RCPinmoto,4,0,180,500,2400);
  RC1.attach(RCPin1,5,0,180,1000,2000);  // attaches the servo on the servoPin to the servo object
  RC2.attach(RCPin2,6,0,180,1000,2000);
  RC1.write(90);
  RC2.write(90);
  pinMode(4, OUTPUT);
  pinMode(pulse, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(enable, OUTPUT);
  pinMode(light1,INPUT);
  pinMode(light2,INPUT);
  digitalWrite(dir,HIGH);
  digitalWrite(enable,LOW);
  stepper.setEnablePin(enable);
  stepper.disableOutputs();
  stepper.setMaxSpeed(ReMaxSpeed);
  stepper.setAcceleration(ReAcceleration);
  stepper.setCurrentPosition(0);
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_AP); //WIFI模式
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    int args = request->args();
    Serial.println(args);
    for (int i = 0; i < args; i++)
    {
      Serial.print("Param name: ");
      Serial.println(request->argName(i));
      Serial.print("Param value: ");
      Serial.println(request->arg(i));
      Serial.println("------");
      if (request->argName(i) == "value")
      {
        status = 1;
        valueString = request->arg(i);
      }
      else if (request->argName(i) == "car")
      {
        status = 1;
        CarValue = request->arg(i);
      }
      else if (request->argName(i) == "moto")
      {
        status = 1;
        MotoValue = request->arg(i);
      }
      else if (request->argName(i) == "Yuntai")
      {
        status = 1;
        YuntaiValue = request->arg(i);
      }
      else if (request->argName(i) == "mode") // GET網站方向狀態
      {
        status = 1;
        ModeValue = request->arg(i);
        mode = ModeValue.toInt();//將 String轉換成int 
      }
    }
    request->send(SPIFFS, "/index.html", "text/html");
  });
  //server.onRequestBody([](AsyncWebServerRequest *request));
  server.serveStatic("/", SPIFFS, "/");

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop()
{
  if (ButtonPressed(light1, 0) == 1 || ButtonPressed(light2, 1) == 1)
  {
    stepper.setSpeed(0);
  }
  else
  {
    switch (valueString.toInt())
    {
    case 1:
      if (digitalRead(light1) == 1)
      {
        stepper.setSpeed(5000);
      }
      break;
    case 2:
      stepper.setSpeed(0);
      break;
    case 3:
      if (digitalRead(light2) == 1)
      {
        stepper.setSpeed(-5000);
      }
      break;
    }
  }
  stepper.runSpeed();
}

void Task1code(void *pvParameters)//雙核運行
{
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;)
  {
    if (status == 1) //讀取狀態
    {
      ZigBee();
      Car(CarValue.toInt());
      YuntaiMode(YuntaiValue.toInt());
      Moto(MotoValue.toInt());
      status = 0;
    }
    delay(1);
  }
}

void ZigBee()
{
  if (mode)
  {
    Serial.println("ON");
    SWITCH[2] = 0x01;
  }
  else
  {
    Serial.println("OFF");
    SWITCH[2] = 0x00;
  }
  Serial1.write(SWITCH,3);
  for (byte z = 0; z < sizeof(SWITCH); z++)
  {
    Serial.print(SWITCH[z],HEX);
    Serial.print(",");
  }
  Serial.println();
}


void Yuntai(const byte CMD[])
{
  digitalWrite(4, HIGH);
  delay(20);
  for (byte z = 0; z < 7; z++)
  {
    Serial2.write(CMD[z]);
    Serial.print(CMD[z],HEX);
    Serial.print(",");
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  delay(20);
}

void Car(int car)
{
  switch (car){ //控制
    case 1: // 左前
      RC1.write(120);
      RC2.write(135);
      break;
    case 2: // 前
      RC1.write(135);
      RC2.write(135);
      break;
    case 3: // 右前
      RC1.write(135);
      RC2.write(120);
      break;
    case 4: // 左
      RC1.write(45);
      RC2.write(135);
      break;
    case 5: // 停
      RC1.write(90);
      RC2.write(90);
      break;
    case 6: // 右
      RC1.write(135);
      RC2.write(45);
      break;
    case 7: // 左後
      RC1.write(45);
      RC2.write(30);
      break;
    case 8: // 後
      RC1.write(45);
      RC2.write(45);
    case 9: // 右後
      RC1.write(30);
      RC2.write(45);
      break;
    }
}

void YuntaiMode(int mode)
{
  switch (mode)
    {       //控制
    case 1: // 前
      Yuntai(UpCMD);
      break;
    case 2: // 左
      Yuntai(LeftCMD);
      break;
    case 3: // 停
      Yuntai(LeftStopCMD);
      Yuntai(DownStopCMD);
      break;
    case 4: // 右
      Yuntai(RightCMD);
      break;
    case 5: // 後
      Yuntai(DownCMD);
      break;
    }
}

void Moto(int moto)
{
  switch (moto)
    {
    case 1:
      RCmoto.write(180);
      break;
    case 2:
      RCmoto.write(90);
      break;
    case 3:
      RCmoto.write(0);
      break;
    }
}

boolean ButtonPressed(uint8_t pin, int numder)//微動開關按下反應
{
  int i = 0;
  if (digitalRead(pin) == LOW)
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