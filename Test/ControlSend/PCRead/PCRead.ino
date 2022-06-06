#include <Arduino.h>
#include <SoftwareSerial.h>
int incomingByte = 0;

SoftwareSerial ZSerial(2, 3);

void setup()
{
    Serial.begin(115200);
    ZSerial.begin(9600);
}

void loop()
{
    Driveread();
}

void Driveread()
{
    if(ZSerial.available()>0)
    {
        Serial.print(ZSerial.read(),HEX);
    }
}
