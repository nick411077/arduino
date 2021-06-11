#ifndef PS2_h
    #define PS2_h

#include "Arduino.h"

#define PSB_SELECT      0x0001
#define PSB_L3          0x0002
#define PSB_R3          0x0004
#define PSB_START       0x0008
#define PSB_PAD_UP      0x0010
#define PSB_PAD_RIGHT   0x0020
#define PSB_PAD_DOWN    0x0040
#define PSB_PAD_LEFT    0x0080
#define PSB_L2          0x0100
#define PSB_R2          0x0200
#define PSB_L1          0x0400
#define PSB_R1          0x0800
#define PSB_GREEN       0x1000
#define PSB_RED         0x2000
#define PSB_BLUE        0x4000
#define PSB_PINK        0x8000
#define PSB_TRIANGLE    0x1000
#define PSB_CIRCLE      0x2000
#define PSB_CROSS       0x4000
#define PSB_SQUARE      0x8000

#define PSS_RX 4
#define PSS_RY 5
#define PSS_LX 6
#define PSS_LY 7

class PS2
{
public:
    void read();
    void write();
    void DigitalWrite(uint8_t, uint8_t);
    void AnalogWrite(int, int, int);
    boolean Button(uint16_t); //will be TRUE if button is being pressed
    boolean NewButtonState();
    boolean NewButtonState(unsigned int); //will be TRUE if button was JUST pressed OR released
    boolean ButtonPressed(unsigned int);  //will be TRUE if button was JUST pressed
    boolean ButtonReleased(unsigned int); //will be TRUE if button was JUST released
    byte Analog(byte);

private:
    unsigned char Data[8];
    unsigned char CheckData[8];
    unsigned char sensorData[8];
    unsigned int last_buttons;
    unsigned int buttons;
};

#endif