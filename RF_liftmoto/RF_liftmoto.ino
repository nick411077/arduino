#include <Arduino.h>
#include <RCSwitch.h>

#define IN1A 4
#define IN1B 5

#define EN1 3

#define LED 13

RCSwitch mySwitch = RCSwitch();

void setup()
{
    Serial.begin(115200);
    mySwitch.enableReceive(0);
    pinMode(IN1A, OUTPUT);
    pinMode(IN1B, OUTPUT);
    pinMode(EN1, OUTPUT);
    pinMode(LED, OUTPUT);

}

void loop()
{
    RF();
}

void moto(unsigned long DIR, int value)
{
    switch (DIR)
    {
    case 5592332:
        digitalWrite(IN1A, HIGH);
        digitalWrite(IN1B, LOW);
        analogWrite(EN1, value);
        Serial.println("up");
        break;
    case 5592512:
        digitalWrite(IN1A, HIGH);
        digitalWrite(IN1B, HIGH);
        analogWrite(EN1, value);
        Serial.println("stop");
        break;
    case 5592323:
        digitalWrite(IN1A, LOW);
        digitalWrite(IN1B, HIGH);
        analogWrite(EN1, value);
        Serial.println("down");
        break;
    }
}

void RF()
{
    if (mySwitch.available())
    {
        digitalWrite(LED,HIGH);
        /*
        Serial.print("Received ");
        Serial.print(mySwitch.getReceivedValue());
        Serial.print(" / ");
        Serial.print(mySwitch.getReceivedBitlength());
        Serial.print("bit ");
        Serial.print("Protocol: ");
        Serial.println(mySwitch.getReceivedProtocol());
        */
        moto(mySwitch.getReceivedValue(),250);

        mySwitch.resetAvailable();
        digitalWrite(LED,LOW);
    }
}