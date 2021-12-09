#include <Arduino.h>

float adc_voltage = 0.0;//分壓電壓
float in_voltage = 0.0;//輸入電壓
 
// Floats for resistor values in divider (in ohms)
float R1 = 1800.0;//電阻
float R2 = 3300.0; //電阻
// Float for Reference Voltage
float ref_voltage = 3.3; //Arduino 輸入
 
// Integer for ADC value
int adc_value = 0;

void setup() 
{
  Serial.begin(115200);
  pinMode(35,INPUT);
}

void loop() {
  adc_value = analogRead(35);//數值
  adc_voltage  = (adc_value * ref_voltage) / 4095.0; //換算分壓電壓
  in_voltage = adc_voltage / (R2/(R1+R2)); //換算輸入電壓
  Serial.print("Input Voltage = ");
  Serial.println(in_voltage, 2);
  delay(500);
}
