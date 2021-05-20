#include "WiFi.h"
#include "MCS.h"

int trigPin =27;                  //Trig Pin
int echoPin =26;                  //Echo Pin
long duration, cm, inches;

const char* ssid = "iot517";
const char* pass = "iot517517";

MCSDevice mcs("DzmJU7LW", "Qpf4twi0atPN0pOr");
MCSDisplayInteger Ultrasound("Ultrasound");

 
void setup() {
  Serial.begin (115200);             // Serial Port begin
  pinMode(trigPin, OUTPUT);        // 定義輸入及輸出 
  pinMode(echoPin, INPUT);
  // set the data rate for the SoftwareSerial port
  mcs.addChannel(Ultrasound);
  Serial.print("Wi-Fi connecting to");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wi-Fi connected successfully!");
  while (!mcs.connected()) {
    mcs.connect();
  }
  Serial.println("MCS connected successfully!");
}

void loop()
{
  while (!mcs.connected()) {
  	mcs.connect();
  	if (mcs.connected()) { Serial.println("MCS 已重新連線"); }
  }
  mcs.process(100);

  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);     // 給 Trig 高電位，持續 10微秒
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  pinMode(echoPin, INPUT);             // 讀取 echo 的電位
  duration = pulseIn(echoPin, HIGH);   // 收到高電位時的時間
 
  cm = (duration/2) / 29.1;         // 將時間換算成距離 cm 或 inch  
  inches = (duration/2) / 74; 

  Serial.print("Distance : ");  
  Serial.print(inches);
  Serial.print("in,   ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  Ultrasound.set(cm);
  
  delay(250);
}