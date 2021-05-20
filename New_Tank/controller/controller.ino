#include <PS2X_lib.h>

byte rxReading;
byte ryReading;
byte lxReading;
byte lyReading;

byte Data[] = {0xFF, 0x01, lxReading,lyReading,rxReading,ryReading};


int error = 0; 
byte type = 0;
int num;
int pwm;


PS2X ps2x;

void setup()
{
    Serial.begin(115200); // initialize serial at baudrate 9600:
    Serial2.begin(115200);
    delay(500);
    error = ps2x.config_gamepad(3, 4, 2, 5, true, true); //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error

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
    num = 0;
    pwm = 0;
    ps2x.read_gamepad(false, 0);
    rxReading = (ps2x.Analog(PSS_RX),DEC);
    ryReading = (ps2x.Analog(PSS_RY),DEC);
    lxReading = (ps2x.Analog(PSS_LX),DEC);
    lyReading = (ps2x.Analog(PSS_LY),DEC);
    if (ps2x.Button(PSB_PAD_UP)){pwm=15;}
    if (ps2x.Button(PSB_PAD_DOWN)){pwm=30;}
    if (ps2x.Button(PSB_PAD_RIGHT)){pwm=45;}
    if (ps2x.Button(PSB_PAD_LEFT)){pwm=60;}
    if (ps2x.Button(PSB_R1)){pwm=75;}
    if (ps2x.Button(PSB_L1)){pwm=90;}
    delay(10); //解決delay會有掉資料的問題
    Serial2.println(num+pwm);
    Serial2.flush();
    if (Serial2.available() > 1)
    {
        while (Serial2.available() > 0)
        {

            GG = Serial2.read();
            Serial.write(GG);
            Serial.flush();
        }
    }
    else
    {
        while (Serial2.read() > 0){}
    }
    
}
