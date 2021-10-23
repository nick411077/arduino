#ifndef APO_h
    #define APO_h

#include "Arduino.h"
#include <Servo.h>


class APO
{
public:
    bool attach(Servo _PINA, Servo _PINB);
    void write(int degreesA, int degreesB);
    void writeMicroseconds(int degreesA, int degreesB);
    int read();
    int readMicroseconds();
    void Forward(int Apwm, int Bpwm);
    void Reverse(int Apwm, int Bpwm);
    void Left(int Apwm, int Bpwm);
    void Right(int Apwm, int Bpwm);
private:
    Servo RCA;
    Servo RCB;
};

#endif // APO_h