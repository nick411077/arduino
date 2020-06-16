#include <DHT.h>

#define DHTPIN A4     //類比腳位
#define DHTTYPE DHT21 //DHT類型

long int Lcounter = 0; //定義 counter 為 int 類型變數，且初始值為0
long int LaState;      //定義 aState 為 int 類型變數
long int LaLastState;  //定義 aLastState 為 int 類型變數
long int Rcounter = 0; //定義 counter 為 int 類型變數，且初始值為0
long int RaState;      //定義 aState 為 int 類型變數
long int RaLastState;  //定義 aLastState 為 int 類型變數
int RE = 12;
int DE = 10;
int num = 0;

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

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
    Serial.begin(115200);
    Serial2.begin(115200);
    dht.begin();
    LaLastState = Position(2);
    RaLastState = Position(3);
    pinMode(RE, OUTPUT);
    pinMode(DE, OUTPUT);
    delay(10);
    digitalWrite(RE, LOW);
    digitalWrite(DE, LOW);
    attachInterrupt(0, Ldata, CHANGE);
    attachInterrupt(1, Rdata, CHANGE);
}

void loop()
{
    num = 0;
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int x = analogRead(A7); //read from xpin
    int y = analogRead(A6); //read from ypin
    int z = analogRead(A5); //read from zpin
    if (Serial2.available() > 0)
    {
        num = Serial2.parseInt();
        switch (num)
        {
        case 1:
            Serial.println("F");
            DigitalWrite(9, HIGH);
            DigitalWrite(8, LOW);
            AnalogWrite(A0, 255);
            DigitalWrite(7, HIGH);
            DigitalWrite(6, LOW);
            AnalogWrite(A2, 255);
            break;
        case 2:
            Serial.println("R");
            DigitalWrite(9, HIGH);
            DigitalWrite(8, LOW);
            AnalogWrite(A0, 255);
            DigitalWrite(7, LOW);
            DigitalWrite(6, HIGH);
            AnalogWrite(A2, 255);
            break;
        case 3:
            Serial.println("FR");
            DigitalWrite(9, HIGH);
            DigitalWrite(8, LOW);
            AnalogWrite(A0, 255);
            DigitalWrite(7, HIGH);
            DigitalWrite(6, LOW);
            AnalogWrite(A2, 155);
            break;
        case 4:
            Serial.println("L");
            DigitalWrite(9, LOW);
            DigitalWrite(8, HIGH);
            AnalogWrite(A0, 255);
            DigitalWrite(7, HIGH);
            DigitalWrite(6, LOW);
            AnalogWrite(A2, 255);
            break;
        case 5:
            Serial.println("FL");
            DigitalWrite(9, HIGH);
            DigitalWrite(8, LOW);
            AnalogWrite(A0, 155);
            DigitalWrite(7, HIGH);
            DigitalWrite(6, LOW);
            AnalogWrite(A2, 255);
            break;
        case 8:
            Serial.println("B");
            DigitalWrite(9, LOW);
            DigitalWrite(8, HIGH);
            AnalogWrite(A0, 255);
            DigitalWrite(7, LOW);
            DigitalWrite(6, HIGH);
            AnalogWrite(A2, 255);
            break;
        case 10:
            Serial.println("BR");
            DigitalWrite(9, LOW);
            DigitalWrite(8, HIGH);
            AnalogWrite(A0, 255);
            DigitalWrite(7, LOW);
            DigitalWrite(6, HIGH);
            AnalogWrite(A2, 155);
            break;
        case 12:
            Serial.println("BL");
            DigitalWrite(9, LOW);
            DigitalWrite(8, HIGH);
            AnalogWrite(A0, 155);
            DigitalWrite(7, LOW);
            DigitalWrite(6, HIGH);
            AnalogWrite(A2, 255);
            break;
        case 0:
            Serial.println("S");
            DigitalWrite(9, HIGH);
            DigitalWrite(8, HIGH);
            AnalogWrite(A0, 255);
            DigitalWrite(7, HIGH);
            DigitalWrite(6, HIGH);
            AnalogWrite(A2, 255);
            break;
        }
    }
    digitalWrite(DE, HIGH);
    Serial2.println();
    Serial2.print("L:");
    Serial2.println(Lcounter);
    Serial2.print("R:");
    Serial2.println(Rcounter);
    Serial2.print("X:");
    Serial2.print(x);
    Serial2.print("Y:");
    Serial2.print(y);
    Serial2.print(" Z:");
    Serial2.print(z);
    Serial2.println();
    Serial2.print("H:");
    Serial2.print(h);
    Serial2.println();
    Serial2.print("T:");
    Serial2.print(t);
    Serial2.println("*C");
    delay(50);
    digitalWrite(DE, LOW);
    while (Serial2.read() > 0){}
}
void Ldata()
{
    LaState = Position(2); //將outputA的讀取值 設給 aState

    if (LaState != LaLastState)
    { //條件判斷，當aState 不等於 aLastState時發生

        if (Position(4) != LaState)
        {               //條件判斷，當outputB讀取值 不等於 aState時發生
            Lcounter++; //計數器+1
        }
        else
        {
            Lcounter--; //計數器-1
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
            Rcounter--; //計數器+1
        }
        else
        {
            Rcounter++; //計數器-1
        }
    }
    RaLastState = RaState; //將aState 最後的值 設給 aLastState
}
