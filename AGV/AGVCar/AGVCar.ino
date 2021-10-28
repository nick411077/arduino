#include <Servo.h>

//#define DEBUG //除錯測試模式
//#define Manual //發送返回模式

// 提示燈
#define RED_LED 36
#define YELLOW_LED 37
#define GREEN_LED 38
#define TONE_PIN 39
byte LedMode = 0;
byte OldLedMode = 0;
// ------

// 燈號狀態更新
long previousTime = 0;  // 用來保存前一次狀態的時間
long interval = 1000;   // 讀取間隔時間，單位為毫秒(miliseconds)
unsigned long currentTime;
// -----------

//巡線宣告
byte LineData[15];
byte LineAutoSetup[8]={0x01, 0x06, 0x00, 0x2F, 0x00, 0x05, 0x78, 0x00};
byte LineManualSetup[8]= {0x01, 0x06, 0x00, 0x2F, 0x00, 0x00, 0xB8, 0x03};//自手動切換有問題就放棄
byte LineCMD[8] = {0x01, 0x03, 0x00, 0x28, 0x00, 0x01, 0x04, 0x02}; 
//-------

// 讀卡宣告
byte CardReaderTemporaryStorage = 0;
byte rfidData[15];
byte rfidSetup[8]={0x01, 0x06, 0x00, 0x04, 0x00, 0x01, 0x09, 0xCB};
byte rfidCMD[8] = {0x01, 0x03, 0x00, 0x06, 0x00, 0x04, 0xA4, 0x08}; //rfidD命令
// -------

// pi 命令
byte RobotCMD[5] = {0x25, 0x05, 0x00, 0x00, 0xFF};
String MoveData = ""; // 接收pi訊息
byte mode = 0;     // 手自動模式變數
byte oldmode;
// ---

// 馬達宣告
Servo RC1;
Servo RC2;
Servo RC3;
Servo RC4;
// -------

// 升降機宣告
Servo RC5;
// ---------

HardwareSerial LineSerial(Serial1);//RFID
HardwareSerial RFIDSerial(Serial2);//Line

void setup()
{
  // 通道
  Serial.begin(115200);//pi
  LineSerial.begin(115200);//Line
  RFIDSerial.begin(115200);//RFID
  // ----

  // AGV自動模式上傳設置
  LineModeSetup();
  RFIDModeSetup();
  
  // 馬達初始化
  RC1.attach(6, 1000, 2000);
  RC2.attach(7, 1000, 2000);
  RC3.attach(8, 1000, 2000);
  RC4.attach(9, 1000, 2000);
  // ---------

  // 升降機初始化
  RC5.attach(10, 1000, 2000);
  // -----------
}

void loop()
{
  currentTime = millis(); // 獲取時間用與判斷是否更新
  oldmode = mode;
  RobotRead(); // 讀取pi訊息
  #ifdef Manual
  LineModeSetup(mode,oldmode);//自手動切換
  #endif // Manual
  // 手動
  if (mode == 0)
  {
    // 提示燈
    if (OldLedMode==2)
    {
      LedMode = 0;
    }
    else
    {
      LedMode = 2;
    }
  }else { //自動模式
    AutoMode();
  }

  // 更新狀態
  if (currentTime - previousTime > interval)
  {
    RGYLED(LedMode);
    RobotWrite();
    OldLedMode = LedMode;
    previousTime = currentTime;
  }
   
  delay(100);
}


