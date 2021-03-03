#include <MCS.h>
#include <WiFi.h>

char _lwifi_ssid[] = "TP-Link_6BF8";
char _lwifi_pass[] = "32696424";
MCSDevice mcs("DaOM8NuZ", "tse0AQqItUeSCX9a");
MCSDisplayInteger load("load1");
MCSDisplayInteger load("load2");
MCSDisplayInteger load("load3");
MCSDisplayInteger load("load4");

void setup()
{
  Serial.begin(9600);

  mcs.addChannel(load);
  Serial.println("Wi-Fi開始連線");
  while (WiFi.begin(_lwifi_ssid, _lwifi_pass) != WL_CONNECTED) { delay(1000); }
  Serial.println("Wi-Fi連線成功");
  Serial.println("MCS連線成功");
  while(!mcs.connected()) { mcs.connect(); }
  Serial.println("MCS連線成功");
  Serial.begin(9600);
}

void loop()
{
  while (!mcs.connected()) {
    mcs.connect();
    if (mcs.connected()) { Serial.println("MCS Reconnected."); }
  }
  mcs.process(100);
  load.set(analogRead(A0));
  Serial.println(analogRead(A0));
  delay(5000);
}