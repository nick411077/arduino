/*
	iTank 尋軌避障範例
 
	此範例利用 Arduino Wire (I2C) 函式庫,
	利用　iTank 車頭底部的紅外線循軌感測器偵測軌道, 並循軌道行進
	(黑軌白軌可由iTank內建的設定功能表設定) 
  行進時會利用車頭的追物模組偵測路徑上的障礙物, 並以左轉繞 [ 形狀路徑避過障礙
	
	執行前請先參考iTank使用手冊, 妥善設定紅外線循軌感測器與追物模組
	以免iTank無法正常感測到軌道與障礙物, 造成程式無法正常運作
	
	http://flagsupport.blogspot.tw
 */

// 引用 I2C 通訊函式庫
#include <Wire.h>

// 引用 FlagTank 通訊函式庫
#include <FlagTank.h>
#include <FlagTankArm.h>

// --------- 表示目前狀態的常數, 用於下方的 state 變數
#define STOP 0     // 停止狀態
#define GO 1       // 尋軌狀態
#define L_TURN_1 2 // 左轉避障第一階段
#define L_TURN_2 3 // 左轉避障第二階段
#define R_TURN 4   // 右轉完回軌道階段
#define SecondTIME 5

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
  duration = pulseIn(echoPin, HIGH);
  duration = duration / 59;
  if ((duration < 5) || (duration > 450))
    return false;
  return duration;
}
//----------------1234567890123
char help[][15] = {"Press",
                   "  K3 to Start",
                   "  K0-2 to Stop"};

byte state; // 目前iTank運作狀態,
            // 程式中用此狀態控制iTank行為

char buf[14] = {0}; // 格式化字串用
byte key = 0;
byte lang = 0; // 儲存讀取的按鍵值
byte line = 0; // 儲存循軌值

void setup()
{
  digitalWrite(12, LOW);
  Serial.begin(9600);
  iTank.begin();    // 啟始程式庫
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
  lang = (Ultrasound(12, 11) || Ultrasound(12, 11));
  iTank.clearLCD(3);
  iTank.writeLCD(3, Ultrasound(12, 11));
  line = 0;
  if (analogRead(0) < 350)
  {
    line = line + 1;
  }
  if (analogRead(1) < 350)
  {
    line = line + 2;
  }
  if (analogRead(2) < 350)
  {
    line = line + 4;
  }
  if (analogRead(3) < 350)
  {
    line = line + 8;
  }
  if (analogRead(6) < 350)
  {
    line = line + 16;
  }
  if (analogRead(7) < 350)
  {
    line = line + 13;
  }
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
    tryFollowLine();
    break;
  }
}

void tryFollowLine(void)
{
  switch (line)
  {
  case 1: //從左邊數來第六個(A7)看見
    iTank.writeMotor(5, -3);
    break;
  case 3: //從左邊數來第六個和第五個(A7,A6)看見
    iTank.writeMotor(5, -1);
    break;
  case 2: //從左邊數來第五個(A6)看見
    iTank.writeMotor(5, 0);
    break;
  case 6: //從左邊數來第五個和第四個(A6,A3)看見
    iTank.writeMotor(7, 2);
    break;
  case 4: //從左邊數來第四個(A3)看見
    iTank.writeMotor(6, 2);
    break;
  case 12: //從左邊數來第四個和第三個(A3,A2)看見
    iTank.writeMotor(7, 7);
    break;
  case 8: //從左邊數來第三個(A2)看見
    iTank.writeMotor(2, 6);
    break;
  case 24: //從左邊數來第三個和第二個(A2,A1)看見
    iTank.writeMotor(2, 7);
    break;
  case 16: //從左邊數來第二個(A1)看見
    iTank.writeMotor(0, 5);
    break;
  case 29: //從左邊數來第二個和第一個(A1,D12)看見
    iTank.writeMotor(-1, 5);
    break;
  case 13: //從左邊數來第一個(D12)看見
    iTank.writeMotor(-3, 5);
    break;
  }
  delay(20);
}