// 巡線模式
void AutoMode(){
    LedMode = 4; //燈號
    byte CardReader = RFIDRead(); // 讀取卡號
    LineRead(); //AGV自主循線用,接收AGV reader的傳回值,並發送循線命令
    Auto(); //巡線
    if (CardReader == mode){
        CardReaderTemporaryStorage = 0;
        mode = 0;
        Stop();
    }else if (CardReader == 0)
    {
        LineRead(); //AGV自主循線用,接收AGV reader的傳回值,並發送循線命令
        Auto(); //巡線
    }else if (CardReader < mode)
    {
        if(CardReaderTemporaryStorage == 0){
            CardReaderTemporaryStorage = CardReader;//放入暫存
            LineRead(); //AGV自主循線用,接收AGV reader的傳回值,並發送循線命令
            Auto(); //巡線
        }else if (mode > CardReaderTemporaryStorage && CardReaderTemporaryStorage >= CardReader )
        {
            CardReaderTemporaryStorage = CardReader;//放入暫存
            LineRead(); //AGV自主循線用,接收AGV reader的傳回值,並發送循線命令
            Auto(); //巡線
        }else{
            CardReaderTemporaryStorage = CardReader;
            Moveleft();
            delay(3000);
        }

    }else if (CardReader > mode)
    {
        if(CardReaderTemporaryStorage == 0){
            CardReaderTemporaryStorage = CardReader;//放入暫存
            LineRead(); //AGV自主循線用,接收AGV reader的傳回值,並發送循線命令
            Auto(); //巡線
        }else if (mode < CardReaderTemporaryStorage && CardReaderTemporaryStorage <= CardReader )
        {
            CardReaderTemporaryStorage = CardReader;//放入暫存
            LineRead(); //AGV自主循線用,接收AGV reader的傳回值,並發送循線命令
            Auto(); //巡線
        }else{
            CardReaderTemporaryStorage = CardReader;
            Moveleft();
            delay(3000);
        }
    }
    
}
// -------

byte LineModeSetup(byte Mode =0,byte OldMode =1)//Line自手模式上傳設置 因為可能會導致暫存區爆滿
{
  byte x[15];
  if (Mode == 0 && OldMode == 1) // 手動模式
  {
    LineSerial.write(LineManualSetup, sizeof(LineManualSetup));
    LineSerial.flush();
#ifdef DEBUG
    if (LineSerial.available() > 0)
    {
      LineSerial.readBytes(x, 15);
      for (byte i = 0; i < sizeof(x); i++)
      {
        Serial.print(x[i], HEX);
        Serial.print(",");
      }
      Serial.write(0xFF);
      Serial.println();
    }
#endif
    while (LineSerial.read() >= 0){}
  }
  else if (Mode == 1 && OldMode == 0) //自動模式
  {
    LineSerial.write(LineAutoSetup, sizeof(LineAutoSetup));
    LineSerial.flush();
#ifdef DEBUG
    if (LineSerial.available() > 0)
    {
      LineSerial.readBytes(x, 15);
      for (byte i = 0; i < sizeof(x); i++)
      {
        Serial.print(x[i], HEX);
        Serial.print(",");
      }
      Serial.write(0xFF);
      Serial.println();
    }
#endif
    while (LineSerial.read() >= 0){}
  }
}

byte RFIDModeSetup()//RFID自動模式上傳設置
{
  byte x[15];
  RFIDSerial.write(rfidSetup,sizeof(rfidSetup));
  RFIDSerial.flush();
  
  if (RFIDSerial.available() > 0) 
  {
    #ifdef DEBUG
    RFIDSerial.readBytes(x, 15);
    for (byte i = 0; i < sizeof(x); i++)
    {
      Serial.print(x[i], HEX);
      Serial.print(",");
    }
    Serial.write(0xFF);
    Serial.println();
    #endif
    while (RFIDSerial.read() >= 0){}
  }
  
  
}

byte LineRead() //接收循線讀值
{
  #ifdef Manual
  LineSerial.write(LineCMD,sizeof(LineCMD));
  LineSerial.flush();
  #endif
  if (LineSerial.available() > 0) 
  {
    LineSerial.readBytes(LineData, 15);
    #ifdef DEBUG
    for (byte i = 0; i < sizeof(LineData); i++)
    {
      Serial.print(LineData[i], HEX);
      Serial.print(",");
    }
    Serial.write(0xFF);
    Serial.println();
    #endif
  }
  return LineData[4];
}

