#include <Arduino.h>

void DigitalWrite(int pinNumber, boolean status)
{
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, status);
}

void AnalogWrite(int pinNumber, int LEDChannel, int value)
{
    ledcSetup(LEDChannel, 5000, 8);
    ledcAttachPin(pinNumber, LEDChannel);
    ledcWrite(LEDChannel, value);
}

void handleCar()
{
    int SerialValue;
    if (Serial.available() > 0)
    {
        SerialValue = Serial.parseInt();

        switch (SerialValue) //車輛控制
        {
        case 1: // 左前
            Serial.println("1");
            DigitalWrite(18, HIGH);
            DigitalWrite(5, LOW);
            AnalogWrite(25, 1, 155);
            DigitalWrite(32, HIGH);
            DigitalWrite(33, LOW);
            AnalogWrite(26, 2, 255);
            break;
        case 2: // 前
            Serial.println("2");
            DigitalWrite(18, HIGH);
            DigitalWrite(5, LOW);
            AnalogWrite(25, 1, 255);
            DigitalWrite(32, HIGH);
            DigitalWrite(33, LOW);
            AnalogWrite(26, 2, 255);
            break;
        case 3: // 右前
            Serial.println("3");
            DigitalWrite(18, HIGH);
            DigitalWrite(5, LOW);
            AnalogWrite(25, 1, 255);
            DigitalWrite(32, HIGH);
            DigitalWrite(33, LOW);
            AnalogWrite(26, 2, 155);
            break;
        case 4: // 左
            Serial.println("4");
            DigitalWrite(18, LOW);
            DigitalWrite(5, HIGH);
            AnalogWrite(25, 1, 255);
            DigitalWrite(32, HIGH);
            DigitalWrite(33, LOW);
            AnalogWrite(26, 2, 255);
            break;
        case 5: // 停
            Serial.println("5");
            DigitalWrite(18, HIGH);
            DigitalWrite(5, HIGH);
            AnalogWrite(25, 1, 255);
            DigitalWrite(32, HIGH);
            DigitalWrite(33, HIGH);
            AnalogWrite(26, 2, 255);
            break;
        case 6: // 右
            Serial.println("6");
            DigitalWrite(18, HIGH);
            DigitalWrite(5, LOW);
            AnalogWrite(25, 1, 255);
            DigitalWrite(32, LOW);
            DigitalWrite(33, HIGH);
            AnalogWrite(26, 2, 255);
            break;
        case 7: // 左後
            Serial.println("7");
            DigitalWrite(18, LOW);
            DigitalWrite(5, HIGH);
            AnalogWrite(25, 1, 155);
            DigitalWrite(32, LOW);
            DigitalWrite(33, HIGH);
            AnalogWrite(26, 2, 255);
            break;
        case 8: // 後
            Serial.println("8");
            DigitalWrite(18, LOW);
            DigitalWrite(5, HIGH);
            AnalogWrite(25, 1, 255);
            DigitalWrite(32, LOW);
            DigitalWrite(33, HIGH);
            AnalogWrite(26, 2, 255);
            break;
        case 9: // 右後
            Serial.println("9");
            DigitalWrite(18, LOW);
            DigitalWrite(5, HIGH);
            AnalogWrite(25, 1, 255);
            DigitalWrite(32, LOW);
            DigitalWrite(33, HIGH);
            AnalogWrite(26, 2, 155);
            break;
        }
    }
}
void setup()
{
  Serial.begin(115200);
  Serial.println("OWO");

}

void loop()
{
  handleCar();
}