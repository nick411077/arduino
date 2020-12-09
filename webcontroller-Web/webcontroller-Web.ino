/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <WiFi.h>
#include <SPIFFS.h>
#include <Servo.h>
#include <ESPAsyncWebServer.h>
#include <analogWrite.h>



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
  myservo.attach(servoPin,5,0,180,500,2400);  // attaches the servo on the servoPin to the servo object
  myservo.write(valueString.toInt());
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
    Serial.println(valueString);
    Serial.println(BtnValue);
    myservo.write(valueString.toInt());
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
}
