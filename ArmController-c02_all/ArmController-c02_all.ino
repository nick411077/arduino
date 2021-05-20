#include <WiFi.h>
#include <WebServer.h>

//309行ESP32不用設定I2C腳位設定

#include <Servo.h>

const char* ssid = "Lavender";     // 為ESP8266命名WiFi名稱並輸入""內
const char* password = "12345678"; // 為ESP8266命名WiFi密碼並輸入""內

WebServer server(80);

// 以下是PCA9685的宣告
#include <Wire.h> //309行ESP32不用設定I2C腳位設定
#include <Adafruit_PWMServoDriver.h>
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  700 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define INIT_POS 350 // 手臂初始位置

// our servo # counter
uint8_t servonum = 1;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
int pulse0=INIT_POS,pulse10=INIT_POS,pulse1=INIT_POS,pulse2=INIT_POS,pulse3=INIT_POS,pulse4=INIT_POS,pulse5=INIT_POS,pulse6=INIT_POS,pulse7=INIT_POS;

void DigitalWrite(int pinNumber, boolean status)
{
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, status);
}
void AnalogWrite(int pinNumber, int LEDChannel, int value)
{
  ledcSetup(LEDChannel, 5000, 8);
  ledcAttachPin(pinNumber, LEDChannel);
  ledcWrite(LEDChannel, value);
}

