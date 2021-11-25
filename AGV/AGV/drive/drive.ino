#include "Arduino.h"
#include <Servo.h>

// 馬達宣告
Servo LeftFrontTire;
Servo RightFrontTire;
Servo LeftBackTire;
Servo RightBackTire;
// -------

// Line inspection
String LineInspectionData = "00000000";
bool LineInspectionRotationDirection = false;

int MaximumSpeed = 40;
int LeftTargetSpeed = 0;
int RightTargetSpeed = 0;
int LeftCurrentSpeed = 0;
int RightCurrentSpeed = 0;
// ---------------

// DriveControl 手動
int Front[] = {MaximumSpeed, MaximumSpeed, MaximumSpeed, MaximumSpeed};
int Back[] = {-MaximumSpeed, -MaximumSpeed, -MaximumSpeed, -MaximumSpeed};
int Stop[] = {0, 3, 0, 3};
int LeftFront[] = {0, MaximumSpeed, 0, MaximumSpeed};
int RightFront[] = {MaximumSpeed, 0, MaximumSpeed, 0};
int LeftBack[] = {0, -MaximumSpeed, 0, -MaximumSpeed};
int RightBack[] = {-MaximumSpeed, 0, -MaximumSpeed, 0};
int Left[] = {-MaximumSpeed, MaximumSpeed, MaximumSpeed, -MaximumSpeed};
int Right[] = {MaximumSpeed, -MaximumSpeed, -MaximumSpeed, MaximumSpeed};
// -----------------
// 模式
byte mode = 0;
// ----
// Foolproof
unsigned long FoolproofPreviousTime = 0;
// ---------
// 充電
bool SupplementalPowerSupply = false;
// ----
void setup()
{
  DriveInit();
  RFIDInit();
  LineInspectionInit();
}

void loop()
{
  if (mode != 0)
  {
    LineInspectionRead();
    delay(3);
    Foolproof();
  }
  else
  {
    FoolproofPreviousTime = millis();
  }
  RFIDRead();
  delay(3);
}
// 充電
void SupplementalPowerSupplyTrue()
{
  long StartingTime = millis();
  LeftCurrentSpeed = 0;
  RightCurrentSpeed = 0;
  DriveControl(Front);
  while (true)
  {   
    if(analogRead(A15) != 0){
      SupplementalPowerSupply = true;
      break;
    }else if((millis()-StartingTime)>5000){
      SupplementalPowerSupplyFalse();
      break;
    }
    delay(500);
  }
  DriveControl(Stop);
  LineInspectionEmpty();
}
void SupplementalPowerSupplyFalse()
{
  DriveControl(Front);
  DriveControl(Back);
  delay(1000);
  DriveControl(Stop);
  SupplementalPowerSupply = false;
}
// ----

// Foolproof
void Foolproof()
{
  if (LineInspectionData == "00000000")
  {
    if ((millis() - FoolproofPreviousTime) > 12000)
    {
      MessageProcessing('s');
    }
  }
  else
  {
    FoolproofPreviousTime = millis();
  }
}
// ---------
// 接收處理
void MessageProcessing(char data)
{
  if (SupplementalPowerSupply)
  {
    SupplementalPowerSupplyFalse();
  }
  if (data == '0')
  {
    mode = 0;
  }
  else if (data == '1')
  {
    mode = 1;
    LineInspectionEmpty();
    LineInspection(LineInspectionData);
  }
  else if (data == '2')
  {
    mode = 2;
    LineInspectionEmpty();
    LineInspection(LineInspectionData);
  }
  else if (data == '3')
  {
    mode = 3;
    LineInspectionEmpty();
    LineInspection(LineInspectionData);
  }
  else if (data == '4')
  {
    mode = 4;
    LineInspectionEmpty();
    LineInspection(LineInspectionData);
  }
  else if (data == 'h')
  {
    RFIDTurnAround();
  }
  else if (data == 't')
  {
    mode = 0;
    SupplementalPowerSupplyTrue();
  }
  else if (data == 'q')
  {
    DriveControl(LeftFront);
  }
  else if (data == 'w')
  {
    DriveControl(Front);
  }
  else if (data == 'e')
  {
    DriveControl(RightFront);
  }
  else if (data == 'a')
  {
    DriveControl(Left);
  }
  else if (data == 's')
  {
    mode = 0;
    DriveControl(Stop);
  }
  else if (data == 'd')
  {
    DriveControl(Right);
  }
  else if (data == 'z')
  {
    DriveControl(LeftBack);
  }
  else if (data == 'x')
  {
    DriveControl(Back);
  }
  else if (data == 'c')
  {
    DriveControl(RightBack);
  }
}
// --

