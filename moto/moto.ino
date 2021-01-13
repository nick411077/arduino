#include <Arduino.h>
#include <AccelStepper.h>

int pulse = 26, dir = 27, enable = 14; //Arduino給驅動器的腳位
int MaxSpeed = 20000;
int Acceleration = 2000;
int Max = 60000;
AccelStepper stepper(1,pulse,dir);

void setup(){
  Serial.begin(115200);
  //設定I/O腳位
  pinMode(pulse, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(enable, OUTPUT);
  digitalWrite(dir,HIGH);
  digitalWrite(enable,LOW);
  stepper.setEnablePin(enable);
  stepper.disableOutputs();
  stepper.setMaxSpeed(MaxSpeed);
  stepper.setAcceleration(Acceleration);
  stepper.setCurrentPosition(0);

}

void loop()

{

  if (stepper.currentPosition() == 0)
  {
    stepper.moveTo(Max);
  }
  else if (stepper.currentPosition() == Max)
  {
    stepper.moveTo(0);
  }

  stepper.run();

}