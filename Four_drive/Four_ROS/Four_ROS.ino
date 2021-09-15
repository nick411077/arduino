#include <WiFi.h>
#include <Servo.h>             //伺服馬達函數庫 https://github.com/RoboticsBrno/ServoESP32
#include <AccelStepper.h>      //步進馬達函數庫
#include <Stepper.h>           //步進馬達函數庫
#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Float64.h>
#include <rosserial_arduino/Adc.h>
#include <geometry_msgs/Twist.h>

//#define DEBUG //除錯測試模式

//步進設置
#define ENB 5
#define DIR 18
#define PUL 19 //Arduino給驅動器的腳位
AccelStepper stepper(1, PUL, DIR);
int MaxSpeed = 2000;    //最高速 空載2000
int Acceleration = 200; //加速度 空載200
//int Max = 1300;         //1:80= 16000轉

//Servo設置
#define RCFPin 32
#define RCBPin 33
#define RCLPin 25
#define RCRPin 26
Servo RCF;
Servo RCB;
Servo RCL;
Servo RCR;

//雙核運行
TaskHandle_t Task1;

//超音波設置
#define TRIG1 22
#define ECHO1 23
#define TRIG2 16
#define ECHO2 17

int counts;
int counts_run = 10;//超音波確認次數
int Distance = 40;
uint8_t UCstatus = 1; //為了loop不要重複運行設定狀態變數只運行一次

//碰撞感應器
#define SL 36
#define SR 39
char released[2] = {0,0}; //左右開關狀態
uint8_t Stepstatus = 1; //為了loop不要重複運行設定狀態變數只運行一次

//直流馬達加速度配置
int val = 90; //加速度變數
int saveval = 90; //加速度變數
int powertime = 20;//delay時間
uint8_t DCstatus = 0; //為了loop不要重複運行設定狀態變數只運行一次

//將 String轉換成int 
int StepValue = 5;
int CarValue = 2;
int PowValue = 45;
int StopValue = 0;

//wifi賬號密碼
const char* ssid = "罐頭new"; //ssid
const char* password = "nick520301"; //password

IPAddress server(192, 168, 1, 116);//ros伺服器
IPAddress ip_address;//ip地址
int status = WL_IDLE_STATUS;//它是調用WiFi .begin()時分配的臨時狀態，並保持活動狀態直到嘗試次數到期

WiFiClient client;//創建一個客戶端，該客戶端可以連接到client.connect() 中定義的指定 Internet IP 地址和端口。

//連接ros伺服器
class WiFiHardware 
{
  public:
  WiFiHardware() {};

  void init() 
  {
    // 連接到ROS伺服器
    client.connect(server, 11411);
  }

  int read() 
  {
    return client.read(); //讀取ROS伺服器字節
  }

  void write(uint8_t* data, int length) 
  {
    // 將數據寫錄
    for(int i=0; i<length; i++)
      client.write(data[i]);
  }

  unsigned long time()
  {
     return millis(); //回傳扳子執行時間
  }
};

void setupWiFi()
{
  WiFi.begin(ssid, password);//連接wifi(賬號,密碼)
  Serial.print("\nConnecting to "); Serial.println(ssid);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
  if(i == 21){
    Serial.print("Could not connect to"); Serial.println(ssid);
    while(1) delay(500);
  }
  Serial.print("Ready! Use ");
  Serial.print(WiFi.localIP());
  Serial.println(" to access client");
}

std_msgs::String str_msg;
ros::NodeHandle_<WiFiHardware> nh;//創建節點
ros::Publisher chatter("chatter", &str_msg);
ros::Subscriber<std_msgs::String> sub("message", &chatterCallback);


void chatterCallback(const std_msgs::String& msg) 
{
  String Msg = msg.data;
  int MsgNumber;
  if (Msg == "w")
  {
    CarValue = 1;
    if (UCstatus == 0)
    {
      DCstatus = 0;
    }
    else
    {
      DCstatus = 1;
      RCR.write(10); //釋放煞車
      RCL.write(10); //釋放煞車
      StopValue = 0;
    }
    chatter.publish( &msg );
  }
  else if (Msg == "s")
  {
    CarValue = 2;
    DCstatus = 1;
    chatter.publish( &msg );
  }
  else if (Msg == "x")
  {
    CarValue = 3;
    DCstatus = 1;
    RCR.write(10);//釋放煞車
    RCL.write(10);//釋放煞車
    StopValue = 0;
    chatter.publish( &msg );
  }
  else if (Msg == "u")
  {
    StepValue= 1;
    Step(StepValue);
    chatter.publish( &msg );
  }
  else if (Msg == "i")
  {
    StepValue= 2;
    Step(StepValue);
    chatter.publish( &msg );
  }
  else if (Msg == "o")
  {
    StepValue= 3;
    Step(StepValue);
    chatter.publish( &msg );
  }
  else if (Msg == "space")
  {
    StopValue = 1;
    chatter.publish( &msg );
  }
  else if (Msg == ",")
  {
    if (PowValue != 0)
    {
      PowValue -= 5;
    }
    Serial.println(PowValue);
    chatter.publish( &msg );
  }
  else if (Msg == ".")
  {
    if (PowValue != 90)
    {
      PowValue += 5;
    }
    Serial.println(PowValue);
    chatter.publish( &msg );
  }
}

