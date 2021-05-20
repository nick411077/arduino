#include <Arduino.h>

int s0 = 8;        //TCS 3200 感測器 S0 連接 Arduino pin 8
int s1 = 9;        //TCS 3200 感測器 S1 連接 Arduino pin 9
int s2 = 12;       //TCS 3200 感測器 S2 連接 Arduino pin 12
int s3 = 11;       //TCS 3200 感測器 S3 連接 Arduino pin 11
int out = 10;      //TCS 3200 感測器 OUT 連接 Arduino pin 10
int redLed = 4;    //RGB LED 腳R 連接 Arduino pin 4
int greenLed = 3;  //RGB LED 腳G 連接 Arduino pin 3
int blueLed = 2;   //RGB LED 腳B 連接 Arduino pin 2
int red = 0;       //設定參數
int green = 0;  
int blue = 0;  
    
void setup()   
{  
  Serial.begin(9600); 
  pinMode(s0, OUTPUT);  
  pinMode(s1, OUTPUT);  
  pinMode(s2, OUTPUT);  
  pinMode(s3, OUTPUT);  
  pinMode(out, INPUT);  
  pinMode(redLed, OUTPUT);  
  pinMode(greenLed, OUTPUT);  
  pinMode(blueLed, OUTPUT);  
  digitalWrite(s0, HIGH);  
  digitalWrite(s1, HIGH);  
}  
    
void loop() 
{  
  color(); 
  Serial.print(" R Intensity:");  //在序列埠螢幕中印出R G B三種顏色感應器輸出值
  Serial.print(red, DEC);  
  Serial.print(" G Intensity: ");  
  Serial.print(green, DEC);  
  Serial.print(" B Intensity : ");  
  Serial.print(blue, DEC);  

  //當紅色 R 輸出值為最低時，在序列埠螢幕中印出 Red Color，點亮紅色 LED
  //程式加上 red < 10 ，用意在於沒有顏色紙板靠近感應器的時候，讓 LED 燈不會亮。
  
  if (red < blue && red < green && red < 10)                 
  {  
   Serial.println(" - (Red Color)");  
   digitalWrite(redLed, HIGH); 
   digitalWrite(greenLed, LOW);  
   digitalWrite(blueLed, LOW);  
  }  

  else if (blue < red && blue < green)   //當藍色 B 輸出值為最低時，在序列埠螢幕中印出 Blue Color，點亮藍色 LED
  {  
   Serial.println(" - (Blue Color)");  
   digitalWrite(redLed, LOW);  
   digitalWrite(greenLed, LOW);  
   digitalWrite(blueLed, HIGH);  
  }  

  else if (green < red && green < blue)  //當綠色 G 輸出值為最低時，在序列埠螢幕中印出 Green Color，點亮綠色 LED
  {  
   Serial.println(" - (Green Color)");  
   digitalWrite(redLed, LOW);  
   digitalWrite(greenLed, HIGH); 
   digitalWrite(blueLed, LOW);  
  }  
  else{
  Serial.println();  
  }
  delay(300);   
  digitalWrite(redLed, LOW);  
  digitalWrite(greenLed, LOW);  
  digitalWrite(blueLed, LOW);  
 }  
    
void color()                  //副程式 color
{    
  digitalWrite(s2, LOW);                                                // S2 LOW and S3 LOW, 打開紅色濾鏡
  digitalWrite(s3, LOW);  
  red = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);  
  digitalWrite(s3, HIGH);                                               // S2 LOW and S3 HIGH, 打開藍色濾鏡
  blue = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);  
  digitalWrite(s2, HIGH);                                               // S2 HIGH and S3 HIGH, 打開綠色濾鏡
  green = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);  
}