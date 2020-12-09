/*
  
  iTank Arduino 應用範例：避障車
 
  此範例利用 Arduino Wire (I2C) 函式庫,
  利用　iTank 車頭底部的紅外線循軌感測器偵測軌道, 並循軌道行進
  (黑軌白軌可由iTank內建的設定功能表設定)  
  
  執行前請先參考iTank使用手冊, 妥善設定紅外線循軌感測器
  以免iTank無法正常感測到軌道, 造成程式無法正常運作
  
  http://flagsupport.blogspot.tw
 */
 
// 引用 I2C 通訊函式庫
#include <Wire.h> 

// 引用 FlagTank 通訊函式庫
#include <FlagTank.h> 
signed long startTurn;
// --------- 表示目前狀態的常數, 用於下方的 state 變數
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


#define STOP 0
#define GO   1

//----------------1234567890123
char help[][15]={"Press",
                 "  K3 to Start",  
                 "  K0-2 to Stop"};

byte state;    // 目前iTank運作狀態,
               // 程式中用此狀態控制iTank行為

char buf[14]={0};
byte key = 0;               // 儲存讀取的按鍵值
byte line = 0;              // 儲存循軌值
byte counts = 0;
byte bumper;                // 儲存iTank前端碰撞感測器值

unsigned long startTime=0;  // 記錄時間

void setup() {
  Serial.begin(19200);
  
  iTank.begin();    // 讓程式等待 iTank 就緒
  iTank.clearLCD(); // 清除畫面
  
  // 在 LCD 顯示訊息
  // 參數為輸出行號(0~5) 及要輸出的字串
  iTank.writeLCD(0, help[0]);
  iTank.writeLCD(1, help[1]);  
  iTank.writeLCD(2, help[2]);
 
  state = STOP;  
}

//用來顯示的符號
byte track =92; // 黑色色塊 
byte ground=95; // 底線字元 '_'
 
void loop() {
  // 讀取紅外線循軌感測值

  // 顯示循軌感測值於LCD第2列
  // 無軌道用 '_' 字元代表 (字碼95) 
  // 有軌道則顯示黑色色塊  (字碼92)
  sprintf(buf,"    L:%c%c%c:R",
            (((line&0x01)==0x01)?track:ground),  // 左感測器
            (((line&0x02)==0x02)?track:ground),  // 中感測器
            (((line&0x04)==0x04)?track:ground)); // 右感測器 
  iTank.writeLCD(4, buf);

  // 取得K0~K3狀態
  byte key = iTank.readKey(); 
  
  // 依目前狀態決定處理方式
  // STOP: 停止中 
  //       - 按K3開始循軌行進
  // GO: 循軌中
 
  switch(state){
    case STOP:    // 停止中
      // 按 K3 即依開始循軌行進
      if(key==8){ 
        state=GO;              // 變更模式  
        if (counts<1){    
        iTank.writeMotor(2,2); // 開始前進,開始起動功率
        counts=counts+1 ;
        }
        
      }  // end of if(key==8)
      break;
      
    case GO:  // 循軌中

    //第1瓶
        while ( ( (   Ultrasound( 12,11 ) ) > ( 25 ) ) )     //超音波值大於30cM就循黑線走
          {
          tryFollowLine();              //循黑線
          }
          Left();                       //避左邊
     //第2瓶
          while ( ( (   Ultrasound( 12,11 ) ) > ( 25 ) ) )     //超音波值大於30cM就循黑線走
          {
          tryFollowLine();              //循黑線
          }
           Left();                     //避右邊
     //第3瓶
          while ( ( (   Ultrasound( 12,11 ) ) > ( 30 ) ) )     //超音波值大於30cM就循黑線走
          {
          tryFollowLine();              //循黑線
          }
           Right();                 //避左邊
          
          //再循7秒
          startTurn = millis();
          delay(20);
          while ((millis() - startTurn) < 7000)   //再循7秒
          {tryFollowLine();}
           
    //結束   
    while (analogRead(0)>350 && (analogRead(7)>350) )     //左1或右1沒碰到黑線
          {
          tryFollowLine();              //循黑線
          } 
        iTank.writeMotor(-1,-1);        //反向急煞
        delay(20);
        iTank.writeMotor(0,0);
        state=STOP;
        break;
  } // end of switch(state)
}


void tryFollowLine(void)      //循軌副程式
{
         if (analogRead(0)<350){iTank.writeMotor(-4,4);}                     //黑線在最左邊
         if (analogRead(0)<350 && analogRead(1)<350 ){iTank.writeMotor(-2,4);}
         if (analogRead(1)<350){iTank.writeMotor(-2,5);}                              //黑線在左邊
         if (analogRead(1)<350 && analogRead(2)<350 ){iTank.writeMotor(1,7);}
         if (analogRead(2)<350){iTank.writeMotor(1,5);}
         if (analogRead(2)<350 && analogRead(3)<350){iTank.writeMotor(4,4);}         //黑線在中間
         if (analogRead(3)<350){iTank.writeMotor(5,1);}
         if (analogRead(3)<350 && analogRead(6)<350 ){iTank.writeMotor(7,1);}
         if (analogRead(6)<350){iTank.writeMotor(5,-2);}                              //黑線在右邊
         if (analogRead(6)<350 && analogRead(7)<350 ){iTank.writeMotor(4,-2);}
         if (analogRead(7)<350){iTank.writeMotor(4,-4);}                              //黑線在最右邊
         
         delay(20);

  }

  //退後車身對齊黑線（校正）
  void Left(void)      
  {
          iTank.writeMotor(-2,-2);    //加反向，退後
          delay(200);
         while  (analogRead(7)>350){    //左轉到右邊光感碰到黑線
          iTank.writeMotor(-4,4);
         }
         delay(200);                    //仰角大一點
         
         iTank.writeMotor(4,4);        //前進到瓶子左側
         delay(1050);
         iTank.writeMotor(4,-3);        //轉回黑線
         delay(1000);
         while  (analogRead(3)>350){
         iTank.writeMotor(2,2);        //前進到黑線停止
         }
         iTank.writeMotor(-2,2); 
         delay(200);     
              
  }
  
void Right(void)      
  {
            iTank.writeMotor(-2,-2);
          delay(300);
                                        //1左閃
                                        //2右閃
                                        //3左閃
         while  (analogRead(1)>350 ){    //右轉到左邊光感碰到黑線
          iTank.writeMotor(4,-4);
         }
         delay(600);                    //仰角大一點
         
         iTank.writeMotor(4,4);        //前進到瓶子左側
         delay(900);
         iTank.writeMotor(-4,4);        //轉回黑線
         delay(850);
         while  (analogRead(3)>350){
         iTank.writeMotor(2,2);        //前進到黑線停止
         }
         iTank.writeMotor(2,-2); 
         delay(200);     
              
  }