void setup() 
{
  Serial.begin(115200);
  RCF.attach(RCFPin,1,0,180,1000,2000);
  RCB.attach(RCBPin,2,0,180,1000,2000);
  RCL.attach(RCLPin,3);
  RCR.attach(RCRPin,4);
  RCF.write(90);
  RCB.write(90);
  pinMode(PUL, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(SL, INPUT);
  pinMode(SR, INPUT);
  digitalWrite(DIR, HIGH);
  digitalWrite(ENB, LOW);
  stepper.setEnablePin(ENB);
  stepper.disableOutputs();
  stepper.setMaxSpeed(MaxSpeed);
  stepper.setAcceleration(Acceleration);
  stepper.setCurrentPosition(0);
  xTaskCreatePinnedToCore(//雙核運行參數
             Task1code, /* Task function. */
             "Task1",   /* name of task. */
             10000,     /* Stack size of task */
             NULL,      /* parameter of the task */
             1,         /* priority of the task */
             &Task1,    /* Task handle to keep track of created task */
             0);        /* pin task to core 0 */
  setupWiFi(); // 連接wifi
  delay(2000);

  nh.initNode(); //節點初始化
  nh.advertise(chatter); // 發布初始化
  nh.subscribe(sub); //添加主節點
}

void loop(){
    nh.spinOnce();
    counts++;
  if (ButtonPressed(SL, 0) == 1 || ButtonPressed(SR, 1) == 1) //如果左或右碰到微動開關離即停止
  {
    if (StepValue == 1)
    {
      Step(2);
      StepValue = 0;
    }
  }
  else
  {
    StepValue = 1;
  }
#ifdef DEBUG
  Serial.print("計數：");
  Serial.println(counts);
  //Serial.print("超音波1：");
  //Serial.println(Ultrasound(TRIG1,ECHO1));
  //Serial.print("超音波2：");
  //Serial.println(Ultrasound(TRIG2,ECHO2));
  
  for (size_t i = 0; i < sizeof(released); i++)
  {
    Serial.print(released[i], HEX);
  }
  Serial.println();
  #endif
  if (DCstatus == 1)//讀取狀態
  {
    moto(CarValue, PowValue);
  }
  if (StopValue == 1)//如果接收P檔或超音波小於40cm就停止
  {
    STOP();
  }
  if (Ultrasound(TRIG1,ECHO1) <= Distance) //超音波小於40公分停止
  {
    if (counts == counts_run)//達到確認次數及停止
    {
      Serial.println("counts" + String(counts_run) + "run done");
      if (UCstatus == 1)//判別使否有停止過了
      {
        Serial.println("UC STOP");
        STOP();
        UCstatus = 0;
      }
    }
  }
  else //沒有達到40公分內重新計數
  {
    #ifdef DEBUG
    Serial.println("counts restart");
    #endif
    counts = 0;
    UCstatus = 1;
  }
}

void moto(int Value, int Power) //直流馬達加速度
{
  while (Value == 1)//前進
  {
    if (saveval<=(90+Power))//如果後退或停止的話進行減到前進
    {
      RCF.write(val);
      RCB.write(val);
      #ifdef DEBUG
      Serial.print("SetValue:");
      Serial.println(RCF.read());
      Serial.println(RCB.read());
      Serial.println(val);
      #endif
      delay(powertime);
    }
    if (val == (90+Power))
    {
      saveval=val;
      Serial.println("close1");
      break;
    }
    val++;
  }
  while (Value == 2)//停止
  {
    if (val == 90)
    {
      saveval=val;
      Serial.println("close2");
      break;
    }
    if (saveval<90)//如果狀態後退加到停止
    {
      RCF.write(val-10);
      RCB.write(val-10);
      #ifdef DEBUG
      Serial.print("SetValue:");
      Serial.println(RCF.read());
      Serial.println(RCB.read());
      Serial.println(val);
      #endif
      delay(powertime);
      val ++;
    }
    if (saveval>90)//如果狀態前進減到停止
    {
      RCF.write(val);
      RCB.write(val);
      #ifdef DEBUG
      Serial.print("SetValue:");
      Serial.println(RCF.read());
      Serial.println(RCB.read());
      Serial.println(val);
      #endif
      delay(powertime);
      val --;
    }
  }
  while (Value == 3)//後退
  {
    if (saveval>=(90 - Power))//如果前進或停止的話進行減到後退
    {
      RCF.write(val-10);
      RCB.write(val-10);
      #ifdef DEBUG
      Serial.print("SetValue:");
      Serial.println(RCF.read());
      Serial.println(RCB.read());
      Serial.println(val);
      #endif
      delay(powertime);
    }
    if (val == (90 - Power))
    {
      saveval=val;
      Serial.println("close3");
      break;
    }
    val --;
  }
  DCstatus = 0;//更新狀態
}

void Step(int Step)
{
  switch (Step)
  {
  case 1://左轉
    stepper.setSpeed(200);//設定速度
    Serial.println("Step1");
    break;
  case 2://停
    stepper.setSpeed(0);
    Serial.println("Step2");
    break;
  case 3://右轉
    stepper.setSpeed(-200);
    Serial.println("Step3");
    break;
  }
}

void STOP()//P檔煞車動作
{
  Serial.println("STOP run");
  moto(2,90);
  RCR.write(150);//加上煞車
  RCL.write(150);
  Serial.println(RCR.read());
  Serial.println(RCL.read());
  StopValue = 0;
}

int Ultrasound(int trigPin, int echoPin)//超音波
{
  long duration;
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(1);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  duration = duration / 28 / 2;
  if (duration > 80) return 100;
  return duration;
}

boolean ButtonPressed(uint8_t pin, int numder)//微動開關按下反應
{
  int i = 0;
  if (digitalRead(pin) == HIGH)
  {
    i = 1;
    if (released[numder] < 1)
    {
      released[numder] = 1;
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    i = 0;
    released[numder] = 0;
    return false;
  }
}

void Task1code(void *pvParameters)//雙核運行
{
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;)
  {
    stepper.runSpeed();//步進馬達運行防止loop進入迴圈導致停止運行
    delay(1);
  }
}