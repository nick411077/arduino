#include <WiFi.h>
#include <HTTPClient.h>

#include "PS2.h"

const char* ssid = "Rogy-418615";
const char* pass = "12345678";

const char* serverName = "http://192.168.47.1/osc/commands/execute";

unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

PS2 ps2;

void setup()
{
  Serial.begin(115200); // initialize serial at baudrate 9600:
  Serial2.begin(115200);
  WiFi.begin(ssid, pass);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");

}

void loop()
{
  ps2.read();
  Moto();
  if (ps2.ButtonPressed(PSB_CIRCLE)){post();}
}


void Moto()
{
    byte num = 0;
    byte RRX;
    byte LRX;
    byte FRY;
    byte BRY;
    if (ps2.Analog(PSS_RY) < 100){num=num+1;FRY=map(ps2.Analog(PSS_RY), 100, 0, 0, 255);}
    if (ps2.Analog(PSS_RX) > 150){num=num+2;RRX=map(ps2.Analog(PSS_RX), 150, 255, 0, 255);}
    if (ps2.Analog(PSS_RX) < 100){num=num+4;LRX=map(ps2.Analog(PSS_RX), 100, 0, 0, 255);}
    if (ps2.Analog(PSS_RY) > 150){num=num+8;BRY=map(ps2.Analog(PSS_RY), 150, 255, 0, 255);}
    switch (num)
    {
    case 1:
        ps2.DigitalWrite(18, HIGH);
        ps2.DigitalWrite(5, LOW);
        ps2.AnalogWrite(25, 1, FRY);
        ps2.DigitalWrite(32, HIGH);
        ps2.DigitalWrite(33, LOW);
        ps2.AnalogWrite(26, 2, FRY);
        break;
    case 2:
        ps2.DigitalWrite(18, HIGH);
        ps2.DigitalWrite(5, LOW);
        ps2.AnalogWrite(25, 1, RRX);
        ps2.DigitalWrite(32, LOW);
        ps2.DigitalWrite(33, HIGH);
        ps2.AnalogWrite(26, 2, RRX);
        break;
    case 3:
        ps2.DigitalWrite(18, HIGH);
        ps2.DigitalWrite(5, LOW);
        ps2.AnalogWrite(25, 1, 255);
        ps2.DigitalWrite(32, HIGH);
        ps2.DigitalWrite(33, LOW);
        ps2.AnalogWrite(26, 2, 155);
        break;
    case 4:
        ps2.DigitalWrite(18, LOW);
        ps2.DigitalWrite(5, HIGH);
        ps2.AnalogWrite(25, 1, LRX);
        ps2.DigitalWrite(32, HIGH);
        ps2.DigitalWrite(33, LOW);
        ps2.AnalogWrite(26, 2, LRX);
        break;
    case 5:
        ps2.DigitalWrite(18, HIGH);
        ps2.DigitalWrite(5, LOW);
        ps2.AnalogWrite(25, 1, 155);
        ps2.DigitalWrite(32, HIGH);
        ps2.DigitalWrite(33, LOW);
        ps2.AnalogWrite(26, 2, 255);
        break;
    case 8:
        ps2.DigitalWrite(18, LOW);
        ps2.DigitalWrite(5, HIGH);
        ps2.AnalogWrite(25, 1, BRY);
        ps2.DigitalWrite(32, LOW);
        ps2.DigitalWrite(33, HIGH);
        ps2.AnalogWrite(26, 2, BRY);
        break;
    case 10:
        ps2.DigitalWrite(18, LOW);
        ps2.DigitalWrite(5, HIGH);
        ps2.AnalogWrite(25, 1, 255);
        ps2.DigitalWrite(32, LOW);
        ps2.DigitalWrite(33, HIGH);
        ps2.AnalogWrite(26, 2, 155);
        break;
    case 12:
        ps2.DigitalWrite(18, LOW);
        ps2.DigitalWrite(5, HIGH);
        ps2.AnalogWrite(25, 1, 155);
        ps2.DigitalWrite(32, LOW);
        ps2.DigitalWrite(33, HIGH);
        ps2.AnalogWrite(26, 2, 255);
        break;
    case 0:
        ps2.DigitalWrite(18, HIGH);
        ps2.DigitalWrite(5, HIGH);
        ps2.AnalogWrite(25, 1, 0);
        ps2.DigitalWrite(32, HIGH);
        ps2.DigitalWrite(33, HIGH);
        ps2.AnalogWrite(26, 2, 0);
        break;
    }
}

void post()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        // Your Domain name with URL path or IP address with path
        http.begin(serverName);

        // Specify content-type header
        http.addHeader("Content-Type", "application/json");
        // Data to send with HTTP POST
         String httpRequestData = "{\"name\":\"camera.startCapture\",\"parameters\":\"image\"}";
        // Send HTTP POST request
        int httpResponseCode = http.POST(httpRequestData);
        //{"name":"camera.startCapture","parameters":"image"}'

        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);

        // Free resources
        http.end();
    }
    else
    {
        Serial.println("WiFi Disconnected");
    }
}