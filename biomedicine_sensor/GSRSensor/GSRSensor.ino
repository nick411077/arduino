#include <Arduino.h>

const int LED = 2;
const int GSR = 32;
int threshold = 0;
int sensorValue;


void setup()
{
    long sum = 0;
    Serial.begin(115200);
    analogReadResolution(10);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
    delay(1000);

    for (int i = 0; i < 500; i++)
    {
        sensorValue = analogRead(GSR);
        sum += sensorValue;
        delay(5);
    }
    threshold = sum / 500;
    Serial.print("threshold =");
    Serial.println(threshold);
}

void loop()
{
    int temp;
    sensorValue = analogRead(GSR);
    //Serial.print("sensorValue=");
    Serial.println(sensorValue);
    temp = threshold - sensorValue;
    if (abs(temp) > 70)
    {
        sensorValue = analogRead(GSR);
        temp = threshold - sensorValue;
        if (abs(temp) > 70)
        {
            digitalWrite(LED, HIGH);
            Serial.println("Emotion Changes Detected!");
            delay(3000);
            digitalWrite(LED, LOW);
            delay(1000);
        }
    }
}
//if (abs(temp) > 70)是調整感應力道反應值 60為預設但是ESP32大概要70才比較不容易誤觸發
//手指套也要嘗試調整位址進行抓握的動作才好觸發