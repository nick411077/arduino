#include "Arduino.h"

// RFID
byte RFIDSiteTemporaryStorage = 0;
byte mode = 0;
// ----

// 超聲波 (output,input)
int FrontUltrasound[] = {11,10};
int BackUltrasound[]  = {9,8};
int LeftUltrasound[]  = {7,6};
int RightUltrasound[] = {5,4};
char DetermineWhichUltrasoundToTurnOn='0';
bool DoesTheUltrasoundStop = false;
// --------------------

// Battery讀取電池
unsigned long  BatteryOldTime = 0;

// -------

// Towerled警示燈{RED_LED,YELLOW_LED,GREEN_LED,BLUE_LED}
int Towerled[]  = {36,37,38,39};
// ----------------------------------------------------

void setup()
{
  RFIDInit();
  AGBInit();
  PiInit();
  UltrasoundInit();
}
void loop()
{
  PIMessageRead();
  if (mode != 0)
  {
    RFIDRead();
  }
  UltrasoundToDetermineWhetherToContinue();
}

//Battery讀取電池

void Battery()
{
  if ((millis() - BatteryOldTime) > 10000)
  {
    BatteryOldTime = millis();
    float adc_voltage = 0.0;  //分壓電壓
    float in_voltage = 0.0;   //輸入電壓
    // Floats for resistor values in divider (in ohms)
    float R1 = 38300.0;       //電阻
    float R2 = 6800.0;        //電阻
    // Float for Reference Voltage
    float ref_voltage = 5.25; //Arduino 輸入
    // Integer for ADC value
    int adc_value = 0;
    adc_value = analogRead(A0);//數值
    adc_voltage  = (adc_value * ref_voltage) / 1023.0; //換算分壓電壓
    in_voltage = adc_voltage / (R2/(R1+R2)); //換算輸入電壓
    
  }
}
// -------------
// 超聲波
void UltrasoundToDetermineWhetherToContinue(){
  if(DetermineWhichUltrasoundToTurnOn=='1'){
    DetermineIfYouNeedToChange(UltrasonicJudgingSafetyValueIsLow(FrontUltrasound));
  }else if (DetermineWhichUltrasoundToTurnOn=='q')
  {
    DetermineIfYouNeedToChange(UltrasonicJudgingSafetyValueIsLow(LeftUltrasound,FrontUltrasound));
  }else if (DetermineWhichUltrasoundToTurnOn=='w')
  {
    DetermineIfYouNeedToChange(UltrasonicJudgingSafetyValueIsLow(FrontUltrasound));
  }else if (DetermineWhichUltrasoundToTurnOn=='e')
  {
    DetermineIfYouNeedToChange(UltrasonicJudgingSafetyValueIsLow(RightUltrasound,FrontUltrasound));
  }else if (DetermineWhichUltrasoundToTurnOn=='a')
  {
    DetermineIfYouNeedToChange(UltrasonicJudgingSafetyValueIsLow(LeftUltrasound));
  }else if (DetermineWhichUltrasoundToTurnOn=='d')
  {
    DetermineIfYouNeedToChange(UltrasonicJudgingSafetyValueIsLow(RightUltrasound));
  }else if (DetermineWhichUltrasoundToTurnOn=='z')
  {
    DetermineIfYouNeedToChange(UltrasonicJudgingSafetyValueIsLow(LeftUltrasound,BackUltrasound));
  }else if (DetermineWhichUltrasoundToTurnOn=='x')
  {
    DetermineIfYouNeedToChange(UltrasonicJudgingSafetyValueIsLow(BackUltrasound));
  }else if (DetermineWhichUltrasoundToTurnOn=='c')
  {
    DetermineIfYouNeedToChange(UltrasonicJudgingSafetyValueIsLow(RightUltrasound,BackUltrasound));
  }  
}
void DetermineIfYouNeedToChange(bool Data){
  if(Data){
    if (Data != DoesTheUltrasoundStop){
      DoesTheUltrasoundStop = true;
      Serial1.print('s');
    }
  }else{
      if (Data != DoesTheUltrasoundStop){
        DoesTheUltrasoundStop = false;
        Serial1.print(DetermineWhichUltrasoundToTurnOn);
      }
  }
}
bool UltrasonicJudgingSafetyValueIsLow(int data[2],int data1[2]){
  if(UltrasonicJudgingSafetyValueIsLow(data) || UltrasonicJudgingSafetyValueIsLow(data1)){
    return true;
  }else{
    return false;
  }
}
bool UltrasonicJudgingSafetyValueIsLow(int data[2]){
  long SafetyValue = 20;
  if(getUltrasoundDistance(data)<SafetyValue){
    return true;
  }else{
    return false;
  }
}
void UltrasoundInit(){
  UltrasoundInit(FrontUltrasound);
  UltrasoundInit(BackUltrasound);
  UltrasoundInit(LeftUltrasound);
  UltrasoundInit(RightUltrasound);
}
void UltrasoundInit(int Data[2]){
  pinMode(Data[0], OUTPUT);        
  pinMode(Data[1], INPUT);
}

