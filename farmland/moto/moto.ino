#include <Arduino.h>
#include <Servo.h> //https://github.com/RoboticsBrno/ServoESP32
#include "APO.h" //

#define RCLPin 25
#define RCRPin 26


Servo RCL;
Servo RCR;
APO RC;


void setup()
{
    Serial.begin(115200); //
    RCL.attach(RCLPin,1,0,180,1000,2000);
    RCR.attach(RCRPin,1,0,180,1000,2000);
    RC.attach(RCL,RCR);
    RC.write(90,90);//整合控制

}

void loop()
{
    
}

void Auto(byte DIR, int PWMA, int PWMB)
{
    switch (DIR)
    {
    case 0:
        RC.write(90 - PWMA, 90 - PWMB);
        break;
    case 1:
        RC.write(0, 30);
        break;
    case 2:
        RC.write(0, 50);
        break;
    case 3:
        RC.write(0, 90);
        break;
    case 4:
        RC.write(30, 0);
        break;
    case 5:
        RC.write(50, 50);
        break;
    case 6:
        RC.write(50, 130); // 往左自轉
        break;
    case 7:
        RC.write(0, 0);
        break;
    case 8:
        RC.write(50, 0);
        break;
    case 9:
        RC.write(130, 50); // 往右自轉
        break;
    case 10:
        RC.write(50, 50);
        break;
    case 11:
        RC.write(0, 0);
        break;
    case 12:
        RC.write(90, 0);
        break;
    case 13:
        RC.write(0, 0);
        break;
    case 15:
        RC.write(50, 50);
        break;
    case 16:
        RC.write(130, 130); //後退
        break;
    }
}

void Moto(byte DIR, int PWMA, int PWMB)
{
    switch (DIR)
    {       //控制
    case 1: // 左前
        RC.Forward(PWMA,PWMB);
        break;
    case 2: // 前
        RC.Forward(PWMA,PWMB);
        break;
    case 3: // 右前
        RC.Forward(PWMA,PWMB);
        break;
    case 4: // 左
        RC.Left(PWMA,PWMB);
        break;
    case 5: // 停
        RC.write(90,90);
        break;
    case 6: // 右
        RC.Right(PWMA,PWMB);
        break;
    case 7: // 左後
        RC.Reverse(PWMA,PWMB);
        break;
    case 8: // 後
        RC.Reverse(PWMA,PWMB);
        break;
    case 9: // 右後
        RC.Reverse(PWMA,PWMB);
        break;
    }
}