byte x[15];
byte rfidCMD[8]={0x01,0x03,0x00,0x06,0x00,0x04,0xA4,0x08};//rfidD命令
byte rfidsetup[8]={0x01, 0x06, 0x00, 0x04, 0x00, 0x01, 0x09, 0xCB};

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial2.write(rfidsetup,sizeof(rfidsetup));
  Serial.println("setuping...");
  delay(500);
}

void loop() 
{ 
  rfidCMDValue();//發送RFID命令
  delay(100);
}

void rfid()//發送RFID命令
{
  Serial2.write(rfidCMD,sizeof(rfidCMD));
  delay(20); 
  rfidCMDValue();//接收RFID讀值 
}

void rfidCMDValue()//接收RFID讀值 
{
  if (Serial2.available() > 0) 
  {
    Serial2.readBytes(x, 15);
    for (byte i = 0; i < sizeof(x); i++)
    {
      Serial.print(x[i], HEX);
      Serial.print(",");
    }
    Serial.println();
  }
}
