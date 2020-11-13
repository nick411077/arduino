#include <FlagTank.h>
#include<FlagTankArm.h>


// 若定義 DEBUG 可讓程式將循軌感測值透過接在 UART 的通訊模組 (預設是用 ZigBee) 送回給 PC
// 可方便在 PC 端觀察數據, 協助除錯以及調校
//#define DEBUG

// --------- 表示目前狀態的常數, 用於下方的 state 變數
#define STOP 0        // 停止狀態
#define GO   1        // 去程尋軌前進到停止線
#define CALIBRATE 2   // 矯正車體面向保特瓶
#define CRAB 3        // 抓取保特瓶
#define TURN1 4       // 抓取保特瓶後退後至停止線
#define TURN2 5       // 原地右轉至空白區域
#define TURN3 6       // 持續原地右轉至找到軌道
#define BACK 7        // 回程尋軌至起點放置保特瓶

//----------------1234567890123 
char help[][15]={"Press",
                 "  K3 to Start",  
                 "  K0-2 to Stop"};
byte state;          // 目前 iTank 運作狀態,
                     // 程式中用此狀態控制 iTank 行為

char buf[14]={0};    // 放置格式化字串的暫存區
byte key = 0;        // 儲存讀取的按鍵值
byte line = 0;       // 儲存循軌值
byte bumper;         // 儲存 iTank 前端碰撞感測器值

void setup() {
  iTank.begin();     // 啟用 iTank 程式庫
  iTank.clearLCD();  // 清除畫面
  // 在 LCD 顯示訊息
  // 參數為輸出行號(0~5) 及要輸出的字串
  iTank.writeLCD(0, help[0]);
  iTank.writeLCD(1, help[1]);  
  iTank.writeLCD(2, help[2]);
  pinMode(13, INPUT);
  state = STOP;               // 設定初始狀態為停止
#ifdef DEBUG
  Serial.begin(19200);      // 啟用序列埠 (方便透過 ZigBee 或其他通訊模組回傳感測數據協助除錯調校)
  Serial.print("---100-");  // 讓 ZigBee 模組以編號 100 的節點為目的地進入透明模式
  delay(100);              // 等待 ZigBee 模組進入透明模式
#endif
  iArm.turnToDefault();    // 將機器手臂轉到預設位置
}

// 讀取紅外線循軌感測器值並依據需要傳送感測值回 PC 的函式
/*void getFloorIR(bool sendToSerial) {
  // 讀取紅外線循軌感測值
  // 傳回值的 bit0~3 分別代表左、中、右感測器的感測值
  // bit 值為 0 表示無軌道, 1 表示有軌道
  line=iTank.readFloorIR();

  // 顯示循軌感測值於LCD第2列
  // 無軌道用 '0' 字元代表
  // 有軌道用 '1' 字元代表
  sprintf(buf,"    L:%c%c%c:R",
            (((line&0x01)==0x01)?'1':'0'),  // 左感測器
            (((line&0x02)==0x02)?'1':'0'),  // 中感測器
            (((line&0x04)==0x04)?'1':'0')); // 右感測器 
  iTank.writeLCD(4, buf);
  if(sendToSerial) {
#ifdef DEBUG
    Serial.println(buf); // 從序列埠送出感測值回 PC 協助除錯調校
#endif
  }
}*/

byte tryTimes = 0;            // 校正方向的次數
byte tryLimits = 0;           // 交正次數限制
int dirL, dirR;               // 校正方向時的馬達轉向

