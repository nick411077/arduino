#include <Arduino.h>

#define PSB_SELECT      0x0001
#define PSB_L3          0x0002
#define PSB_R3          0x0004
#define PSB_START       0x0008
#define PSB_PAD_UP      0x0010
#define PSB_PAD_RIGHT   0x0020
#define PSB_PAD_DOWN    0x0040
#define PSB_PAD_LEFT    0x0080
#define PSB_L2          0x0100
#define PSB_R2          0x0200
#define PSB_L1          0x0400
#define PSB_R1          0x0800
#define PSB_GREEN       0x1000
#define PSB_RED         0x2000
#define PSB_BLUE        0x4000
#define PSB_PINK        0x8000
#define PSB_TRIANGLE    0x1000
#define PSB_CIRCLE      0x2000
#define PSB_CROSS       0x4000
#define PSB_SQUARE      0x8000

#define PSS_RX 4
#define PSS_RY 5
#define PSS_LX 6
#define PSS_LY 7

unsigned char sensorData[8] = {0xFF, 0x2D, 0, 0, 0, 0, 0, 0};
unsigned char Data[8];
uint16_t buttons;

boolean Button(uint16_t button) {
  return ((~buttons & button) > 0);
}
byte Analog(byte button) {
   return Data[button];
}

void DigitalWrite(int pinNumber, boolean status)
{
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, status);
}

void AnalogWrite(int pinNumber, int LEDChannel, int value)
{
    ledcSetup(LEDChannel, 5000, 8);
    ledcAttachPin(pinNumber, LEDChannel);
    ledcWrite(LEDChannel, value);
}

void read()
{
    if (Serial2.available())
    {
        Serial2.readBytes(Data, 8);
        int Check = Data[0] + Data[1];
        Serial.println(Check);
        if (Check == 130)
        {
            for (byte z = 0; z < sizeof(Data); z++)
            {
                Serial.print(Data[z], HEX);
            }
            Serial.println();
            buttons =  (uint16_t)(Data[4] << 8) + Data[3];
        }
        else
        {
            unsigned char CheckData[8]={0x55, 0x2D, 0xFF, 0xFF, 0x7B, 0x7B, 0x7B, 0x7B};
            for (byte z = 0; z < sizeof(CheckData); z++)
            {
                Data[z]=CheckData[z];
            }
            while (Serial2.read() >= 0){}
        }
    }
}
void write() //PTZ上
{
  for (byte z = 0; z < sizeof(sensorData); z++)
  {
    Serial2.write(sensorData[z]);
    Serial.write(sensorData[z]);
  }
}


void setup()
{
  Serial.begin(115200); // initialize serial at baudrate 9600:
  Serial2.begin(115200);
  Serial.println("OWO");

}

void loop()
{
  read();
  Moto();
}


void Moto()
{
    byte num = 0;
    byte RRX;
    byte LRX;
    byte FRY;
    byte BRY;
    if (Analog(PSS_RY) < 100){num=num+1;FRY=map(Analog(PSS_RY), 100, 0, 0, 255);}
    if (Analog(PSS_RX) > 150){num=num+2;RRX=map(Analog(PSS_RX), 150, 255, 0, 255);}
    if (Analog(PSS_RX) < 100){num=num+4;LRX=map(Analog(PSS_RX), 100, 0, 0, 255);}
    if (Analog(PSS_RY) > 150){num=num+8;BRY=map(Analog(PSS_RY), 150, 255, 0, 255);}
    switch (num)
    {
    case 1:
        Serial.println("F");
        DigitalWrite(18, HIGH);
        DigitalWrite(5, LOW);
        AnalogWrite(25, 1, FRY);
        DigitalWrite(32, HIGH);
        DigitalWrite(33, LOW);
        AnalogWrite(26, 2, FRY);
        break;
    case 2:
        Serial.println("R");
        DigitalWrite(18, HIGH);
        DigitalWrite(5, LOW);
        AnalogWrite(25, 1, RRX);
        DigitalWrite(32, LOW);
        DigitalWrite(33, HIGH);
        AnalogWrite(26, 2, RRX);
        break;
    case 3:
        Serial.println("FR");
        DigitalWrite(18, HIGH);
        DigitalWrite(5, LOW);
        AnalogWrite(25, 1, 255);
        DigitalWrite(32, HIGH);
        DigitalWrite(33, LOW);
        AnalogWrite(26, 2, 155);
        break;
    case 4:
        Serial.println("L");
        DigitalWrite(18, LOW);
        DigitalWrite(5, HIGH);
        AnalogWrite(25, 1, LRX);
        DigitalWrite(32, HIGH);
        DigitalWrite(33, LOW);
        AnalogWrite(26, 2, LRX);
        break;
    case 5:
        Serial.println("FL");
        DigitalWrite(18, HIGH);
        DigitalWrite(5, LOW);
        AnalogWrite(25, 1, 155);
        DigitalWrite(32, HIGH);
        DigitalWrite(33, LOW);
        AnalogWrite(26, 2, 255);
        break;
    case 8:
        Serial.println("B");
        DigitalWrite(18, LOW);
        DigitalWrite(5, HIGH);
        AnalogWrite(25, 1, BRY);
        DigitalWrite(32, LOW);
        DigitalWrite(33, HIGH);
        AnalogWrite(26, 2, BRY);
        break;
    case 10:
        Serial.println("BR");
        DigitalWrite(18, LOW);
        DigitalWrite(5, HIGH);
        AnalogWrite(25, 1, 255);
        DigitalWrite(32, LOW);
        DigitalWrite(33, HIGH);
        AnalogWrite(26, 2, 155);
        break;
    case 12:
        Serial.println("BL");
        DigitalWrite(18, LOW);
        DigitalWrite(5, HIGH);
        AnalogWrite(25, 1, 155);
        DigitalWrite(32, LOW);
        DigitalWrite(33, HIGH);
        AnalogWrite(26, 2, 255);
        break;
    case 0:
        Serial.println("S");
        DigitalWrite(18, HIGH);
        DigitalWrite(5, HIGH);
        AnalogWrite(25, 1, 0);
        DigitalWrite(32, HIGH);
        DigitalWrite(33, HIGH);
        AnalogWrite(26, 2, 0);
        break;
    }
}