byte RFIDRead()//接收RFID讀值 
{
  if (RFIDSerial.available() > 0) 
  {
    RFIDSerial.readBytes(rfidData, 15);
    #ifdef DEBUG
    for (byte i = 0; i < sizeof(rfidData); i++)
    {
      Serial.print(rfidData[i], HEX);
      Serial.print(",");
    }
    Serial.write(0xFF);
    Serial.println();
    #endif
    return rfidData[5];
  }
  return 0;
}

void RobotRead(){ // 讀取樹梅派訊息
  if (Serial.available() > 0)
  { //確認暫存區是否有資料
    for (int i = 0; i < 100; i++)
    {
      if (Serial.available() > 0)
      {
        char dataByte = Serial.read(); //讀取暫存區資料
        if (dataByte == '#')
        {
          RobotCommand(getValue(MoveData, '_', 0)); //拆解資料並回傳控制
          MoveData = "";
          return;
        }
        MoveData += dataByte;
        break;
      }
      break;
    }
  }
}

void RobotWrite()
{
  byte Robot = 0;
  RobotCMD[2] = 0;
  if (mode == 1){RobotCMD[2] =+ 1;}
  RobotCMD[3] = LedMode;
  Serial.write(RobotCMD,sizeof(RobotCMD));
  Serial.flush();
}

// 客戶端控制
void RobotCommand(String command)
{
  if (command == "a")
  { // 車體前進
    Moveforward();
  }
  if (command == "b")
  { // 車體後退
    Moveback();
  }
  if (command == "c")
  { // 車體左轉
    Moveleft();
  }
  if (command == "d")
  { // 車體右轉
    Moveright();
  }
  if (command == "e")
  { // 車體停止
    Stop();
  }
  if (command == "u")
  { // 上升
    /* code */
  }
  if (command == "i")
  { // 停止
    /* code */
  }
  if (command == "o")
  { // 下降
    /* code */
  }
  if (command == "f")
  { // 手動模式
    mode = 0;
  }
  // 自動模式
  if (command == "1")
  { 
    mode = 1;
  }else if (command == "2")
  {
      mode = 2;
  }
  else if (command == "3")
  {
      mode = 3;
  }else if (command == "4")
  {
      mode = 4;
  }
  //-------------
    
}

