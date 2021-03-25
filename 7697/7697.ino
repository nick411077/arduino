#include <Arduino.h>
#include <MCS.h>
#include <LWiFi.h>

char _lwifi_ssid[] = "iphone";
char _lwifi_pass[] = "00000000";
MCSDevice mcs("DaOM8NuZ", "tse0AQqItUeSCX9a");
MCSDisplayInteger load1("load1");
MCSDisplayInteger load2("load2");
MCSDisplayInteger load3("load3");
MCSDisplayInteger load4("load4");

void setup()
{
  Serial.begin(9600);

  mcs.addChannel(load1);
  mcs.addChannel(load2);
  mcs.addChannel(load3);
  mcs.addChannel(load4);
  Serial.println("Wi-Fi開始連線");
  Serial.println(_lwifi_ssid);
  WiFi.begin(_lwifi_ssid, _lwifi_pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wi-Fi連線成功");
  Serial.println("MCS連線成功");
  while(!mcs.connected()) { mcs.connect(); }
  Serial.println("MCS連線成功");
  Serial.begin(9600);
}
void load(int y, MCSDisplayInteger load, int trigPin, int echoPin)
{
  int x = Ultrasound(trigPin, echoPin);
  load.set(x);
  Serial.print(y);
  Serial.print(" : ");
  Serial.println(x);
}

void DigitalWrite(int pinNumber, boolean status)
{
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, status);
}

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
  duration = pulseIn(echoPin, HIGH,4000);
  duration = duration / 59;
  if ((duration < 5) || (duration > 500)) return false;
  return duration;
}

void loop()
{
  while (!mcs.connected()) {
    mcs.connect();
    if (mcs.connected()) { Serial.println("MCS Reconnected."); }
  }
  mcs.process(100);
  load(1,load1,2,3);
  delay(1000);
}
