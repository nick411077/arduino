#include <DHT.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define DHTPIN A4     //類比腳位
#define DHTTYPE DHT11 //DHT類型

#define MIN_PULSE_WIDTH       500
#define MAX_PULSE_WIDTH       2500
#define FREQUENCY             50


long int Lcounter = 0; //定義 counter 為 int 類型變數，且初始值為0
long int LaState;      //定義 aState 為 int 類型變數
long int LaLastState;  //定義 aLastState 為 int 類型變數
long int Rcounter = 0; //定義 counter 為 int 類型變數，且初始值為0
long int RaState;      //定義 aState 為 int 類型變數
long int RaLastState;  //定義 aLastState 為 int 類型變數
byte moto0 =180;
byte moto1 =100;
byte moto2 =170;
int RE = 13;
int DE = 12;
int numder = 0;
int num = 0;
int moto = 0;

void DigitalWrite(int pinNumber, boolean status)
{
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, status);
}
void AnalogWrite(int pinNumber, int value)
{
    pinMode(pinNumber, OUTPUT);
    analogWrite(pinNumber, value);
}
int Position(uint8_t pinNumber)
{
    pinMode(pinNumber, INPUT);
    digitalRead(pinNumber);
}
int pulseWidth(int angle)
{
  int pulse_wide, analog_value;
  pulse_wide   = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  analog_value = int(float(pulse_wide) / 1000000 * FREQUENCY * 4096);
  return analog_value;
}

DHT dht(DHTPIN, DHTTYPE);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void setup()
{
    Serial.begin(115200);
    Serial2.begin(115200);
    dht.begin();
    pwm.begin();
    pwm.setPWMFreq(FREQUENCY);  //模擬伺服在50赫茲更新下執行
    LaLastState = Position(2);
    RaLastState = Position(3);
    pinMode(RE, OUTPUT);
    pinMode(DE, OUTPUT);
    delay(10);
    digitalWrite(RE, LOW);
    digitalWrite(DE, LOW);
    attachInterrupt(0, Ldata, CHANGE);
    attachInterrupt(1, Rdata, CHANGE);
    pwm.setPWM(0, 0, pulseWidth(moto0));
    pwm.setPWM(1, 0, pulseWidth(moto1));
    pwm.setPWM(2, 0, pulseWidth(moto2));
}

