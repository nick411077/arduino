#include <Arduino.h>
#include <SoftwareSerial.h>
byte UpV = 30, DownV = 30, LeftV = 30, RightV = 30; //數值從$00(停止)到$3F(高速)

//byte UpCMD[7]={0xFF,0x01,0x00,0x08,0x00,0x38,0x41};
byte UpCMD[7] = {0xFF, 0x01, 0x00, 0x08, 0x00, UpV, 0x09 + UpV};
byte UpStopCMD[7] = {0xFF, 0x01, 0x00, 0x08, 0x00, 0x00, 0x09};

//byte DownCMD[7]={0xFF,0x01,0x00,0x10,0x00,0x38,0x49};
byte DownCMD[7] = {0xFF, 0x01, 0x00, 0x10, 0x00, DownV, 0x11 + DownV};
byte DownStopCMD[7] = {0xFF, 0x01, 0x00, 0x10, 0x00, 0x00, 0x11};

//byte LeftCMD[7]={0xFF,0x01,0x00,0x04,0x18,0x00,0x1D};
byte LeftCMD[7] = {0xFF, 0x01, 0x00, 0x04, LeftV, 0x00, 0x05 + LeftV};
byte LeftStopCMD[7] = {0xFF, 0x01, 0x00, 0x04, 0x00, 0x00, 0x05};

//byte RightCMD[7]={0xFF,0x01,0x00,0x02,0x18,0x00,0x1B};
byte RightCMD[7] = {0xFF, 0x01, 0x00, 0x02, RightV, 0x00, 0x03 + RightV};
byte RightStopCMD[7] = {0xFF, 0x01, 0x00, 0x02, 0x00, 0x00, 0x03};

SoftwareSerial mySerial2(2, 3);
int data;
void setup()
{
  // Open Serial2 communications and wait for port to open:
  Serial2.begin(9600);
  mySerial2.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  delay(5000);
}

void loop()
{
  if (mySerial2.available() > 1)
  {
    while (mySerial2.available() > 0)
    {
      data = mySerial2.parseInt();

      switch (data)
      {       //控制
      case 1: // 左前
        Up();
        break;
      case 2: // 前
        UpStop();
        break;
      case 3: // 右前
        Down();
        break;
      case 4: // 左
        DownStop();
        break;
      case 5: // 停
        Left();
        break;
      case 6: // 右
        LeftStop();
        break;
      case 7: // 左後
        Right();
        break;
      case 8: // 後
        RightStop();
      case 9: // 右後
        break;
      }
    }
  }
}

void Up() //PTZ上
{
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH); //HIGH傳送
  delay(20);
  for (byte z = 0; z < sizeof(UpCMD); z++)
  {
    Serial2.write(UpCMD[z]);
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  digitalWrite(5, LOW); //LOW接收
  delay(20);
}

void UpStop() //PTZ上
{
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH); //HIGH傳送
  delay(20);
  for (byte z = 0; z < sizeof(UpStopCMD); z++)
  {
    Serial2.write(UpStopCMD[z]);
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  digitalWrite(5, LOW); //LOW接收
  delay(20);
}

void Down() //PTZ下
{
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH); //HIGH傳送
  delay(20);
  for (byte z = 0; z < sizeof(DownCMD); z++)
  {
    Serial2.write(DownCMD[z]);
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  digitalWrite(5, LOW); //LOW接收
  delay(20);
}

void DownStop() //PTZ下
{
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH); //HIGH傳送
  delay(20);
  for (byte z = 0; z < sizeof(DownStopCMD); z++)
  {
    Serial2.write(DownStopCMD[z]);
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  digitalWrite(5, LOW); //LOW接收
  delay(20);
}

void Left() //PTZ左
{
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH); //HIGH傳送
  delay(20);
  for (byte z = 0; z < sizeof(LeftCMD); z++)
  {
    Serial2.write(LeftCMD[z]);
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  digitalWrite(5, LOW); //LOW接收
  delay(20);
}

void LeftStop() //PTZ左
{
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH); //HIGH傳送
  delay(20);
  for (byte z = 0; z < sizeof(LeftStopCMD); z++)
  {
    Serial2.write(LeftStopCMD[z]);
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  digitalWrite(5, LOW); //LOW接收
  delay(20);
}

void Right() //PTZ右
{
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH); //HIGH傳送
  delay(20);
  for (byte z = 0; z < sizeof(RightCMD); z++)
  {
    Serial2.write(RightCMD[z]);
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  digitalWrite(5, LOW); //LOW接收
  delay(20);
}

void RightStop() //PTZ右
{
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH); //HIGH傳送
  delay(20);
  for (byte z = 0; z < sizeof(RightStopCMD); z++)
  {
    Serial2.write(RightStopCMD[z]);
  }
  delay(20);
  digitalWrite(4, LOW); //LOW接收
  digitalWrite(5, LOW); //LOW接收
  delay(20);
}