void loop() {
  line=0;
   if (analogRead(0)<350){line=line+1;}
   if (analogRead(1)<350){line=line+2;}
   if (analogRead(2)<350){line=line+4;}
   if (analogRead(3)<350){line=line+8;}
   if (analogRead(6)<350){line=line+16;}
   if (analogRead(7)<350){line=line+13;}
  //getFloorIR(false);          // 取得循軌感測器, 不送出感測值回 PC
  key = iTank.readKey();      // 取得 K0~K3 按鈕狀態 
  bumper = iTank.readTouch();
  // 取得前端碰撞感測狀態
  if(bumper>0 || key==4) {    // 碰撞到物體或使用者按 K3
    state=STOP;               // 回復成停止狀態
    iTank.stop();             // 停車
  }
  

  // 依目前狀態決定處理方式
  switch(state){
    case STOP:                // 停止中
      if(key==8){             // 按 K3 即開始循軌行進
        state=GO;             // 變更為去程尋軌狀態 
        iArm.turnToDefault(); // 將手臂恢復為預設位置, 可避免車體重心因手臂偏前
        iTank.writeMotor(2,2);// 前進
        
      }  // end of if(key==8)
      break;
    case GO:                  // 循軌狀態中
      if (line >= 35)
      {               // 遇到停止線
        iTank.stop(); // 先停車
        if (analogRead(7) < 250)
        {
          while ((analogRead(0) > 250))
          {
            if (analogRead(0) < 250)
            {
              line = line + 1;
            }
            if (analogRead(1) < 250)
            {
              line = line + 2;
            }
            if (analogRead(2) < 250)
            {
              line = line + 4;
            }
            if (analogRead(3) < 250)
            {
              line = line + 8;
            }
            if (analogRead(6) < 250)
            {
              line = line + 16;
            }
            if (analogRead(7) < 250)
            {
              line = line + 13;
            }
            iTank.writeMotor(-1, 0);
          }
        }
        else
        {
          while ((analogRead(7) > 250))
          {
            if (analogRead(0) < 250)
            {
              line = line + 1;
            }
            if (analogRead(1) < 250)
            {
              line = line + 2;
            }
            if (analogRead(2) < 250)
            {
              line = line + 4;
            }
            if (analogRead(3) < 250)
            {
              line = line + 8;
            }
            if (analogRead(6) < 250)
            {
              line = line + 16;
            }
            if (analogRead(7) < 250)
            {
              line = line + 13;
            }
            iTank.writeMotor(0, -1);
          }
        }

        iTank.stop(); // 停車準備校正車體方向
        state = CRAB; // 轉換到校正車體方向狀態
        /*dirL = -2;            // 停止線前軌道是左彎道, 車體可能偏右
        dirR = 2;             // 先嘗試原地左轉找校正線
        tryTimes = 0;         // 設定校正次數初始值為 0
        tryLimits = 20;*/
        // 設定最多校正 20 次
      }
      else
      {
        // 呼叫循軌函式
        tryFollowLine(); // 為碰到停止線前都循軌道前進
      }
      break;
    case CALIBRATE:           // 校正車體方向
      if((dirR > 0 && line == 0x01) || // 向左轉找校正線且左邊感測器找到校正線
        (dirL > 0 && line == 0x02)) {  // 向右轉找校正線且中間感測器找到校正線
        state = CRAB;         // 進入抓取保特瓶狀態
        iTank.stop();         // 停車
        delay(500);           // 稍微暫停
      }
      else {
        tryCalibrate();       // 繼續校正車體方向
      }
      break;
    case CRAB:                // 抓取保特瓶
      iTank.writeMotor(-2,-2);// 先後退一小段避免放下手臂撞倒保特瓶
      delay(100);
      iTank.stop();           // 停車
      iArm.turnTo(1,150);     // 舉起手肘
      delay(300);
      iTank.stop();           // 停車
      iArm.show(180,170,90,13,10); // 停整手臂到抓取位置並張開手爪
      delay(300);
        iTank.writeMotor(2,2);  // 前進到抓取位置
      delay(300);
       iTank.stop();  
      iArm.turnTo(3,150);    // 手爪閉合抓住保特瓶
      delay(1000);
      iArm.turnTo(0,0);     // 舉起手臂
      iArm.turnTo(1,9);      // 放下手肘
      delay(1000);
      iTank.writeMotor(-4,-4);
      delay(350);
      state = TURN1;// 進入回程階段
      // 後退
      break;
    case TURN1:

   iTank.writeMotor(5,-5);
       delay(500);
      while(analogRead(1)>350) {     // 偵測到停止線
        //state = TURN2;       // 進入原地右轉找軌道的階段
        iTank.writeMotor(5,-5);
       
      }
      
      
        state=BACK;// 原地右轉
      
      
      break;
    case TURN2:              // 回程後退後原地右轉階段
      if(line == 0x00) {     // 先找到停止線右轉後到軌道間的空白區
        state = TURN3;       // 進入回程尋軌階段 
      }
      break;
    case TURN3:                 // 回程右轉找軌道
      if(line & 0x04 == 0x04) { // 右側感測器找到軌道
        state = BACK;           // 進入循軌回起點的階段
        iTank.writeMotor(2,2);  // 前進
      }
      break;
    case BACK:                  // 循軌回起點
      if(line >=35) {        // 找到起點線
        state = STOP;           // 回到停止狀態
        iTank.stop();           // 停車
        iArm.show(152,143,iArm.read(2),106,10); // 伸出手臂微鬆手爪讓保特瓶落地
        delay(500);
        iArm.turnTo(3,10);      // 手爪全鬆 (兩階段可避免保特瓶倒地)
        delay(500);
        iTank.writeMotor(-2,-2);// 後退離開保特瓶
        delay(500);
        iTank.stop();           // 停車
        iArm.turnTo(3, 180);    // 閉合手爪
      }
      else
      {
      tryFollowLine();}          // 尚未到達起點線前循軌前進
      break;
  } // end of switch(state)
}

// 循軌函式
// 以簡單的邏輯進行控制: 軌道在左即左轉
//                       軌道在右即右轉
//                       軌道在中即直行
void tryFollowLine(void)
{
  switch(line){
    case 1:
       iTank.writeMotor(-4,4);
       break;
    case 3:
        iTank.writeMotor(-2,4);
        break;
    case 2:
        iTank.writeMotor(-1,4);
        break;
    case 6:
        iTank.writeMotor(2,6);
        break;
    case 4:
        iTank.writeMotor(1,2);
        break;
    case 12:
        iTank.writeMotor(5,5);
        break;
    case 8:
        iTank.writeMotor(2,1);
        break;
    case 24:
        iTank.writeMotor(6,2);
        break;  
    case 16:
        iTank.writeMotor(4,-1);
        break;
    case 29:
        iTank.writeMotor(4,-2);
        break;
    case 13:
        iTank.writeMotor(4,-4);
        break;         
    }
    delay(20);
}

// 校正車體方向
// 先分段左轉找校正線
// 若一段時間找不到則原地右轉找校正線
void tryCalibrate(void) {
  iTank.writeMotor(dirL,dirR);   // 原地旋轉 50 毫秒
  delay(50);
  iTank.stop();                  // 停車, 避免旋轉過猛變更車體方向
  if(++tryTimes > tryLimits) {   // 如果已分段旋轉超過設定的次數
    // 一開始車體是在校正線附近開始向左轉, 如果左轉到底找不到校正線,
    // 就要往右轉, 這時會把限制次數從 20 次加倍為 40 次, 可從左往右轉到底
    tryLimits = 40;              // 調整限制次數為 40 次
    dirL = -dirL;                // 往反方向旋轉
    dirR = -dirR;                // 往返方向旋轉
    tryTimes = 0;                // 將次數歸零
  }
}
