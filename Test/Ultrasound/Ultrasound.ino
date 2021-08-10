#include <Arduino.h>


#ifdef ESP32
#define TRIG 26
#define ECHO 27
#elif defined(ESP8266)
#define TRIG 15
#define ECHO 13
#endif

int Ultrasound(int trigPin, int echoPin)
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
  return duration;
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  // Pass INC as a parameter to get the distance in inches
  //distance = ultrasonic.read();
  
  Serial.print("Distance in CM: ");
  Serial.println(Ultrasound(TRIG,ECHO));
  delay(1000);
}