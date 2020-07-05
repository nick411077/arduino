#include <PS2X_lib.h>

int DE = 6;
int RE = 7;
int num = 0;
int pwm = 0;
byte GG;
PS2X ps2x;
int error = 0; 
byte type = 0;




void setup()
{
    Serial.begin(115200); // initialize serial at baudrate 9600:
    Serial2.begin(115200);
    delay(500);
    error = ps2x.config_gamepad(2, 4, 5, 3, true, true); //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error

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
    pinMode(DE, OUTPUT);
    pinMode(RE, OUTPUT);
    delay(10);
    digitalWrite(DE, LOW); //  (always high as Master Writes data to Slave)
    digitalWrite(RE, LOW);
}
void loop()
{
    num = 0;
    pwm = 0;
    if (error == 1) //skip loop if no controller found
        return;
    ps2x.read_gamepad(false, 0);
    byte rxReading = ps2x.Analog(PSS_RX);
    byte ryReading = ps2x.Analog(PSS_RY);
    if (ryReading < 100  &&ryReading != 0  ){num=num+1;}
    if (rxReading > 150 &&rxReading != 255){num=num+2;}
    if (rxReading < 100  &&rxReading != 0  ){num=num+4;}
    if (ryReading > 150 &&ryReading != 255){num=num+8;}
    if (ps2x.Button(PSB_PAD_UP)){pwm=15;}
    if (ps2x.Button(PSB_PAD_DOWN)){pwm=30;}
    if (ps2x.Button(PSB_PAD_RIGHT)){pwm=45;}
    if (ps2x.Button(PSB_PAD_LEFT)){pwm=60;}
    if (ps2x.Button(PSB_R1)){pwm=75;}
    if (ps2x.Button(PSB_L1)){pwm=90;}
    delay(10); //解決delay會有掉資料的問題
    digitalWrite(DE, HIGH);
    Serial2.println(num+pwm);
    Serial.println(num+pwm);
    Serial2.flush();
    digitalWrite(DE, LOW);
    Serial.println(Serial2.available());
    if (Serial2.available() > 75)
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