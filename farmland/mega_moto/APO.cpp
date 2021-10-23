#include "APO.h"
#include "Arduino.h"

bool APO::attach(Servo _PINA, Servo _PINB)
{
    RCA = _PINA;
    RCB = _PINB;
}
void APO::write(int degreesA, int degreesB)
{
    RCA.write(degreesA);
    RCB.write(degreesB);
}
void APO::writeMicroseconds(int degreesA, int degreesB)
{
    RCA.writeMicroseconds(degreesA);
    RCB.writeMicroseconds(degreesB);
}
int APO::read()
{
    RCA.read();
    RCB.read();
}
int APO::readMicroseconds()
{
    RCA.readMicroseconds();
    RCB.readMicroseconds();
}
void APO::Forward(int Apwm, int Bpwm)
{
    RCA.write(90-Apwm);
    RCB.write(90-Bpwm);
}
void APO::Reverse(int Apwm, int Bpwm)
{
    RCA.write(90+Apwm);
    RCB.write(90+Bpwm);
}
void APO::Left(int Apwm, int Bpwm)
{
    RCA.write(90+Apwm);
    RCB.write(90-Bpwm);
}
void APO::Right(int Apwm, int Bpwm)
{
    RCA.write(90-Apwm);
    RCB.write(90+Bpwm);
}