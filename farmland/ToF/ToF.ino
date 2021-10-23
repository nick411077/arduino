#include <Arduino.h>



byte x[7];
byte Data[8] = {0x50,0x03,0x00,0x34,0x00,0x01,0xC8,0x45};
unsigned int Long;

void setup()
{
    Serial.begin(115200); //
    Serial2.begin(115200);
    Serial3.begin(115200);
}

void loop()
{
    ToF();
    delay(100);
}


void ToF()
{
    Serial2.write(Data,sizeof(Data));
    delay(20);
    ToFValue();
}
void ToFValue()
{
    if (Serial2.available() > 0)
    {
        Serial2.readBytes(x, 7);
        int Check = x[0] + x[1] + x[2];
        if (Check == 0x55)
        {
            for (byte i = 0; i < sizeof(x); i++)
            {
                Serial.print(x[i], HEX);
                Serial.print(",");
            }
            Long =  (uint16_t)(x[3] << 8) + x[4];
            Serial.println();
            Serial.print("distance:");
            Serial.println(Long);
            
        }
        else
        {
            while (Serial2.read() >= 0)
            {
            }
        }
    }
}