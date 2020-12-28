#include <Arduino.h>
//馬達在右邊finish
int pulse = 26, dir = 27, enable = 14;      //Arduino給驅動器的腳位
int V1 = 37, V2 = 39, V3 = 41;              //給驅動器5V
int interruptR = 9, interruptL = 8;         //光遮斷器，9號是右邊，8號是左邊
int T_pulse = 200;                          //microsecond
long pulse_num = 20000;                     //Arduino產生的脈衝數
int sign = 0, readnum = 0;
void stepperpulse();                      //產生脈衝的函式
void setup()
{
  Serial.begin(9600);
  //設定I/O腳位
  pinMode( pulse, OUTPUT );
  pinMode( dir, OUTPUT );
  pinMode( enable, OUTPUT );
  pinMode( V1, OUTPUT );
  pinMode( V2, OUTPUT );
  pinMode( V3, OUTPUT );
  pinMode( interruptR,INPUT );
  pinMode( interruptL,INPUT );
  //腳位初始化
  digitalWrite( V1, 1 );
  digitalWrite( V2, 1 );
  digitalWrite( V3, 1 );
  digitalWrite( enable, 0 );
  delay(100);
  digitalWrite( dir, 0 );     // 0:反轉  1:正轉
  delay(100);
  digitalWrite( pulse, 1 );
  for( long count = 0 ; count < 2000 ; count++ )   
  stepperpulse(T_pulse);
}
void loop()
{
  for( long count = 0 ; count < pulse_num ; count++ )      
  {
    int flagR = digitalRead(interruptR);              //右邊光遮斷器的旗標，碰到時會變成低電位
    if (true)
      stepperpulse(T_pulse); 
    else
      pulse_num = 0;
  }
}
void stepperpulse(long T_pulse)
{                                                        
  long time1 = micros();                            //紀錄當時的時間
  digitalWrite( pulse, 0 );                        
  while( micros() - time1 < T_pulse - 5)          //週期為 T_pulse，其中高電維持 5us
  {                                                 //   _____               _____
  }                                                 //   |    |              |    |
  long time2 = micros();                            // __|    |______________|    |_____________
  digitalWrite( pulse, 1 );                         //
  while( micros() - time2 < 5)
  {
  }
}
