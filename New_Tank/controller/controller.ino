#include <PS2X_lib.h>


unsigned char Data[8] = {0xFF, 0x7B, 0, 0, 0, 0, 0, 0};
uint16_t buttons;

int error = 0; 
byte type = 0;
byte GG;
int num;
int pwm;


PS2X ps2x;

void setup()
{
    Serial.begin(115200); // initialize serial at baudrate 9600:
    Serial2.begin(115200);
    delay(500);
    error = ps2x.config_gamepad(25, 26, 33, 27, true, true); //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error

    if (error == 0)
    {
        Serial.println("Found Controller, configured successful");
    }

    else if (error == 1)
        Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");

    else if (error == 2)
        Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

    else if (error == 3)
        Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

    type = ps2x.readType();
    switch (type)
    {
    case 0:
        Serial.println("Unknown Controller type");
        break;
    case 1:
        Serial.println("DualShock Controller Found");
        break;
    case 2:
        Serial.println("GuitarHero Controller Found");
        break;
    }
    delay(10);
}
void loop()
{
    Data[2]= 0x00;
    num = 0;
    pwm = 0;
    ps2x.read_gamepad(false, 0);
    Data[4] = ps2x.Analog(PSS_RX);
    Data[5] = ps2x.Analog(PSS_RY);
    Data[6] = ps2x.Analog(PSS_LX);
    Data[7] = ps2x.Analog(PSS_LY);
    buttons = ps2x.ButtonDataByte();
    Data[3] = ~buttons >> 8;
    Data[2] = ~buttons;//進位方式為PS2X_lib.h 可以參考
    serial();
    delay(100);
    
}
void serial() //PTZ上
{
  for (byte z = 0; z < sizeof(Data); z++)
  {
    Serial2.write(Data[z]);
    Serial.write(Data[z]);
  }
}