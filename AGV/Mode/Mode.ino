 #include <Servo.h>

#define RED_LED 36
#define YELLOW_LED 37
#define GREEN_LED 38
#define TONE_PIN 39

byte LedMode = 0;
byte OldLedMode = 0;

byte x[30];
byte LineCMD[8]={0x01,0x03,0x00,0x28,0x00,0x01,0x04,0x02};//循線命令
Servo RC1;  
Servo RC2;
Servo RC3;
Servo RC4;
int times=0;
String MoveData = "";                            // 接收訊息
char mode='1';// 手自動模式變數 

long previousTime = 0;  // 用來保存前一次狀態的時間
long interval = 1000;   // 讀取間隔時間，單位為毫秒(miliseconds)
unsigned long currentTime;

int times = 0;
String MoveData = ""; // 接收訊息
char mode = '1';      // 手自動模式變數

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial1.begin(115200);
  RC1.attach(6,1000,2000); 
  RC2.attach(7,1000,2000);  
  RC3.attach(8,1000,2000);
  RC4.attach(9,1000,2000);
}

void loop() 
{ 
  
  while (mode=='0') // 手動模式
  {
    if (OldLedMode==4)
    {
      LedMode = 0;
    }
    else
    {
      LedMode = 4;
    }
    Serial.println("aaa");
    RobotCMD();//透過Pi網頁遙控用,去UNO暫存器取值並拆解後,回傳控制車子的命令
  }
    RobotCMD();
    Line();//AGV自主循線用,接收AGV reader的傳回值,並發送循線命令
    Serial.println(x[4]);
    switch(x[4])
    {
      case 60: //正中間3456號
        Moveforward();
        break;
      case 63: //右邊有路123456號測到
        turnright();
        break;
      case 252://左邊有路345678號測到
        turnleft();
        break;
      case 192://左邊87感測到
        Moveforwardl1();
        break;
      case 128://左邊8號測到
        Moveforwardl2();
        break;
      case 3://右邊21號測到
        Moveforwardr1();
        break;
      case 30://右邊2345號測到
        Moveforwardr3();
        break;
      case 62://中間偏右23456號
        Moveforwardr();
        break; 
      case 124://中間偏左34567號
        Moveforwardl();
        break; 
      case 120://中間偏左4567號
        Moveforwardl3();
        break;
      case 1://右邊1號測到
        Moveforwardr2();
        break;
      case 224://左邊876號測到
        Moveforwardl3();
        break;
      case 7://右邊321號測到
        Moveforwardr3();
        break;      
      case 0://無偵測到
        Moveleft();
        if((x[4]==128) || (x[4]==192) || (x[4]==224)){
          Stop();
          delay(300);
          }
          break;
          }
  if (currentTime - previousTime > interval)
  {
    LED(LedMode);
    OldLedMode = LedMode;
    previousTime = currentTime;
  }
   
  delay(100);
}

void Line()//發送循線命令
{
  Serial.println("sss");
  for (byte z=0; z<sizeof(LineCMD); z++)
  {
    Serial1.write(LineCMD[z]);
  }
  delay(20); 
  LineCMDValue();//接收循線讀值 
}

void LineCMDValue()//接收循線讀值 
{
  Serial.readBytes(x, 8);
  Serial.println(x[4]);//顯示循線讀值 
}
void RobotCMD(){
  if(Serial.available() > 0){                                 //確認暫存區是否有資料
    for(int i=0;i<100;i++){
      if(Serial.available() > 0){
        char dataByte = Serial.read();                        //讀取暫存區資料
        if(dataByte == '#'){
          RobotCommand(getValue(MoveData,'_',0));              //拆解資料並回傳控制     
          MoveData="";
          return;
        }
        MoveData += dataByte;
        break;
      }
      break;
    }
  }
}

void RobotCommand(String command) {
  if (command =="a") {     // 車體前進
    Moveforward();  
  }
  if (command == "b") {     // 車體後退                                          
    Moveback();  
  } 
  
  if (command == "c") {     // 車體左轉                                                
    Moveleft();    
  } 
  if (command == "d") {     // 車體右轉                                                
    Moveright();    
  } 
  if (command == "e") {     // 車體停止                                           
    Stop(); 
  }
  if (command == "u") {     // 上升
      /* code */
  }
  if (command == "i") {     // 停止
      /* code */
  }
  if (command == "o") {     // 下降
      /* code */
  }
  if (command == "f") {     // 手動模式                                          
    mode = '0';
  }
  if (command == "g") {     // 自動模式                                           
    mode = '1';
  }
}

String getValue(String data, char separator, int index){                  //拆解資料
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void Moveforwardr(){//修右
  RC1.write(120);
  RC3.write(120);
  RC2.write(69);
  RC4.write(69);
}
void Moveforwardr3(){//中修右
  RC1.write(120);
  RC3.write(120);
  RC2.write(72);
  RC4.write(72);
}
void Moveforwardr1(){//中修右
  RC1.write(120);
  RC3.write(120);
  RC2.write(79);
  RC4.write(79);
}
void Moveforwardr2(){//大修右
  RC1.write(120);
  RC3.write(120);
  RC2.write(120);
  RC4.write(120);
}
void Moveforwardl(){//修左
  RC1.write(113);
  RC3.write(113);
  RC2.write(63);
  RC4.write(63);
}
void Moveforwardl3(){//小修左
  RC1.write(110);
  RC3.write(110);
  RC2.write(63);
  RC4.write(63);
}
void Moveforwardl1(){//中修左
  RC1.write(103);
  RC3.write(103);
  RC2.write(63);
  RC4.write(63);
}
void Moveforwardl2(){//大修左
  RC1.write(63);
  RC3.write(63);
  RC2.write(63);
  RC4.write(63);
}



void Moveforward() {  //前進
  RC1.write(120);
  RC3.write(120);
  RC2.write(63);
  RC4.write(63);
}
void Moveback() {  //後退
  RC1.write(63);
  RC3.write(63);
  RC2.write(120);
  RC4.write(120);
}
void Moveright() {   //原地向右旋轉
  RC2.write(120);
  RC1.write(120);
  RC3.write(120);       
  RC4.write(120);          
}
void turnright(){  //遇到右側有路，右轉動作 
  Stop();
  delay(300);
  Moveforward();
  delay(1000);
  Moveright();
  delay(3400);
}
void turnleft(){  //遇到左側有路，左轉動作
  Stop();
  delay(300);
  Moveforward();
  delay(1000);
  Moveleft();
  delay(3400);
}
//以上兩個動作是先做暫停，讓車體不要移動，前進1秒，讓旋轉中心到線上(以利轉彎後偵測到磁帶時，車體與磁帶較平行)
//原地旋轉3.4秒到磁帶上面
void Moveleft() {  //原地向左旋轉
  RC1.write(63);
  RC3.write(63);
  RC2.write(63);
  RC4.write(63);
}
void Stop() {    //車機停止
  RC1.write(90); 
  RC2.write(90); 
  RC3.write(90); 
  RC4.write(90);
}

void DigitalWrite(uint8_t pinNumber, uint8_t status)
{
  pinMode(pinNumber, OUTPUT);
  digitalWrite(pinNumber, status);
}

void LED(byte value)
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
    DigitalWrite(TONE_PIN, LOW);
    break;
  }
}