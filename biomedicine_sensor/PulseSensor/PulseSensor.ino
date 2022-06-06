#define USE_ARDUINO_INTERRUPTS true    
#include <PulseSensorPlayground.h>     //涵入程式庫 PulseSensorPlayground.h

const int PulseWire = 0;               //感測器訊號線連接 Arduino Analog A0 pin
const int LED13 = 13;                  //LED 長腳連接 Arduino pin 13
int Threshold = 550;                   // Arduino 根據讀入的訊號，要開始計算每分鐘脈搏數的門檻值。這個門檻值，可以微調。
                               
PulseSensorPlayground pulseSensor;  

void setup() {   
  Serial.begin(115200);          
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);       //LED 隨著脈搏次數閃爍
  pulseSensor.setThreshold(Threshold);   
  if (pulseSensor.begin()) 
  {
    Serial.println("We created a pulseSensor Object !");
  }
}

void loop() {
int myBPM = pulseSensor.getBeatsPerMinute();
if (pulseSensor.sawStartOfBeat()) {
 Serial.println("♥  A HeartBeat Happened ! ");     //當 Arduino 讀到真正的脈搏訊號，印出每分鐘的脈搏數
 Serial.print("BPM: ");
 Serial.println(myBPM); 
}
  delay(20);
}