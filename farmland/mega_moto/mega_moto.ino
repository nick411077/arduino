#include <Arduino.h>
#include <L298NX2.h>

//Servo設置
#define RCLPin 25
#define RCRPin 26

#define ENL1 4
#define ENL2 5

#define INL1A 30
#define INL1B 31
#define INL2A 32
#define INL2B 33

#define ENR1 6
#define ENR2 7

#define INR1A 34
#define INR1B 35
#define INR2A 36
#define INR2B 37

L298NX2 MotorsF(ENL1,INL1A,INL1B,ENR1,INR1A,INR1B);
L298NX2 MotorsB(ENL2,INL2A,INL2B,ENR2,INR2A,INR2B);


byte DIR = 5;
byte PWM;
boolean mode;

byte ADIR = 0;
//收發ESP32資料
byte WriteData[8] = {0x50,0x03,0x00,0x00,0x00,0x00,0x00,0x00};

byte ReadData[8];

void setup()
{
    Serial.begin(115200); //
    Serial1.begin(115200);
}

void motoAuto(byte dir, byte _PWM = 128)
{
    int _SWPWM = PWM - 30;
    if (_SWPWM < 255)
    {
        /* code */
    }
    
    MotorsF.setSpeed(_PWM);
    MotorsB.setSpeed(_PWM);
    switch (dir)
    {
    case 0:
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 1:
        MotorsF.setSpeedB(_PWM-30);
        MotorsB.setSpeedB(_PWM-30);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 2:
        MotorsF.setSpeedB(_PWM-50);
        MotorsB.setSpeedB(_PWM-50);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 3:
        MotorsF.setSpeedB(_PWM-90);
        MotorsB.setSpeedB(_PWM-90);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 4:
        MotorsF.setSpeedA(_PWM-30);
        MotorsB.setSpeedA(_PWM-30);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 5:
        MotorsF.setSpeed(_PWM-50);
        MotorsB.setSpeed(_PWM-50);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 6:
        MotorsF.setSpeed(_PWM-50);
        MotorsB.setSpeed(_PWM-50);
        MotorsF.backwardA();
        MotorsB.backwardA();
        MotorsF.forwardB();
        MotorsB.forwardB();
        break;
    case 7:
        MotorsF.setSpeedA(_PWM-30);
        MotorsB.setSpeedA(_PWM-30);
        MotorsF.setSpeedB(_PWM-50);
        MotorsB.setSpeedB(_PWM-50);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 8:
        MotorsF.setSpeedA(_PWM-50);
        MotorsB.setSpeedA(_PWM-50);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 9:
        MotorsF.setSpeed(_PWM-50);
        MotorsB.setSpeed(_PWM-50);
        MotorsF.forwardA();
        MotorsB.forwardA();
        MotorsF.backwardB();
        MotorsB.backwardB();
        break;
    case 10:
        MotorsF.setSpeed(_PWM-50);
        MotorsB.setSpeed(_PWM-50);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 11:
        MotorsF.setSpeedA(_PWM-30);
        MotorsB.setSpeedA(_PWM-30);
        MotorsF.setSpeedB(_PWM-50);
        MotorsB.setSpeedB(_PWM-50);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 12:
        MotorsF.setSpeedA(_PWM-90);
        MotorsB.setSpeedA(_PWM-90);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 13:
        MotorsF.setSpeedA(_PWM-50);
        MotorsB.setSpeedA(_PWM-50);
        MotorsF.setSpeedB(_PWM-30);
        MotorsB.setSpeedB(_PWM-30);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 14:
        MotorsF.setSpeedA(_PWM-50);
        MotorsB.setSpeedA(_PWM-50);
        MotorsF.setSpeedB(_PWM-30);
        MotorsB.setSpeedB(_PWM-30);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 15:
        MotorsF.setSpeed(_PWM-50);
        MotorsB.setSpeed(_PWM-50);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 16:
        MotorsF.setSpeed(_PWM-50);
        MotorsB.setSpeed(_PWM-50);
        MotorsF.backward();
        MotorsB.backward();
        break;
    }
}

void motoManual(byte dir, int _PWM = 63)
{
    byte _SWPWM = _PWM - 30;
    MotorsF.setSpeed(_PWM);
    MotorsB.setSpeed(_PWM);
    switch (dir)
    {       //控制
    case 1: // 左前
        MotorsF.setSpeedB(_SWPWM);
        MotorsB.setSpeedB(_SWPWM);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 2: // 前
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 3: // 右前
        MotorsF.setSpeedA(_SWPWM);
        MotorsB.setSpeedA(_SWPWM);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 4: // 左
        MotorsF.backwardA();
        MotorsB.backwardA();
        MotorsF.forwardB();
        MotorsB.forwardB();
        break;
    case 5: // 停
        MotorsF.stop();
        MotorsB.stop();
        break;
    case 6: // 右
        MotorsF.forwardA();
        MotorsB.forwardA();
        MotorsF.backwardB();
        MotorsB.backwardB();
        break;
    case 7: // 左後
        MotorsF.setSpeedB(_SWPWM);
        MotorsB.setSpeedB(_SWPWM);
        MotorsF.backward();
        MotorsB.backward();
        break;
    case 8: // 後
        MotorsF.backward();
        MotorsB.backward();
        break;
    case 9: // 右後
        MotorsF.setSpeedA(_SWPWM);
        MotorsB.setSpeedA(_SWPWM);
        MotorsF.backward();
        MotorsB.backward();
        break;
    }
}

void ESP32Write()
{
    
}

void ESP32Read()
{
    if (Serial1.available())
    {
        Serial1.readBytes(ReadData,8);
        int Check = ReadData[0] + ReadData[1];
        if (Check == 67)
        {
            for (byte i = 0; i < sizeof(ReadData); i++)
            {
                Serial.print(ReadData[i], HEX);
                Serial.print(",");
            }
            Serial.println();
        }
        else
        {
            Serial.println("BUG");
            while (Serial1.read() >= 0){}
        }
        
    }
    ADIR = ReadData[2];
    DIR = ReadData[3];
    PWM = ReadData[4];
    mode = ReadData[5];
}

void loop()
{
    Serial.print("Mode:");
    Serial.println(mode);
    while (mode == 0)
    {
        ESP32Read();
        motoManual(DIR,PWM);
    }
    ESP32Read();
    motoAuto(ADIR);
    delay(10);
}