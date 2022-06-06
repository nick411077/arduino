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

    ZSerial.write("ControlSendTest\n");
    delay(1000);
    ZSerial.write("控制板發送測試\n");
    delay(1000);
}
