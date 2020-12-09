
 #include <FlagTank.h>       // 引用 iTank 函式庫
#include <FlagTankVision.h> // 引用 iVision 函式庫
#include <FlagTankArm.h>   // 引用 iArm 函式庫

typedef enum {
  ST_STOP,
  ST_FINDBALL,
  ST_GOFORBALL,
  ST_CENTERBALL,
  ST_ADJCENTER,
  ST_GRABBALL,
  ST_FINDGATE,
  ST_GOFORGATE,
  ST_PUTBALL,
} State_Type;

#define BALL_R_MIN 40   // 找球時的最小球體半徑
#define BALL_R_STOP 130 // 靠近球時停車的球體半徑
#define BALL_R_MAX 200  // 找到並靠近球後, 調整面對球時的球體半徑
#define BALL_X 200

State_Type state = ST_STOP;
int key;
// 開始尋找 RGB 值為 240, 93, 23 的橘色物體
// H、S、V 誤差容許範圍為 +-4, +-40%, +-100%
byte ballColor[] = {255, 112, 63, 4, 40, 100};
byte gateColor[] = {165, 207, 155, 4, 40, 100};
int counts;   // 用來計數連續多次取得同一資訊才算條件成立
int centerR;  // 正對球體時的球體半徑
int centerX;  // 正對球體時的球體 X 座標
void setup() {
  iVision.initSerial();    // 初始化與 iVision 的通訊
  iTank.begin();           // 等待 iTank 就緒
  iTank.writeMotor(0,0);
  iTank.clearLCD();
  iTank.writeLCD(0, "Wait iVision...");
  bool wait = true;
  while(wait) {            // 等待 iVision 就緒
    iVision.getVersion();
    delay(500);
    while(iVision.read()) {
      if(iVision.type=='v') { // 收到版本資訊
        wait = false; 
        break;
      }    
    }
  }
  iTank.writeLCD(0, "iVision Ready");

  while(iVision.read()); // 清除未讀取資料
  iVision.findColor(ballColor, 6);
}

