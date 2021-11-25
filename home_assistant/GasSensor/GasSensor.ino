#include <Arduino.h>

void setup() 
{
    Serial.begin(115200);
}
 
void loop() {
    float sensor_volt;
    float RS_gas; // Get value of RS in a GAS
    float ratio; // Get ratio RS_GAS/RS_air
    int sensorValue = analogRead(35);
    sensor_volt=(float)sensorValue/4095*3.3;
    RS_gas = (3.3-sensor_volt)/sensor_volt; 
    ratio = RS_gas/60;
    Serial.println(sensorValue);
    Serial.print("sensor_volt = ");
    Serial.println(sensor_volt);
    Serial.print("RS_ratio = ");
    Serial.println(RS_gas);
    Serial.print("Rs/R0 = ");
    Serial.println(ratio);
    Serial.print("\n\n");
    delay(1000);
 
}
