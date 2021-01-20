/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <WiFi.h>
#include <SPIFFS.h>
#include <Servo.h>
#include <ESPAsyncWebServer.h>
#include <AccelStepper.h>

int pulse = 19, dir = 18, enable = 5; //Arduino給驅動器的腳位
int light1=35, light2=34, light3=39;
int ReMaxSpeed = 20000;
int ReAcceleration = 2000;
int MaxSpeed = 40000;
int Acceleration = 4000;
int Max = 60000;
AccelStepper stepper(1,pulse,dir);

Servo RCmoto;
Servo RC1;
Servo RC2;  // create servo object to control a servo
// twelve servo objects can be created on most boards

// GPIO the servo is attached to

// Replace with your network credentials
const char* ssid     = "Lavender";
const char* password = "12345678";
const int RCPinmoto = 27;
const int RCPin1 = 26;
const int RCPin2 = 25;

// Set web server port number to 80
AsyncWebServer server(80);

// Variable to store the HTTP request


// Decode HTTP GET value
String valueString = String(90);
String CarValue = String(5);
String MotoValue = String(2);
uint8_t resolution =13;
int value;

void DigitalWrite(int pinNumber, boolean status)
{
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, status);
}

void LightRead(int pinNumber)
{
  pinMode(pinNumber,INPUT);
  digitalRead(pinNumber);
}

void setup() {
  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }
  Serial.begin(115200);
  RCmoto.attach(RCPinmoto,5,0,180,1000,2000);
  //RC1.attach(RCPin1,5,0,180,1000,2000);  // attaches the servo on the servoPin to the servo object
  //RC2.attach(RCPin2,5,0,180,1000,2000);
  //RC1.write(90);
  //RC2.write(90);
  pinMode(pulse, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(enable, OUTPUT);
  pinMode(light1,INPUT);
  pinMode(light2,INPUT);
  pinMode(light3,INPUT);
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
    }
    value = map(valueString.toInt(),0,180,0,Max);
    Serial.println(valueString);
    Serial.println(CarValue);
    stepper.moveTo(value);
    Serial.println(value);
    Serial.println(stepper.currentPosition());
    //RC1.write(valueString.toInt());
    switch (MotoValue.toInt())
    {
    case 1:
      RCmoto.write(0);
      break;
    case 2:
      RCmoto.write(90);
      break;
    case 3:
      RCmoto.write(180);
      break;
    
    }
    switch (CarValue.toInt()){ //控制
    case 1: // 左前
      RC1.write(135);
      RC2.write(180);
      break;
    case 2: // 前
      RC1.write(180);
      RC2.write(180);
      break;
    case 3: // 右前
      RC1.write(180);
      RC2.write(135);
      break;
    case 4: // 左
      RC1.write(0);
      RC2.write(180);
      break;
    case 5: // 停
      RC1.write(90);
      RC2.write(90);
      break;
    case 6: // 右
      RC1.write(180);
      RC2.write(0);
      break;
    case 7: // 左後
      RC1.write(0);
      RC2.write(45);
      break;
    case 8: // 後
      RC1.write(0);
      RC2.write(0);
    case 9: // 右後
      RC1.write(45);
      RC2.write(0);
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
  Serial.println(digitalRead(light1));
}