void loop() {  
  key = iTank.readKey();
  if(iVision.checkRead()) {           // 檢查是否有待讀取資料
    iVision.read();                   // 讀取資料
    counts++;                         // 遞增計數
    iTank.writeLCD(1, "     ");       // 清除第 1 列文字
    iTank.writeLCD(1, counts);        // 顯示目前計數
    iTank.writeLCD(2, iVision.type);  // 顯示訊息類別
    char blank[] = "    ";
    // 在 LCD 顯示物體座標
    iTank.writeLCD(3, String("xy=") + iVision.x+", " + iVision.y + blank);
    // 在 LCD 顯示物體面積
    iTank.writeLCD(4, String("a =") + iVision.area + blank);
    // 在 LCD 顯示物體半徑
    iTank.writeLCD(5, String("r =") + iVision.r + blank);
  }
  switch(state) {
    case ST_STOP:                 // 停止狀態下的處理
      if(key == 8) {              // 按了 K3
        state = ST_FINDBALL;      // 進入找球狀態
        iArm.turnTo(20,49,28,22); // 先把手臂擺定位
        delay(100);
        counts = 0;               // 計數歸零
      }
      break;
    case ST_FINDBALL:
      if(iVision.type == 'r' &&     // 收到顏色辨識結果
          iVision.r > BALL_R_MIN && // 符合半徑最小值, 避免誤判環境中其他物體
          iVision.x > BALL_X) {     // 等球出現在畫面中央
        if(counts == 5) {           // 連續 5 次符合條件的辨識結果
          state = ST_GOFORBALL;     // 進入往前靠近球體的狀態
          iVision.showMsg(String("find@") + iVision.x + ":r" + iVision.r + "\n");
          delay(100);               
          iTank.writeMotor(1,1);    // 往前進
        }
      }
      else {
        counts = 0;                 // 辨識條件不符, 計數歸零
        iTank.writeMotor(-1, 1);    // 慢速左轉繼續找球
        delay(100);         
        iTank.writeMotor(0, 0);
        delay(100);                 // 要有間隔時間辨識
      }
      break;
    case ST_GOFORBALL:                // 往前靠近球
      if(iVision.type == 'r' &&       // 收到辨識結果
          iVision.r > BALL_R_STOP) {  // 球體夠大, 表示已經靠近
        iTank.writeMotor(0, 0);       // 停車
        state = ST_CENTERBALL;        // 轉向球中央
        iVision.showMsg(String("stop@") + iVision.x + ":r" + iVision.r + "\n");
      }
      break;
    case ST_CENTERBALL:
      if(iVision.type == 'r' &&       // 收到辨識結果
          (iVision.r > BALL_R_MAX &&  // 球體夠大, 表示已經面對中央
            iVision.x > 105)) {       // 
        state = ST_ADJCENTER;         // 進入修正太過偏左的狀態
        iVision.showMsg(String("center@") + iVision.x + ":r" + iVision.r + "\n");
        centerR = iVision.r;          // 記錄面對球體時球體的半徑
        centerX = iVision.x;
      }
      else {
        iTank.writeMotor(-1, 1);      // 慢速左轉繼續找球
        delay(100);         
        iTank.writeMotor(0, 0);
        delay(100);                   // 要有間隔時間辨識
      }
      break;
    case ST_ADJCENTER:
      if(centerX < 120 || iVision.x < 135) {  // 已經對中 
        state = ST_GRABBALL;                  // 準備抓球
      }
      else if(centerX > 120) {                // 太偏左
        iTank.writeMotor(1,-1);               // 慢速右轉回正中央
        delay(100);
        iTank.writeMotor(0,0);
        delay(100);
      }
      break;
    case ST_GRABBALL:
      iArm.turnTo(180,169,90,22);   // 將手臂平放伸直準備抓球
      delay(500);
      iTank.writeMotor(1,1);        // 向前推進讓抓爪靠球體邊緣
      delay((centerX > 120) ? 250 : 200);
      iTank.writeMotor(0,0);
      iArm.turnTo(180,169,90,150);  // 將抓爪合閉抓住球
      delay(500);
      iArm.turnTo(60,49,90,150);    // 稍微往上抬起手臂才不被置球底座卡住球
      delay(500);
      iTank.writeMotor(-2,-2);      // 後退讓球遠離置球底座
      delay(1000);
      iTank.writeMotor(0,0);
      delay(500);
      iArm.turnTo(20,49,90,150);    // 將手臂高舉恢復回行進姿勢才不會擋到攝影機
      delay(2000);                  
      state = ST_FINDGATE;          // 進入尋找置球區的狀態
      iVision.findColor(gateColor, 6);  // 改為辨識置球區的顏色
      break;
    case ST_FINDGATE:
      if(iVision.type == 'r') {
        iVision.showMsg(String("GF_X:") + iVision.x + "\n");
        iVision.showMsg(String("GF_R:") + iVision.r + "\n");
        if(iVision.x < 400 &&       // 在斜角度找到置球區
            iVision.x > 200) {
          state = ST_GOFORGATE;     // 進入前進到置球區的狀態
          iTank.writeMotor(1,1);    // 前進
          break;
        }
      }
      iTank.writeMotor(1,0);       // 慢慢右轉找置球區
      delay(100);
      iTank.writeMotor(0, 0);
      delay(100);
      break;
    case ST_GOFORGATE:
      if(iVision.type == 'r') {
        iVision.showMsg(String("GG_X:") + iVision.x + "\n");
        iVision.showMsg(String("GG_R:") + iVision.r + "\n");
        if(iVision.r > 210) {     // 辨識到的置球區半徑夠大, 表示已經在置球區邊緣
          state = ST_PUTBALL;     // 進入放球的狀態
          iTank.writeMotor(0,0);
        }
      }
      break;
    case ST_PUTBALL:
      iTank.writeMotor(0,0);
      iArm.turnTo(180,169,90,150);    // 將手臂平放
      delay(500);
      iArm.turnTo(180,169,90,80);     // 鬆開抓爪護住球, 避免球亂彈
      delay(500);
      iArm.turnTo(180,169,90,22);     // 抓爪全開
      delay(500);
      state = ST_STOP;
      iVision.findColor(ballColor, 6);// 恢復成辨識球體的顏色
      iArm.turnTo(20,49,28,180);       // 高舉手臂擺定位
      break;
  }
  delay(1); // 避免跑太快
}

