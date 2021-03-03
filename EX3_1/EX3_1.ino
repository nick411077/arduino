#include <Arduino.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3);
void setup()
{
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(4, LOW);//LOW接收
  digitalWrite(5, LOW);//LOW接收
}
void loop()
{
  if (mySerial.available() > 1)
    {
        while (mySerial.available() > 0)
        {
            Serial.println(mySerial.read());
            Serial.flush();
        }
    }
}
