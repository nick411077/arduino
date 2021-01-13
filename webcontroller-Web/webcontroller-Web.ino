/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <WiFi.h>
#include <SPIFFS.h>
#include <Servo.h>
#include <ESPAsyncWebServer.h>
#include <analogWrite.h>
#include <AccelStepper.h>
#include <DBH1.h> // DBH1.cpp 第178行 IN2A 應該是 IN1B

int pulse = 19, dir = 18, enable = 17; //Arduino給驅動器的腳位
int MaxSpeed = 20000;
int Acceleration = 2000;
int Max = 60000;
AccelStepper stepper(1,pulse,dir);
DBH1 dbh1;


Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

// GPIO the servo is attached to

// Replace with your network credentials
const char* ssid     = "Lavender";
const char* password = "12345678";
const int servoPin = 27;

// Set web server port number to 80
AsyncWebServer server(80);

// Variable to store the HTTP request


// Decode HTTP GET value
String valueString = String(90);
String BtnValue = String(5);
uint8_t resolution =13;
int value;

void DigitalWrite(int pinNumber, boolean status)
{
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, status);
}


void setup() {
  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }
  Serial.begin(115200);
  myservo.attach(servoPin,5,0,180,1000,2000);  // attaches the servo on the servoPin to the servo object
  myservo.write(valueString.toInt());
  pinMode(pulse, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(enable, OUTPUT);
  digitalWrite(dir,HIGH);
  digitalWrite(enable,LOW);
  stepper.setEnablePin(enable);
  stepper.disableOutputs();
  stepper.setMaxSpeed(MaxSpeed);
  stepper.setAcceleration(Acceleration);
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
      else if (request->argName(i) == "moto")
      {
        BtnValue = request->arg(i);
      }
    }
    value = map(valueString.toInt(),0,180,0,Max);
    Serial.println(valueString);
    Serial.println(BtnValue);
    stepper.moveTo(value);
    Serial.println(value);
    Serial.println(stepper.currentPosition());
    //myservo.write(valueString.toInt());
    switch (BtnValue.toInt()){ //控制
    case 1: // 左前
      DigitalWrite(18, HIGH);
      DigitalWrite(5, LOW);
      analogWrite(25, 225);
      DigitalWrite(17, HIGH);
      DigitalWrite(16, LOW);
      analogWrite(26, 255);
      break;
    case 2: // 前
      DigitalWrite(18, HIGH);
      DigitalWrite(5, LOW);
      analogWrite(25, 255);
      DigitalWrite(17, HIGH);
      DigitalWrite(16, LOW);
      analogWrite(26, 255);
      break;
    case 3: // 右前
      DigitalWrite(18, HIGH);
      DigitalWrite(5, LOW);
      analogWrite(25, 255);
      DigitalWrite(17, HIGH);
      DigitalWrite(16, LOW);
      analogWrite(26, 225);
      break;
    case 4: // 左
      DigitalWrite(18, LOW);
      DigitalWrite(5, HIGH);
      analogWrite(25, 255);
      DigitalWrite(17, HIGH);
      DigitalWrite(16, LOW);
      analogWrite(26, 255);
      break;
    case 5: // 停
      DigitalWrite(18, HIGH);
      DigitalWrite(5, HIGH);
      analogWrite(25, 255);
      DigitalWrite(17, HIGH);
      DigitalWrite(16, HIGH);
      analogWrite(26, 255);
      break;
    case 6: // 右
      DigitalWrite(18, HIGH);
      DigitalWrite(5, LOW);
      analogWrite(25, 255);
      DigitalWrite(17, LOW);
      DigitalWrite(16, HIGH);
      analogWrite(26, 255);
      break;
    case 7: // 左後
      DigitalWrite(18, LOW);
      DigitalWrite(5, HIGH);
      analogWrite(25, 225);
      DigitalWrite(17, LOW);
      DigitalWrite(16, HIGH);
      analogWrite(26, 255);
      break;
    case 8: // 後
      DigitalWrite(18, LOW);
      DigitalWrite(5, HIGH);
      analogWrite(25, 255);
      DigitalWrite(17, LOW);
      DigitalWrite(16, HIGH);
      analogWrite(26, 255);
      break;
    case 9: // 右後
      DigitalWrite(18, LOW);
      DigitalWrite(5, HIGH);
      analogWrite(25, 255);
      DigitalWrite(17, LOW);
      DigitalWrite(16, HIGH);
      analogWrite(26, 225);
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
