#include <PS2X_lib.h>



unsigned char PS2XData[8] = {0x55, 0x2D, 0, 0, 0, 0, 0, 0};
unsigned char Data[8];
uint16_t buttons;

int error = 0; 
byte type = 0;


PS2X ps2x;

void setup()
{
    Serial.begin(115200); // initialize serial at baudrate 115200:
    Serial2.begin(115200);
    delay(500);
    error = ps2x.config_gamepad(25, 26, 33, 27, true, true); //設置引腳和設置:  GamePad(clock, command, attention, PS2XData, Pressures?, Rumble?) check for error

    if (error == 0)
    {
        Serial.println("找到控制器，配對成功");
    }

    else if (error == 1)
        Serial.println("未找到控制器，檢查接線，查看 readme.txt 以啟用調試。 訪問 www.billporter.info 獲取故障排除提示");

    else if (error == 2)
        Serial.println("找到控制器但不接受命令。 請參閱 readme.txt 以啟用調試。 訪問 www.billporter.info 獲取故障排除提示");

    else if (error == 3)
        Serial.println("控制器拒絕進入壓力模式，可能不支持。");

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
    ps2x.read_gamepad(false, 0);
    PS2XData[4] = ps2x.Analog(PSS_RX);
    PS2XData[5] = ps2x.Analog(PSS_RY);
    PS2XData[6] = ps2x.Analog(PSS_LX);
    PS2XData[7] = ps2x.Analog(PSS_LY);
    buttons = ps2x.ButtonDataByte();
    Serial.println(buttons);
    PS2XData[3] = ~buttons >> 8;
    PS2XData[2] = ~buttons;//進位方式為PS2X_lib.h 可以參考
    write();
    delay(5);//傳送太快導致會掉資料
}
void read()
{
    if (Serial2.available())
    {
        Serial2.readBytes(Data, 8);
        for (byte z = 0; z < sizeof(Data); z++)
        {
            Serial.write(Data[z]);
            
        }
    }
}
void write() //PTZ上
{
  for (byte z = 0; z < sizeof(PS2XData); z++)
  {
    Serial2.write(PS2XData[z]);
    Serial.print(PS2XData[z],HEX);
  }
  Serial.println();
}
boolean Button(uint16_t button) {
  return ((~buttons & button) > 0);
}