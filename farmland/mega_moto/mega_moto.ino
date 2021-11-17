#include <Arduino.h>
#include <L298NX2.h>

//Servo設置
#define RCLPin 25
#define RCRPin 26

#define ENL1 7
#define ENL2 6

#define INL1A 30
#define INL1B 31
#define INL2A 32
#define INL2B 33

#define ENR1 5
#define ENR2 4

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
//ToF
byte ToFData[7];
byte Data[8] = {0x50,0x03,0x00,0x34,0x00,0x01,0xC8,0x45};
unsigned int Long;

// 燈號狀態更新
long previousTime = 0;  // 用來保存前一次狀態的時間
long interval = 500;   // 讀取間隔時間，單位為毫秒(miliseconds)
unsigned long currentTime;

void setup()
{
    Serial.begin(115200); //
    Serial1.begin(115200);
    Serial2.begin(115200);
}

void motoAuto(byte dir, byte _PWM)
{
    byte PW = map(_PWM,0,100,0,250);
    byte _SWPWM = PW-50;
    if (PW<=50){_SWPWM=0;}
    
    MotorsF.setSpeed(PW);
    MotorsB.setSpeed(PW);
    switch (dir)
    {
    case 0:
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 1:
        MotorsF.setSpeedB(PW-30);
        MotorsB.setSpeedB(PW-30);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 2:
        MotorsF.setSpeedB(PW-50);
        MotorsB.setSpeedB(PW-50);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 3:
        MotorsF.setSpeedB(PW-90);
        MotorsB.setSpeedB(PW-90);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 4:
        MotorsF.setSpeedA(PW-30);
        MotorsB.setSpeedA(PW-30);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 5:
        MotorsF.setSpeed(PW-50);
        MotorsB.setSpeed(PW-50);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 6:
        MotorsF.setSpeed(PW-50);
        MotorsB.setSpeed(PW-50);
        MotorsF.backwardA();
        MotorsB.backwardA();
        MotorsF.forwardB();
        MotorsB.forwardB();
        break;
    case 7:
        MotorsF.setSpeedA(PW-30);
        MotorsB.setSpeedA(PW-30);
        MotorsF.setSpeedB(PW-50);
        MotorsB.setSpeedB(PW-50);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 8:
        MotorsF.setSpeedA(PW-50);
        MotorsB.setSpeedA(PW-50);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 9:
        MotorsF.setSpeed(PW-50);
        MotorsB.setSpeed(PW-50);
        MotorsF.forwardA();
        MotorsB.forwardA();
        MotorsF.backwardB();
        MotorsB.backwardB();
        break;
    case 10:
        MotorsF.setSpeed(PW-50);
        MotorsB.setSpeed(PW-50);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 11:
        MotorsF.setSpeedA(PW-30);
        MotorsB.setSpeedA(PW-30);
        MotorsF.setSpeedB(PW-50);
        MotorsB.setSpeedB(PW-50);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 12:
        MotorsF.setSpeedA(PW-90);
        MotorsB.setSpeedA(PW-90);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 13:
        MotorsF.setSpeedA(PW-50);
        MotorsB.setSpeedA(PW-50);
        MotorsF.setSpeedB(PW-30);
        MotorsB.setSpeedB(PW-30);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 14:
        MotorsF.setSpeedA(PW-50);
        MotorsB.setSpeedA(PW-50);
        MotorsF.setSpeedB(PW-30);
        MotorsB.setSpeedB(PW-30);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 15:
        MotorsF.setSpeed(PW-50);
        MotorsB.setSpeed(PW-50);
        MotorsF.forward();
        MotorsB.forward();
        break;
    case 16:
        MotorsF.setSpeed(PW-50);
        MotorsB.setSpeed(PW-50);
        MotorsF.backward();
        MotorsB.backward();
        break;
    }
}

void motoManual(byte dir, int _PWM = 45)
{
    byte PW = map(_PWM,0,100,0,250);
    byte _SWPWM = PW-50;
    if (PW<=50){_SWPWM=0;}
  
    MotorsF.setSpeed(PW);
    MotorsB.setSpeed(PW);
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
        Serial2.readBytes(ToFData, 7);
        int Check = ToFData[0] + ToFData[1] + ToFData[2];
        if (Check == 0x55)
        {
            for (byte i = 0; i < sizeof(ToFData); i++)
            {
                Serial.print(ToFData[i], HEX);
                Serial.print(",");
            }
            Long =  (uint16_t)(ToFData[3] << 8) + ToFData[4];
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

void loop()
{
    //currentTime = millis(); // 獲取時間用與判斷是否更新
    if (mode==0)
    {
        ESP32Read();
        motoManual(DIR,PWM);
    }
    else
    {
        ESP32Read();
        motoAuto(ADIR,PWM);
    }
    /*if (currentTime - previousTime > interval)
    {
        ToF();
        previousTime = currentTime;
    }*/
}