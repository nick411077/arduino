#include <Arduino.h>
#include <SoftwareSerial.h>
int incomingByte = 0;

SoftwareSerial ZSerial(2, 3);

void setup()
{
    Serial.begin(115200);
    ZSerial.begin(115200);
}

void loop()
{
    if(Serial.available()>0)
    {
        ZSerial.write(Serial.read());
    }
}