long getUltrasoundDistance(int Data[2]){
  digitalWrite(Data[0], HIGH);     
  
  (2);
  digitalWrite(Data[0], LOW);
  pinMode(Data[1], INPUT);                    
  return ((pulseIn(Data[1], HIGH))/2) / 29.1; 
}
// ------
// pi
void PiInit()
{
  Serial.begin(115200);
}
void PIMessageRead()
{
  if (Serial.available())
  {
    MessageProcessing(Serial.read());
  }
}
void MessageProcessing(char data)
{
  Serial.println(data);
  if (data == '0')
  {
    mode = 0;
    Serial1.print('0');
    DetermineWhichUltrasoundToTurnOn = '0';
  }
  else if (data == '1')
  {
    mode = 1;
    Serial1.print('1');
    DetermineWhichUltrasoundToTurnOn = '1';
  }
  else if (data == '2')
  {
    mode = 2;
    Serial1.print('2');
    DetermineWhichUltrasoundToTurnOn = '1';
  }
  else if (data == '3')
  {
    mode = 3;
    Serial1.print('3');
    DetermineWhichUltrasoundToTurnOn = '1';
  }
  else if (data == '4')
  {
    mode = 4;
    Serial1.print('4');
    DetermineWhichUltrasoundToTurnOn = '1';
  }else if (data == 'q')
  {
    mode = 0;
    Serial1.print('0');
    Serial1.print('q');
    DetermineWhichUltrasoundToTurnOn = 'q';
  }
  else if (data == 'w')
  {
    mode = 0;
    Serial1.print('0');
    Serial1.print('w');
    DetermineWhichUltrasoundToTurnOn = 'w';
  }
  else if (data == 'e')
  {
    mode = 0;
    Serial1.print('0');
    Serial1.print('e');
    DetermineWhichUltrasoundToTurnOn = 'e';
  }
  else if (data == 'a')
  {
    mode = 0;
    Serial1.print('0');
    Serial1.print('a');
    DetermineWhichUltrasoundToTurnOn = 'a';
  }
  else if (data == 's')
  {
    mode = 0;
    Serial1.print('0');
    Serial1.print('s');
    DetermineWhichUltrasoundToTurnOn = '0';
  }
  else if (data == 'd')
  {
    mode = 0;
    Serial1.print('0');
    Serial1.print('d');
    DetermineWhichUltrasoundToTurnOn = 'd';
  }
  else if (data == 'z')
  {
    mode = 0;
    Serial1.print('0');
    Serial1.print('z');
    DetermineWhichUltrasoundToTurnOn = 'z';
  }
  else if (data == 'x')
  {
    mode = 0;
    Serial1.print('0');
    Serial1.print('x');
    DetermineWhichUltrasoundToTurnOn = 'x';
  }
  else if (data == 'c')
  {
    mode = 0;
    Serial1.print('0');
    Serial1.print('c');
    DetermineWhichUltrasoundToTurnOn = 'c';
  }
}
// ---

// AGB
void AGBInit()
{
  Serial1.begin(115200);
}
// ---

// RFID
void RFIDInit()
{
  Serial2.begin(115200);
  byte RFIDStart[8] = {0x01, 0x06, 0x00, 0x04, 0x00, 0x01, 0x09, 0xCB};
  Serial2.write(RFIDStart, sizeof(RFIDStart));
  Serial2.flush();
  delay(5);
}
void RFIDRead()
{
  if (Serial2.available() > 0)
  {
    byte Data[15];
    Serial2.readBytes(Data, 15);
    RFIDSite(Data[9]);
  }
}
void RFIDSite(byte CardReader)
{
  if (CardReader != 0)
  {
    if (CardReader == mode)
    {
      if (CardReader == 4 && RFIDSiteTemporaryStorage == 3)
      {
        mode = 0;
        RFIDSiteTemporaryStorage = 0;
        DetermineWhichUltrasoundToTurnOn='0';
        Serial1.print("t");
      }else if(CardReader == 4 && RFIDSiteTemporaryStorage != 3){
        MessageProcessing('4');
      }else
      {
        mode = 0;
        RFIDSiteTemporaryStorage = 0;
        DetermineWhichUltrasoundToTurnOn='0';
        Serial1.print("s");
      }
      //delay(2000);
      //MessageProcessing(random(49,53));
    }
    else if (RFIDSiteTemporaryStorage != 0)
    {
      if (mode > CardReader && CardReader < RFIDSiteTemporaryStorage)
      {
        Serial1.print("h");
      }
      else if (mode < CardReader && CardReader > RFIDSiteTemporaryStorage)
      {
        Serial1.print("h");
      }
      RFIDSiteTemporaryStorage = CardReader;
    }
    else
    {
      RFIDSiteTemporaryStorage = CardReader;
    }
  }
}
// ----