String getValue(String data, char separator, int index)
{ //拆解資料
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// 巡線
void Auto()
{
  switch (LineData[4])
  {
  case 60: //正中間3456號
    Moveforward();
    break;
  case 63: //右邊有路123456號測到
    turnright();
    break;
  case 252: //左邊有路345678號測到
    turnleft();
    break;
  case 192: //左邊87感測到
    Moveforwardl1();
    break;
  case 128: //左邊8號測到
    Moveforwardl2();
    break;
  case 3: //右邊21號測到
    Moveforwardr1();
    break;
  case 30: //右邊2345號測到
    Moveforwardr3();
    break;
  case 62: //中間偏右23456號
    Moveforwardr();
    break;
  case 124: //中間偏左34567號
    Moveforwardl();
    break;
  case 120: //中間偏左4567號
    Moveforwardl3();
    break;
  case 1: //右邊1號測到
    Moveforwardr2();
    break;
  case 224: //左邊876號測到
    Moveforwardl3();
    break;
  case 7: //右邊321號測到
    Moveforwardr3();
    break;
  case 0: //無偵測到
    Moveleft();
    if ((LineData[4] == 128) || (LineData[4] == 192) || (LineData[4] == 224))
    {
      Stop();
      delay(300);
    }
    break;
  }
}

// 巡線修正
void Moveforwardr()
{ //修右
  RC1.write(120);
  RC3.write(120);
  RC2.write(69);
  RC4.write(69);
}
void Moveforwardr3()
{ //中修右
  RC1.write(120);
  RC3.write(120);
  RC2.write(72);
  RC4.write(72);
}
void Moveforwardr1()
{ //中修右
  RC1.write(120);
  RC3.write(120);
  RC2.write(79);
  RC4.write(79);
}
void Moveforwardr2()
{ //大修右
  RC1.write(120);
  RC3.write(120);
  RC2.write(120);
  RC4.write(120);
}
void Moveforwardl()
{ //修左
  RC1.write(113);
  RC3.write(113);
  RC2.write(63);
  RC4.write(63);
}
void Moveforwardl3()
{ //小修左
  RC1.write(110);
  RC3.write(110);
  RC2.write(63);
  RC4.write(63);
}
void Moveforwardl1()
{ //中修左
  RC1.write(103);
  RC3.write(103);
  RC2.write(63);
  RC4.write(63);
}
void Moveforwardl2()
{ //大修左
  RC1.write(63);
  RC3.write(63);
  RC2.write(63);
  RC4.write(63);
}

void Moveforward()
{ //前進
  RC1.write(120);
  RC3.write(120);
  RC2.write(63);
  RC4.write(63);
}
void Moveback()
{ //後退
  RC1.write(63);
  RC3.write(63);
  RC2.write(120);
  RC4.write(120);
}
void Moveright()
{ //原地向右旋轉
  RC2.write(120);
  RC1.write(120);
  RC3.write(120);
  RC4.write(120);
}
void turnright()
{ //遇到右側有路，右轉動作
  Stop();
  delay(300);
  Moveforward();
  delay(1000);
  Moveright();
  delay(3400);
}
void turnleft()
{ //遇到左側有路，左轉動作
  Stop();
  delay(300);
  Moveforward();
  delay(1000);
  Moveleft();
  delay(3400);
}
//以上兩個動作是先做暫停，讓車體不要移動，前進1秒，讓旋轉中心到線上(以利轉彎後偵測到磁帶時，車體與磁帶較平行)
//原地旋轉3.4秒到磁帶上面
void Moveleft()
{ //原地向左旋轉
  RC1.write(63);
  RC3.write(63);
  RC2.write(63);
  RC4.write(63);
}
void Stop()
{ //車機停止
  RC1.write(90);
  RC2.write(90);
  RC3.write(90);
  RC4.write(90);
} 
// ----

void DigitalWrite(uint8_t pinNumber, uint8_t status)
{
  pinMode(pinNumber, OUTPUT);
  digitalWrite(pinNumber, status);
}

// 燈號
void RGYLED(byte value)
{
  switch (value)
  {
  case 0: //全熄
    DigitalWrite(GREEN_LED, LOW);
    DigitalWrite(YELLOW_LED, LOW);
    DigitalWrite(RED_LED, LOW);
    DigitalWrite(TONE_PIN, LOW);
    break;
  case 1: //紅燈
    DigitalWrite(GREEN_LED, LOW);
    DigitalWrite(YELLOW_LED, LOW);
    DigitalWrite(RED_LED, HIGH);
    DigitalWrite(TONE_PIN, HIGH);
    break;
  case 2: //黃燈
    DigitalWrite(GREEN_LED, LOW);
    DigitalWrite(YELLOW_LED, HIGH);
    DigitalWrite(RED_LED, LOW);
    break;
  case 3: //紅黃燈
    DigitalWrite(GREEN_LED, LOW);
    DigitalWrite(YELLOW_LED, HIGH);
    DigitalWrite(RED_LED, HIGH);
    break;
  case 4: //綠燈
    DigitalWrite(GREEN_LED, HIGH);
    DigitalWrite(YELLOW_LED, LOW);
    DigitalWrite(RED_LED, LOW);
    break;
  case 5: //綠紅燈
    DigitalWrite(GREEN_LED, HIGH);
    DigitalWrite(YELLOW_LED, LOW);
    DigitalWrite(RED_LED, HIGH);
    break;
  case 6: //綠黃燈
    DigitalWrite(GREEN_LED, HIGH);
    DigitalWrite(YELLOW_LED, HIGH);
    DigitalWrite(RED_LED, LOW);
    break;
  case 7: //全亮燈
    DigitalWrite(GREEN_LED, HIGH);
    DigitalWrite(YELLOW_LED, HIGH);
    DigitalWrite(RED_LED, HIGH);
    break;
  case 8: //嗡鳴器
    DigitalWrite(TONE_PIN, HIGH);
    break;
  }
}
// ----
