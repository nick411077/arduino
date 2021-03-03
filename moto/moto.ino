#include <Arduino.h>
#include <AccelStepper.h>
//每步為1.8度，一週為200步
int pulse = 19, dir = 18, enable = 5; //Arduino給驅動器的腳位
int light1=35, light2=34, light3=39;
int ReMaxSpeed = 20000;
int ReAcceleration = 2000;
int MaxSpeed = 60000;
int Acceleration = 10000;
int Max = -60000;
AccelStepper stepper(1,pulse,dir);

void setup(){
  Serial.begin(115200);
  //設定I/O腳位
  pinMode(pulse, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(enable, OUTPUT);
  pinMode(light1,INPUT);
  pinMode(light2,INPUT);
  pinMode(light3,INPUT);
  digitalWrite(dir,HIGH);
  digitalWrite(enable,LOW);
  stepper.setEnablePin(enable);
  stepper.disableOutputs();
  stepper.setMaxSpeed(MaxSpeed);
  stepper.setAcceleration(Acceleration);
  stepper.setCurrentPosition(0);
  Serial.println("1");
  while (digitalRead(light1) == 0)
  {
    stepper.setSpeed(-10000);
      stepper.runSpeed();
  }
  if (digitalRead(light1) == 1)
    {
      Serial.println("2");
      stepper.setCurrentPosition(0);
      stepper.setSpeed(-40000);
      while (digitalRead(light2) == 1)
      {
        stepper.runSpeed();
      }
      Serial.print("set");
      Serial.println(stepper.currentPosition());
    }
    Max = stepper.currentPosition();
    stepper.setMaxSpeed(MaxSpeed);
    stepper.setAcceleration(Acceleration);
}

void loop()

{

  if (stepper.currentPosition() == 0)
  {
    Serial.println(Max);
    stepper.moveTo(Max);
  }
  else if (stepper.currentPosition() == Max)
  {
    Serial.println(0);
    stepper.moveTo(0);
  }
  stepper.run();
}