
#include "Wire.h"
#include "FlagTank.h"
#include "FlagTankArm.h"

#define STOP 0     // 停止狀態
#define GO 1       // 尋軌狀態

//超音波
int Ultrasound(int trigPin, int echoPin)
{
  long duration;
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH,5000);
  duration = duration / 59;
  if ((duration < 5) || (duration > 50)) return 50;
  return duration;
}
//----------------1234567890123
char help[][15] = {"Press",
                   "  K3 to Start",
                   "  K0-2 to Stop"};
char light1[15]={0};
char light2[15]={0};

byte state; // 目前iTank運作狀態,
            // 程式中用此狀態控制iTank行為

byte key = 0;
byte lang = 0; // 儲存讀取的按鍵值
byte line = 0; // 儲存循軌值

void setup()
{
  digitalWrite(12, LOW);
  iArm.turnTo(180,169,90,22);
  Serial.begin(9600);
  iTank.begin();    // 讓程式等待 iTank 就緒
  iTank.clearLCD(); // 清除畫面
  
  // 在 LCD 顯示訊息
  // 參數為輸出行號(0~5) 及要輸出的字串
  iTank.writeLCD(0, help[0]);
  iTank.writeLCD(1, help[1]);  
  iTank.writeLCD(2, help[2]);

  state = STOP;
}

void loop()
{
  key = iTank.readKey();
  if (key == 4)
  {               // 碰撞到物體或使用者按 K3
    state = STOP; // 回復成停止狀態
    iTank.stop(); // 停車
  }
 switch (state)
  {
  case STOP: // 停止中
    if (key == 8)
    {                     // 按 K3 即依開始循軌行進
      state = GO; // 變更模式        // 開始前進
      break;
    }
    break;
  case GO: // 循軌中
    testline();
    break;
  }
}
void testline(void)
{
  Serial.print("0:");
  Serial.print(" ");
  Serial.print(analogRead(A0));
  Serial.print(" ");
  Serial.println();
  Serial.print("1:");
  Serial.print(" ");
  Serial.print(analogRead(A1));
  Serial.print(" ");
  Serial.println();
  Serial.print("2:");
  Serial.print(" ");
  Serial.print(analogRead(A2));
  Serial.print(" ");
  Serial.println();
  Serial.print("3:");
  Serial.print(" ");
  Serial.print(analogRead(A3));
  Serial.print(" ");
  Serial.println();
  Serial.print("6:");
  Serial.print(" ");
  Serial.print(analogRead(A6));
  Serial.print(" ");
  Serial.println();
  Serial.print("7:");
  Serial.print(" ");
  Serial.print(analogRead(A7));
  Serial.print(" ");
  Serial.println();
  Serial.print("超音波:");
  Serial.print(" ");
  Serial.print(Ultrasound(12,11));
  Serial.print(" ");
  Serial.println();
  iTank.clearLCD(3);
  iTank.clearLCD(4);
  iTank.clearLCD(5);
  iTank.writeLCD(3,Ultrasound(12,11));
  sprintf(light1,"%d,%d,%d",(analogRead(A0)),(analogRead(A1)),(analogRead(A2)));
  sprintf(light2,"%d,%d,%d",(analogRead(A3)),(analogRead(A6)),(analogRead(A7)));
  iTank.writeLCD(4,light1);
  iTank.writeLCD(5,light2);
  delay(200);
}