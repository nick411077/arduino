#include <HardwareSerial.h>
#include <PZEM004T.h>
HardwareSerial PzemSerial2(2);
PZEM004T pzem(&PzemSerial2); // (RX,TX) connect to TX,RX of PZEM

int channelNo=1;      //0606計算顯示第幾電力支路變數
IPAddress ip(192,168,1,1);

void setup()
{
//  pinMode(9, OUTPUT);  //超過500W就關電
  Serial.begin(115200);  // 用於手動輸入文字
  Serial2.begin(9600);
  pzem.setAddress(ip);
}


//unsigned int i=0;
float e = 0; //總消耗功率
float i = 0;
float v = 0;
float p = 0;
float p1 = 0;
float p2 = 0;

void loop()
{
  digitalWrite(6, HIGH); //1205
  getdata();
  p1 = p;
  digitalWrite(6, LOW); //1205

  digitalWrite(7, HIGH); //1205
  getdata();
  p2 = p;
  digitalWrite(7, LOW); //1205
  
  digitalWrite(8, HIGH); //1205
  getdata();
  e = p1 + p2 + p; //1205 總消耗功率
  digitalWrite(8, LOW); //1205
}

void getdata()
{
  int temp;       //20200510
  int int_i;      //20200510:儲存電流的整數值
  int float_i;    //20200510:儲存電流的小數值
  int int_v;      //20200510:儲存電壓的整數值
  int float_v;    //20200510:儲存電壓的小數值
  
  i = pzem.current(ip);
  //<<0609test
  Serial.print("Pzem Current(A): ");
  Serial.println(i);
  //0609test>>
  temp=i*100;    //20200510
  int_i=temp/100;   //20200510
  float_i=temp%100; //20200510 
       
  v = pzem.voltage(ip);
  temp=v*100;       //20200510
  int_v=temp/100;   //20200510
  float_v=temp%100;  //20200510
//<<20200422
  if (v < 0) v = 0;
  if (i < 0) i = 0;
  if (p < 0) p = 0;
  i=i*10;      //電流以0.1A的單位顯示
  p=(i*v)/10;
//20200422>>

  channelNo=channelNo+1;
  if(channelNo==4)
    channelNo=1;
  
  Serial2.write(int_i);   //20200510
  Serial2.write(float_i); //20200510
  Serial2.write(int_v);   //20200510
  Serial2.write(float_v); //20200510
  Serial2.write(channelNo);       //20200606
  delay(8000);  //1223 test
  //0711>
  
//  <<1223測試
    Serial.print("Time: ");
    Serial.println(millis()); //prints time since program started
    Serial.print("Current(0.1A): ");
    Serial.println(i);
    Serial.print("Voltage: ");
    Serial.println(v);
    Serial.print("Power: ");
    Serial.println(p);
    Serial.print("Total Power: ");
    Serial.println(e);
    Serial.print("Power Channel NO: ");
    Serial.println(channelNo);
    Serial.println();
   //1223測試>> 
}