// Drive
void DriveInit()
{
  LeftFrontTire.attach(6, 1000, 2000);
  RightFrontTire.attach(7, 1000, 2000);
  LeftBackTire.attach(8, 1000, 2000);
  RightBackTire.attach(9, 1000, 2000);
  pinMode(A15,INPUT);
}
void DriveControl(int data[])
{
  DriveControl(data[0], data[1], data[2], data[3]);
}
void DriveControl(int LF, int RF, int LB, int RB)
{
  int LeftSpeed = 90;
  int RightSpeed = 93;
  LeftFrontTire.write(LeftSpeed + LF);
  RightFrontTire.write(RightSpeed - RF);
  LeftBackTire.write(LeftSpeed + LB);
  RightBackTire.write(RightSpeed - RB);
}
// -----

// RFID
void RFIDInit()
{
  Serial2.begin(115200);
}
void RFIDRead()
{
  if (Serial2.available() > 0)
  {
    MessageProcessing(Serial2.read());
  }
}

void RFIDTurnAround()
{
  if (LineInspectionRotationDirection)
  {
    LeftCurrentSpeed = -MaximumSpeed;
    RightCurrentSpeed = MaximumSpeed;
    DriveControl(LeftCurrentSpeed, RightCurrentSpeed, LeftCurrentSpeed, RightCurrentSpeed);
  }
  else
  {
    LeftCurrentSpeed = MaximumSpeed;
    RightCurrentSpeed = -MaximumSpeed;
    DriveControl(LeftCurrentSpeed, RightCurrentSpeed, LeftCurrentSpeed, RightCurrentSpeed);
  }
  delay(1000);
  LineInspectionEmpty();
  LineInspection(LineInspectionData);
}
// ----

// Line inspection
void LineInspectionInit()
{
  byte LineInspectionStart[8] = {0x01, 0x06, 0x00, 0x2F, 0x00, 0x05, 0x78, 0x00};
  Serial1.begin(115200);
  Serial1.write(LineInspectionStart, sizeof(LineInspectionStart));
  Serial1.flush();
  delay(5);
  LineInspectionEmpty();
}
void LineInspectionRead()
{
  if (Serial1.available() > 0)
  {
    LineInspectionData = "00000000";
    byte Data[8];
    Serial1.readBytes(Data, 8);
    byte i = 7;
    while (Data[5] != 0)
    {
      LineInspectionData[i] = (Data[5] % 2 == 0 ? '0' : '1');
      Data[5] /= 2;
      i--;
    }
    LineInspection(LineInspectionData);
  }
  LeftCurrentSpeed = SpeedStartAndEnd(LeftCurrentSpeed, LeftTargetSpeed);
  RightCurrentSpeed = SpeedStartAndEnd(RightCurrentSpeed, RightTargetSpeed);
  DriveControl(LeftCurrentSpeed, RightCurrentSpeed, LeftCurrentSpeed, RightCurrentSpeed);
}
void LineInspectionEmpty()
{
  while (Serial1.read() >= 0)
  {
  }
  LineInspectionData = "00000000";
}
void LineInspection(String Data)
{
  LeftTargetSpeed = 0;
  RightTargetSpeed = 0;
  if (Data == "11111111")
  {
    LeftTargetSpeed = 20;
    RightTargetSpeed = 20;
  }
  else if (Data == "00000000")
  {
    if (LineInspectionRotationDirection)
    {
      LeftTargetSpeed = -MaximumSpeed;
      RightTargetSpeed = MaximumSpeed;
    }
    else
    {
      LeftTargetSpeed = MaximumSpeed;
      RightTargetSpeed = -MaximumSpeed;
    }
  }
  else
  {
    for (int i = 0; i < 4; i++)
    {
      if (Data[i] == '1')
      {
        RightTargetSpeed += (MaximumSpeed / 2);
      }
      if (Data[7 - i] == '1')
      {
        LeftTargetSpeed += (MaximumSpeed / 2);
      }
    }
    if (LeftTargetSpeed > MaximumSpeed)
    {
      if (LeftTargetSpeed == (MaximumSpeed * 2))
      {
        LineInspectionRotationDirection = false;
      }
      RightTargetSpeed = RightTargetSpeed - (LeftTargetSpeed - MaximumSpeed);
      LeftTargetSpeed = MaximumSpeed;
    }
    if (RightTargetSpeed > MaximumSpeed)
    {
      if (RightTargetSpeed == (MaximumSpeed * 2))
      {
        LineInspectionRotationDirection = true;
      }
      LeftTargetSpeed = LeftTargetSpeed - (RightTargetSpeed - MaximumSpeed);
      RightTargetSpeed = MaximumSpeed;
    }
  }
}
int SpeedStartAndEnd(int SpeedStart, int SpeedEnd)
{
  int AccumulatedValue = 2;
  if (SpeedStart == SpeedEnd)
  {
    return SpeedEnd;
  }
  else if (SpeedStart < SpeedEnd)
  {
    return SpeedStart + AccumulatedValue;
  }
  else if (SpeedStart > SpeedEnd)
  {
    return SpeedStart - AccumulatedValue;
  }
}
// ---------------