void loop()
{
    num = 0;
    moto = 0;
    numder = 0;
    int x = analogRead(A7); //read from xpin
    int y = analogRead(A6); //read from ypin
    int z = analogRead(A5); //read from zpin
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (Serial2.available() > 0)
    {
        numder = Serial2.parseInt();
        Serial.println(numder);
        if (numder < 14){num = numder;}
        if (numder < 29 && numder > 14){num = numder - 15;moto = 15;}
        if (numder < 44 && numder > 29){num = numder - 30;moto = 30;}
        if (numder < 59 && numder > 44){num = numder - 45;moto = 45;}
        if (numder < 74 && numder > 59){num = numder - 60;moto = 60;}
        if (numder < 89 && numder > 74){num = numder - 75;moto = 75;}
        if (numder > 89){num = numder - 90;moto = 90;}
        switch (num)
        {
        case 1:
            Serial.println("F");
            DigitalWrite(9, HIGH);
            DigitalWrite(8, LOW);
            AnalogWrite(A0, 255);
            DigitalWrite(7, HIGH);
            DigitalWrite(6, LOW);
            AnalogWrite(A1, 255);
            break;
        case 2:
            Serial.println("R");
            DigitalWrite(9, HIGH);
            DigitalWrite(8, LOW);
            AnalogWrite(A0, 255);
            DigitalWrite(7, LOW);
            DigitalWrite(6, HIGH);
            AnalogWrite(A1, 255);
            break;
        case 3:
            Serial.println("FR");
            DigitalWrite(9, HIGH);
            DigitalWrite(8, LOW);
            AnalogWrite(A0, 255);
            DigitalWrite(7, HIGH);
            DigitalWrite(6, LOW);
            AnalogWrite(A1, 155);
            break;
        case 4:
            Serial.println("L");
            DigitalWrite(9, LOW);
            DigitalWrite(8, HIGH);
            AnalogWrite(A0, 255);
            DigitalWrite(7, HIGH);
            DigitalWrite(6, LOW);
            AnalogWrite(A1, 255);
            break;
        case 5:
            Serial.println("FL");
            DigitalWrite(9, HIGH);
            DigitalWrite(8, LOW);
            AnalogWrite(A0, 155);
            DigitalWrite(7, HIGH);
            DigitalWrite(6, LOW);
            AnalogWrite(A1, 255);
            break;
        case 8:
            Serial.println("B");
            DigitalWrite(9, LOW);
            DigitalWrite(8, HIGH);
            AnalogWrite(A0, 255);
            DigitalWrite(7, LOW);
            DigitalWrite(6, HIGH);
            AnalogWrite(A1, 255);
            break;
        case 10:
            Serial.println("BR");
            DigitalWrite(9, LOW);
            DigitalWrite(8, HIGH);
            AnalogWrite(A0, 255);
            DigitalWrite(7, LOW);
            DigitalWrite(6, HIGH);
            AnalogWrite(A1, 155);
            break;
        case 12:
            Serial.println("BL");
            DigitalWrite(9, LOW);
            DigitalWrite(8, HIGH);
            AnalogWrite(A0, 155);
            DigitalWrite(7, LOW);
            DigitalWrite(6, HIGH);
            AnalogWrite(A1, 255);
            break;
        case 0:
            Serial.println("S");
            DigitalWrite(9, HIGH);
            DigitalWrite(8, HIGH);
            AnalogWrite(A0, 255);
            DigitalWrite(7, HIGH);
            DigitalWrite(6, HIGH);
            AnalogWrite(A1, 255);
            break;
        }
        switch (moto)
        {
        case 15:
            Serial.println("15");
            if (moto0<=180)
            {
                moto0++;
                pwm.setPWM(0, 0, pulseWidth(moto0));
            }
            break;
        case 30:
            Serial.println("30");
            if (moto0>=160)
            {
                moto0--;
                pwm.setPWM(0, 0, pulseWidth(moto0));
            }
            break;
        case 45:
            Serial.println("45");
            if (moto1<=130)
            {
                moto1++;
                pwm.setPWM(1, 0, pulseWidth(moto1));
            }
            break;
        case 60:
            Serial.println("60");
            if (moto1>=70)
            {
                moto1--;
                pwm.setPWM(1, 0, pulseWidth(moto1));
            }
            break;
        case 75:
            Serial.println("75");
            if (moto2<=180)
            {
                moto2++;
                pwm.setPWM(2, 0, pulseWidth(moto2));
            }
            break;
        case 90:
            Serial.println("90");
            if (moto2>=100)
            {
                moto2--;
                pwm.setPWM(2, 0, pulseWidth(moto2));
            }
            break;
        }
    }
    String alldata = "{\"L\":\""+String(Lcounter)+"\",\"R\":\""+Rcounter+"\",\"X\":\""+x+"\",\"Y\":\""+y+"\",\"Z\":\""+z+"\",\"H\":\""+h+"\",\"T\":\""+t+"\"}";
    Serial2.println(alldata);
    Serial2.flush();
    while (Serial2.read() >= 0){}
    delay(5);
}
void Ldata()
{
    LaState = Position(2); //將outputA的讀取值 設給 aState

    if (LaState != LaLastState)
    { //條件判斷，當aState 不等於 aLastState時發生

        if (Position(4) != LaState)
        {               //條件判斷，當outputB讀取值 不等於 aState時發生
            Lcounter--; //計數器+1
        }
        else
        {
            Lcounter++; //計數器-1
        }
    }
    LaLastState = LaState; //將aState 最後的值 設給 aLastState
}
void Rdata()
{
    RaState = Position(3); //將outputA的讀取值 設給 aState

    if (RaState != RaLastState)
    { //條件判斷，當aState 不等於 aLastState時發生

        if (Position(5) != RaState)
        {               //條件判斷，當outputB讀取值 不等於 aState時發生
            Rcounter++; //計數器+1
        }
        else
        {
            Rcounter--; //計數器-1
        }
    }
    RaLastState = RaState; //將aState 最後的值 設給 aLastState
}
