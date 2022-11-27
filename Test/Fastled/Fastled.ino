#include "FastLED.h"            // 此示例程序需要使用FastLED库
 
#define NUM_LEDS 20             // LED灯珠数量
#define DATA_PIN 2              // Arduino输出控制信号引脚
#define LED_TYPE WS2811         // LED灯带型号
#define COLOR_ORDER BGR         // RGB灯珠中红色、绿色、蓝色LED的排列顺序
 
uint8_t max_bright = 255;       // LED亮度控制变量，可使用数值为 0 ～ 255， 数值越大则光带亮度越高
 
CRGB leds[NUM_LEDS];            // 建立光带leds
 
void setup() { 
  LEDS.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS);  // 初始化光带 
  FastLED.setBrightness(max_bright);                            // 设置光带亮度
}
 
void loop() 
{

for(int dot=(NUM_LEDS-1) ; dot >=0 ; dot--)

{ leds[dot] = CRGB::HotPink;

FastLED.show();

delay(300);

}

for(int dot = 0;dot < NUM_LEDS; dot++)

{

leds[dot] = CRGB::Blue;

FastLED.show();

delay(300);

}

}

