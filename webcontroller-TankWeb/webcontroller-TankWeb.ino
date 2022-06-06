
#include <WiFi.h>
#include <SPIFFS.h>
#include <Servo.h>
#include <ESPAsyncWebServer.h>
#include <AccelStepper.h>
#include <Stepper.h>

//雲台設置
byte UpV = 0x30, DownV = 0x30, LeftV = 0x30, RightV = 0x30; //數值從$00(停止)到$3F(高速)

//byte UpCMD[7]={0xFF,0x01,0x00,0x08,0x00,0x38,0x41};
byte UpCMD[7] = {0xFF, 0x01, 0x00, 0x08, 0x00, UpV, 0x09 + UpV};
byte UpStopCMD[7] = {0xFF, 0x01, 0x00, 0x08, 0x00, 0x00, 0x09};

//byte DownCMD[7]={0xFF,0x01,0x00,0x10,0x00,0x38,0x49};
byte DownCMD[7] = {0xFF, 0x01, 0x00, 0x10, 0x00, DownV, 0x11 + DownV};
byte DownStopCMD[7] = {0xFF, 0x01, 0x00, 0x10, 0x00, 0x00, 0x11};

//byte LeftCMD[7]={0xFF,0x01,0x00,0x04,0x18,0x00,0x1D};
byte LeftCMD[7] = {0xFF, 0x01, 0x00, 0x04, LeftV, 0x00, 0x05 + LeftV};
byte LeftStopCMD[7] = {0xFF, 0x01, 0x00, 0x04, 0x00, 0x00, 0x05};

//byte RightCMD[7]={0xFF,0x01,0x00,0x02,0x18,0x00,0x1B};
byte RightCMD[7] = {0xFF, 0x01, 0x00, 0x02, RightV, 0x00, 0x03 + RightV};
byte RightStopCMD[7] = {0xFF, 0x01, 0x00, 0x02, 0x00, 0x00, 0x03};
//光遮設置
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
int value;

void check()//進行步進初始位址
{
  if (digitalRead(light1) == 0)//如果下面光遮有遮到
  {
    while (digitalRead(light1) == 0)//就往上慢慢走
    {
      stepper.setSpeed(-2500);
      stepper.runSpeed();
    }
    restart();
  }
  else
  {
    while (digitalRead(light1) == 1)//沒有的話就往下走初始化
    {
      stepper.setSpeed(10000);
      stepper.runSpeed();
    }
    restart();
  }
}

void restart()//計算里程
{
  if (digitalRead(light1) == 1)//如果確定沒有遮到就計算里程
  {
    Serial.println("2");
    stepper.setCurrentPosition(0);
    stepper.setSpeed(-10000);
    while (digitalRead(light2) == 1)//碰到上面光遮就跳出迴圈
    {
      stepper.runSpeed();
    }
    Serial.print("set");
    Serial.println(stepper.currentPosition());
    Max = stepper.currentPosition();
    stepper.setMaxSpeed(MaxSpeed);
    stepper.setAcceleration(Acceleration);
  }
  else
  {
    check();
  }
}

void setup() {
  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }
  Serial.begin(115200);
  Serial2.begin(9600);
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
  check();
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_AP); //WIFI模式
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    int args = request->args();
    Serial.println(args);
    for (int i = 0; i < args; i++){
      Serial.print("Param name: ");
      Serial.println(request->argName(i));
      Serial.print("Param value: ");
      Serial.println(request->arg(i));
      Serial.println("------");
      if (request->argName(i) == "value")
      {
        valueString = request->arg(i);
      }
      else if (request->argName(i) == "car")
      {
        CarValue = request->arg(i);
      }
      else if (request->argName(i) == "moto")
      {
        MotoValue = request->arg(i);
      }
      else if (request->argName(i) == "Yuntai")
      {
        YuntaiValue = request->arg(i);
      }
    }
    value = map(valueString.toInt(),0,180,0,Max);
    stepper.moveTo(value);
    Serial.println(stepper.currentPosition());
    Serial2.println(YuntaiValue.toInt());
    switch (MotoValue.toInt())
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
    switch (CarValue.toInt()){ //控制
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
    switch (YuntaiValue.toInt())
    {       //控制
    case 1: // 前
      Up();
      break;
    case 2: // 左
      Left();
      break;
    case 3: // 停
      LeftStop();
      DownStop();
      break;
    case 4: // 右
      Right();
      break;
    case 5: // 後
      Down();
      break;
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

void loop()
{
  stepper.run();
}
void Up() //PTZ上
{
  digitalWrite(4, HIGH);
  delay(20);
  for (byte z = 0; z < sizeof(UpCMD); z++)
  {
    Serial2.write(UpCMD[z]);
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  delay(20);
}

void UpStop() //PTZ上
{
  digitalWrite(4, HIGH);
  delay(20);
  for (byte z = 0; z < sizeof(UpStopCMD); z++)
  {
    Serial2.write(UpStopCMD[z]);
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  delay(20);
}

void Down() //PTZ下
{
  digitalWrite(4, HIGH);
  delay(20);
  for (byte z = 0; z < sizeof(DownCMD); z++)
  {
    Serial2.write(DownCMD[z]);
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  delay(20);
}

void DownStop() //PTZ下
{
  digitalWrite(4, HIGH);
  delay(20);
  for (byte z = 0; z < sizeof(DownStopCMD); z++)
  {
    Serial2.write(DownStopCMD[z]);
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  delay(20);
}

void Left() //PTZ左
{
  digitalWrite(4, HIGH);
  delay(20);
  for (byte z = 0; z < sizeof(LeftCMD); z++)
  {
    Serial2.write(LeftCMD[z]);
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  delay(20);
}

void LeftStop() //PTZ左
{
  digitalWrite(4, HIGH);
  delay(20);
  for (byte z = 0; z < sizeof(LeftStopCMD); z++)
  {
    Serial2.write(LeftStopCMD[z]);
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  delay(20);
}

void Right() //PTZ右
{
  digitalWrite(4, HIGH);
  delay(20);
  for (byte z = 0; z < sizeof(RightCMD); z++)
  {
    Serial2.write(RightCMD[z]);
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  delay(20);
}

void RightStop() //PTZ右
{
  digitalWrite(4, HIGH);
  delay(20);
  for (byte z = 0; z < sizeof(RightStopCMD); z++)
  {
    Serial2.write(RightStopCMD[z]);
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  delay(20);
}
