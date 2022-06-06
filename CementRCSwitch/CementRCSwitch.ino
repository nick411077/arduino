#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Servo.h>

String SWITCH;
SoftwareSerial ZSerial(3, 2);

Servo RCmoto;

const int RCPinmoto = 5;

byte ReadData[3];
byte CheckData[3];
boolean mode = 0;

void setup()
{
    Serial.begin(115200);
    ZSerial.begin(115200);
    RCmoto.attach(RCPinmoto,500,2500);
    while (ZSerial.read()>0){}
    Serial.println("start");
}

void loop()
{
    ESP32Read();
    Relay();
}

void ESP32Read()
{
    if (ZSerial.available())
    {
            Serial.println("done2");
            ZSerial.readBytes(ReadData,3);
            mode = ReadData[2];
            for (byte i = 0; i < sizeof(ReadData); i++)
            {
                Serial.print(ReadData[i], HEX);
                Serial.print(",");
            }
            Serial.println();

    }
}

void Relay()
{
    if (mode)
    {
        RCmoto.write(90);
    }
    else
    {
        RCmoto.write(110);
    }
}