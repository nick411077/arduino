#include <Arduino.h>

void setup() 
{
  Serial.begin(115200);
  pinMode(26,INPUT_PULLDOWN);
}

void loop() 
{
  Serial.print("switch:");
  Serial.println(digitalRead(26));
  delay(500);
}
