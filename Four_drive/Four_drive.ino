#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <FS.h>
#include <Servo.h>
#include <ESPAsyncWebServer.h>
#include <AccelStepper.h>
#include <Stepper.h>
// D7 (GPIO 13) 當 trigPin, D8 (GPIO 15) 當 echoPin 的話, 就要呼叫 ping(13, 15).
//步進設置
int pulse = 14, dir = 12, enable = 2; //Arduino給驅動器的腳位
AccelStepper stepper(1,pulse,dir);
int MaxSpeed = 2000; //空載2000
int Acceleration = 200; //空載200
int Max = 1300; //1:80= 16000轉
//PWM設置
Servo RC1;
Servo RC2;
const int RCPin1 = 5;
const int RCPin2 = 4;


//WiFi設置
const char* ssid     = "Lavender";
const char* password = "12345678";

//WebServer設置
AsyncWebServer server(80);


// Decode HTTP GET 設置
String valueString = String(90);
String CarValue = String(5);
int value;


void setup() {
  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }
  Serial.begin(115200);
  RC1.attach(RCPin1,1000,2000);  
  RC2.attach(RCPin2,1000,2000);
  RC1.write(90);
  RC2.write(90);
  pinMode(4, OUTPUT);
  pinMode(pulse, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(enable, OUTPUT);
  digitalWrite(dir,HIGH);
  digitalWrite(enable,LOW);
  stepper.setEnablePin(enable);
  stepper.disableOutputs();
  stepper.setMaxSpeed(MaxSpeed);
  stepper.setAcceleration(Acceleration);
  stepper.setCurrentPosition(650);
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
    }
    value = map(valueString.toInt(),0,180,0,Max);
    stepper.moveTo(value);
    Serial.println(stepper.currentPosition());
    switch (CarValue.toInt()){ //控制
    case 1: // 左前
      RC1.write(120);
      RC2.write(120);
      break;
    case 2: // 前
      RC1.write(90);
      RC2.write(90);
      break;
    case 3: // 右前
      RC1.write(70);
      RC2.write(70);
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
