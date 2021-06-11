#include "PS2.h"
#include "Arduino.h"

boolean PS2::NewButtonState() {
  return ((last_buttons ^ buttons) > 0);
}

/****************************************************************************************/
boolean PS2::NewButtonState(unsigned int button) {
  return (((last_buttons ^ buttons) & button) > 0);
}

/****************************************************************************************/
boolean PS2::ButtonPressed(unsigned int button) {
  return(NewButtonState(button) & Button(button));
}

/****************************************************************************************/
boolean PS2::ButtonReleased(unsigned int button) {
  return((NewButtonState(button)) & ((~last_buttons & button) > 0));
}

/****************************************************************************************/
boolean PS2::Button(uint16_t button) {
  return ((~buttons & button) > 0);
}

/****************************************************************************************/
byte PS2::Analog(byte button) {
   return Data[button];
}

void PS2::DigitalWrite(uint8_t pinNumber, uint8_t status)
{
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, status);
}

void PS2::AnalogWrite(int pinNumber, int LEDChannel, int value)
{
    ledcSetup(LEDChannel, 15000, 8);
    ledcAttachPin(pinNumber, LEDChannel);
    ledcWrite(LEDChannel, value);
}

void PS2::read()
{
    if (Serial2.available())
    {
        for (byte z = 0; z < sizeof(Data); z++)
        {
            CheckData[z]=Data[z];
        }
        Serial2.readBytes(Data, 8);
        int Check = Data[0] + Data[1];
        if (Check == 130)
        {
            for (byte z = 0; z < sizeof(Data); z++)
            {
                Serial.print(Data[z], HEX);
            }
            Serial.println();
            last_buttons = buttons;
            buttons =  (uint16_t)(Data[3] << 8) + Data[2];
        }
        else
        {
            Serial.println("BUG");
            for (byte z = 0; z < sizeof(CheckData); z++)
            {
                Data[z]=CheckData[z];
            }
            while (Serial2.read() >= 0){}
        }
    }
}

void PS2::write() //PTZ上
{
  for (byte z = 0; z < sizeof(sensorData); z++)
  {
    Serial2.write(sensorData[z]);
    Serial.write(sensorData[z]);
  }
}