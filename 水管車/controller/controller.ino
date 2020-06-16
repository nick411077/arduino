#include <PS2X_lib.h>

int DRE = 14;
int num = 0;
long int GG;
PS2X ps2x;
int error = 0; 
byte type = 0;



void setup()
{
    Serial.begin(115200); // initialize serial at baudrate 9600:
    Serial2.begin(115200);
    delay(500);
    error = ps2x.config_gamepad(27, 25, 26, 33, false, false); //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error

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
    pinMode(DRE, OUTPUT);
    delay(10);
    digitalWrite(DRE, LOW); //  (always high as Master Writes data to Slave)
}
void loop()
{
    num = 0;
    if (error == 1) //skip loop if no controller found
        return;
    ps2x.read_gamepad(false, 0);
    int lxReading = ps2x.Analog(PSS_LX);
    int lyReading = ps2x.Analog(PSS_LY);
    int rxReading = ps2x.Analog(PSS_RX);
    int ryReading = ps2x.Analog(PSS_RY);
    if (lyReading < 50 ){num=num+1;}
    if (lxReading > 200){num=num+2;}
    if (lxReading < 50 ){num=num+4;}
    if (lyReading > 200){num=num+8;}
    digitalWrite(DRE, HIGH);
    Serial2.println(num);
    delay(50);
    digitalWrite(DRE, LOW);
    while (Serial2.available() >0)
    {
        digitalWrite(DRE, LOW);
        GG = Serial2.read();
        Serial.write(GG);
    }
    delay(50);
}