void handleCar()
{
  String message = "";
  int BtnValue = 0;
  for (uint8_t i = 0; i < server.args(); i++) //拆字
  {
    if (server.argName(i) == "a")
    {
      String s = server.arg(i);
      BtnValue = s.toInt();
    }
    Serial.println(server.argName(i) + ": " + server.arg(i) + "\n");
  }

  switch (BtnValue) //車輛控制
  {
  case 1: // 左前
    DigitalWrite(18, HIGH);
    DigitalWrite(5, LOW);
    AnalogWrite(25, 1, 155);
    DigitalWrite(17, HIGH);
    DigitalWrite(16, LOW);
    AnalogWrite(26, 2, 255);
    break;
  case 2: // 前
    DigitalWrite(18, HIGH);
    DigitalWrite(5, LOW);
    AnalogWrite(25, 1, 255);
    DigitalWrite(17, HIGH);
    DigitalWrite(16, LOW);
    AnalogWrite(26, 2, 255);
    break;
  case 3: // 右前
    DigitalWrite(18, HIGH);
    DigitalWrite(5, LOW);
    AnalogWrite(25, 1, 255);
    DigitalWrite(17, HIGH);
    DigitalWrite(16, LOW);
    AnalogWrite(26, 2, 155);
    break;
  case 4: // 左
    DigitalWrite(18, LOW);
    DigitalWrite(5, HIGH);
    AnalogWrite(25, 1, 255);
    DigitalWrite(17, HIGH);
    DigitalWrite(16, LOW);
    AnalogWrite(26, 2, 255);
    break;
  case 5: // 停
    DigitalWrite(18, HIGH);
    DigitalWrite(5, HIGH);
    AnalogWrite(25, 1, 255);
    DigitalWrite(17, HIGH);
    DigitalWrite(16, HIGH);
    AnalogWrite(26, 2, 255);
    break;
  case 6: // 右
    DigitalWrite(18, HIGH);
    DigitalWrite(5, LOW);
    AnalogWrite(25, 1, 255);
    DigitalWrite(17, LOW);
    DigitalWrite(16, HIGH);
    AnalogWrite(26, 2, 255);
    break;
  case 7: // 左後
    DigitalWrite(18, LOW);
    DigitalWrite(5, HIGH);
    AnalogWrite(25, 1, 155);
    DigitalWrite(17, LOW);
    DigitalWrite(16, HIGH);
    AnalogWrite(26, 2, 255);
    break;
  case 8: // 後
    DigitalWrite(18, LOW);
    DigitalWrite(5, HIGH);
    AnalogWrite(25, 1, 255);
    DigitalWrite(17, LOW);
    DigitalWrite(16, HIGH);
    AnalogWrite(26, 2, 255);
    break;
  case 9: // 右後
    DigitalWrite(18, LOW);
    DigitalWrite(5, HIGH);
    AnalogWrite(25, 1, 255);
    DigitalWrite(17, LOW);
    DigitalWrite(16, HIGH);
    AnalogWrite(26, 2, 155);
    break;
    // 手臂部分
  case 15:                      // 各軸回原點
    pwm.setPWM(0, 0, INIT_POS); //夾爪, 貼紙編號7
    delay(50);
    pwm.setPWM(1, 0, INIT_POS); //軸1
    delay(50);
    pwm.setPWM(2, 0, INIT_POS); //軸2  編號11  運動需反向
    delay(50);
    pwm.setPWM(3, 0, INIT_POS); //軸2  編號12  運動需反向
    delay(50);
    pwm.setPWM(4, 0, INIT_POS);
    delay(50);
    pwm.setPWM(5, 0, INIT_POS);
    delay(50);
    pwm.setPWM(6, 0, INIT_POS);
    delay(50);
    pwm.setPWM(7, 0, INIT_POS);
    delay(50);
    break;
  case 0: // (夾爪) 夾
    pulse0 += 10;
    if (pulse0 > SERVOMAX)
      break;
    pwm.setPWM(0, 0, pulse0);
    delay(100);
    break;
  case 10: // (夾爪) 放
    pulse0 -= 10;
    if (pulse0 < SERVOMIN)
      break;
    pwm.setPWM(0, 0, pulse0);
    delay(100);
    break;
  case 11: // Arm point 1 順
    pulse1 += 10;
    if (pulse1 > SERVOMAX)
      break;
    pwm.setPWM(1, 0, pulse1);
    delay(100);
    break;
  case 12: // Arm point 1 逆
    pulse1 -= 10;
    if (pulse1 < SERVOMIN)
      break;
    pwm.setPWM(1, 0, pulse1);
    delay(100);
    break;
  //軸2 有兩個RC servo一起帶動, 分別占用9685編號0,1,(2,3),4,5.....
  case 21: // 軸 2 順
    pulse2 += 10;
    pulse3 -= 10; //兩個servo需反轉
    if (pulse2 > SERVOMAX)
      break;
    pwm.setPWM(2, 0, pulse2);
    pwm.setPWM(3, 0, pulse3);
    delay(100);
    break;
  case 22: // 軸 2 逆
    pulse2 -= 10;
    pulse3 += 10; //兩個servo需反轉
    if (pulse2 < SERVOMIN)
      break;
    pwm.setPWM(2, 0, pulse2);
    pwm.setPWM(3, 0, pulse3);
    delay(100);
    break;
  case 31: // Arm point 3 順
    pulse4 += 10;
    if (pulse4 > SERVOMAX)
      break;
    pwm.setPWM(4, 0, pulse4);
    delay(100);
  case 32: // Arm point 3 逆
    pulse3 -= 10;
    if (pulse3 < SERVOMIN)
      break;
    pwm.setPWM(4, 0, pulse3);
    delay(100);
    break;
  case 41: // Arm point 4 順
    pulse4 += 10;
    if (pulse4 > SERVOMAX)
      break;
    pwm.setPWM(5, 0, pulse4);
    delay(100);
    break;
  case 42: // Arm point 4 逆
    pulse4 -= 10;
    if (pulse4 < SERVOMIN)
      break;
    pwm.setPWM(5, 0, pulse4);
    delay(100);
    break;
  case 51: // Arm point 5 順
    pulse5 += 10;
    if (pulse5 > SERVOMAX)
      break;
    pwm.setPWM(6, 0, pulse5);
    delay(100);
    break;
  case 52: // Arm point 5 逆
    pulse5 -= 10;
    if (pulse5 < SERVOMIN)
      break;
    pwm.setPWM(6, 0, pulse5);
    delay(100);
    break;
  case 61: // Arm point 6 順
    pulse6 += 10;
    if (pulse6 > SERVOMAX)
      break;
    pwm.setPWM(7, 0, pulse6);
    delay(100);
    break;
  case 62: // Arm point 6 逆
    pulse6 -= 10;
    if (pulse6 < SERVOMIN)
      break;
    pwm.setPWM(7, 0, pulse6);
    delay(100);
    break;
  }
  //網頁部分
  message += "<html> <head> <title>Gungor yalcin</title> <meta charset=\"utf-8\">";
  message += "<style>div{float: left;  width: 10%;  height: 200px;  padding: 20px;}</style></head>";
  message += "<body><h1 style=font-size:45px;text-align:center;>NodeMCU WiFi Control Web Server</h1><div>";
  //夾爪按鈕
  message += "<table style=margin-left:100px;>";
  message += "<th colspan=\"2\"><h style=width:200;height:2;font-size:30px>夾爪</h>";
  message += "<tr>";  //夾
  message += "<td><a href=\/?a=0\><button style=\width:100;height:100;font-size:60px;\ class=\button\>夾</button></a>";
  message += "<tr>";  //放
  message += "<td><a href=\/?a=10\><button style=\width:100;height:100;font-size:60px;\ class=\button\>放</button></a>";
  message += "</table></div>";
  //車輛及手臂底座
  message += "<table  style=margin-left:400px>";
  message += "<th colspan=\"3\"><h style=width:200;height:50;font-size:30px>車輛控制</h>";
  message += "<td style=\"width: 10%\">";
  message += "<th colspan=\"3\"><h style=width:200;height:50;font-size:30px>手臂底座Servo</h>";
  
  message += "<tr>";  //車輛第一行
  message += "<td><a href=\/?a=1\><button style=\width:100;height:100;font-size:100px;\ class=\button\>\\</button></a>";
  message += "<td><a href=\/?a=2\><button style=\width:100;height:100;font-size:100px;\ class=\button\>^</button></a>";
  message += "<td><a href=\/?a=3\><button style=\width:100;height:100;font-size:100px;\ class=\button\>/</button></a>";
  message += "<td>";  //手臂第一行
  message += "<td><a href=\/?a=11\><button style=\width:100;height:100;font-size:100px;\ class=\button\>\\</button></a>";
  message += "<td><a href=\/?a=12\><button style=\width:100;height:100;font-size:100px;\ class=\button\>^</button></a>";
  message += "<td><a href=\/?a=13\><button style=\width:100;height:100;font-size:100px;\ class=\button\>/</button></a>";

  message += "<tr>";  //車輛第二行
  message += "<td><a href=\/?a=4\><button style=\width:100;height:100;font-size:100px;\ class=\button\> < </button></a>";
  message += "<td><a href=\/?a=5\><button style=\width:100;height:100;font-size:40px;\ class=\button\>Stop</button></a>";
  message += "<td><a href=\/?a=6\><button style=\width:100;height:100;font-size:100px;\ class=\button\> > </button></a>";
  message += "<td>";  //手臂第二行
  message += "<td><a href=\/?a=14\><button style=\width:100;height:100;font-size:100px;\ class=\button\> < </button></a>";
  message += "<td><a href=\/?a=15\><button style=\width:100;height:100;font-size:40px;\ class=\button\>原點</button></a>";
  message += "<td><a href=\/?a=16\><button style=\width:100;height:100;font-size:100px;\ class=\button\> > </button></a>";
  message += "<td>";

  message += "<tr>";  //車輛第三行
  message += "<td><a href=\/?a=7\><button style=\width:100;height:100;font-size:100px;\ class=\button\>/</button></a>";
  message += "<td><a href=\/?a=8\><button style=\width:100;height:100;font-size:100px;\ class=\button\>v</button></a>";
  message += "<td><a href=\/?a=9\><button style=\width:100;height:100;font-size:100px;\ class=\button\>\\</button></a>";
  message += "<td>";  //手臂第三行
  message += "<td><a href=\/?a=17\><button style=\width:100;height:100;font-size:100px;\ class=\button\>/</button></a>";
  message += "<td><a href=\/?a=18\><button style=\width:100;height:100;font-size:100px;\ class=\button\>v</button></a>";
  message += "<td><a href=\/?a=19\><button style=\width:100;height:100;font-size:100px;\ class=\button\>\\</button></a>";
  message += "</table>";

  message += "<h3 style=\"width: 100%\";></h3>";
  //關節控制
  message += "<table style=\"margin: auto\">";
  message += "<tr>";
  message += "<th colspan=\"2\"><h style=width:200;height:50;font-size:30px>Arm joint1</h>";
  message += "<td style=\"width: 10%\">";
  message += "<th colspan=\"2\"><h style=width:200;height:50;font-size:30px>Arm joint2</h>";
  message += "<td style=\"width: 10%\">";
  message += "<th colspan=\"2\"><h style=width:200;height:50;font-size:30px>Arm joint3</h>";

  message += "<tr>";  //關節1
  message += "<td><a href=\/?a=11\><button style=\width:100;height:100;font-size:60px;\ class=\button\>順</button></a>";
  message += "<td><a href=\/?a=12\><button style=\width:100;height:100;font-size:60px;\ class=\button\>逆</button></a>";
  message += "<td>";  //關節2
  message += "<td><a href=\/?a=21\><button style=\width:100;height:100;font-size:60px;\ class=\button\>順</button></a>";
  message += "<td><a href=\/?a=22\><button style=\width:100;height:100;font-size:60px;\ class=\button\>逆</button></a>";
  message += "<td>";  //關節3
  message += "<td><a href=\/?a=31\><button style=\width:100;height:100;font-size:60px;\ class=\button\>順</button></a>";
  message += "<td><a href=\/?a=32\><button style=\width:100;height:100;font-size:60px;\ class=\button\>逆</button></a>";

  message += "<tr>";
  message += "<th colspan=\"2\"><h style=width:200;height:50;font-size:30px>Arm joint4</h>";
  message += "<td style=\"width: 10%\">";
  message += "<th colspan=\"2\"><h style=width:200;height:50;font-size:30px>Arm joint5</h>";
  message += "<td style=\"width: 10%\">";
  message += "<th colspan=\"2\"><h style=width:200;height:50;font-size:30px>Arm joint6</h>";

  message += "<tr>";  //關節4
  message += "<td><a href=\/?a=41\><button style=\width:100;height:100;font-size:60px;\ class=\button\>順</button></a>";
  message += "<td><a href=\/?a=42\><button style=\width:100;height:100;font-size:60px;\ class=\button\>逆</button></a>";
  message += "<td>";  //關節5
  message += "<td><a href=\/?a=51\><button style=\width:100;height:100;font-size:60px;\ class=\button\>順</button></a>";
  message += "<td><a href=\/?a=52\><button style=\width:100;height:100;font-size:60px;\ class=\button\>逆</button></a>";
  message += "<td>";  //關節6
  message += "<td><a href=\/?a=61\><button style=\width:100;height:100;font-size:60px;\ class=\button\>順</button></a>";
  message += "<td><a href=\/?a=62\><button style=\width:100;height:100;font-size:60px;\ class=\button\>逆</button></a>";

  message += "</table>";
  message += "</body>";
  message += "</html>";
  server.send(200, "text/html", message);
}

void handleNotFound() //錯誤位址回報錯誤
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_AP); //WIFI模式
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  // 序列阜顯示WiFi的連接狀況&WiFi的IP
  Serial.println("");
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(myIP);
  server.on("/", handleCar);
  server.on("/inline", []() {server.send(200, "text/plain", "this works as well");  //test web
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  //Wire.pins(3,4); 
  pwm.begin();
  //pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(2);
//各軸手臂回歸初始位置
 pwm.setPWM(0, 0, 350);
 delay(50);
 pwm.setPWM(1, 0, 350);
 delay(50);
 pwm.setPWM(2, 0, 350);
 delay(50);
 pwm.setPWM(3, 0, 350);
 delay(50);
 pwm.setPWM(4, 0, 350);
 delay(50);
 pwm.setPWM(5, 0, 350);
 delay(50);
 pwm.setPWM(6, 0, 350);
 delay(50);
 pwm.setPWM(7, 0, 350);
 delay(50);

}

void loop()
{
  server.handleClient();
}
