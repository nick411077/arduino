# 水管車
# Controller<h2>是搖桿控制端
### Controller程式分類
搖桿判別
```C
    if (ryReading < 100  &&ryReading != 0  ){num=num+1;}
    if (rxReading > 150 &&rxReading != 255){num=num+2;}
    if (rxReading < 100  &&rxReading != 0  ){num=num+4;}
    if (ryReading > 150 &&ryReading != 255){num=num+8;}
    if (ps2x.Button(PSB_PAD_UP)){pwm=15;}
    if (ps2x.Button(PSB_PAD_DOWN)){pwm=30;}
    if (ps2x.Button(PSB_PAD_RIGHT)){pwm=45;}
    if (ps2x.Button(PSB_PAD_LEFT)){pwm=60;}
    if (ps2x.Button(PSB_R1)){pwm=75;}
    if (ps2x.Button(PSB_L1)){pwm=90;}
```
進行接收資料
```C
    delay(10);  //解決delay會有掉資料的問題
    digitalWrite(DRE, HIGH);
    Serial2.println(num+pwm);
    Serial.println(num+pwm);
    digitalWrite(DRE, LOW);
    while (Serial2.available() > 0)
    {
        GG = Serial2.read();
        Serial.write(GG);
        Serial.flush();
    }
```

### Controller debug
會出現掉資料的問題是緩存區滿了導致資料不全<br>
所以要進行擴充緩存區大小<br>
如果不用delay的話arduino可以很快的把資料取走就不會有滿出來的情況<br>
但是我們必須使用delay所以要增加緩存區大小好讓在滿出來前取走<br>
更改還存區大小要從 HardwareSerial.h 46跟53行更改
```C
#if !defined(SERIAL_TX_BUFFER_SIZE)
#if ((RAMEND - RAMSTART) < 1023) //如果arduino記憶體可用空間小於1KB 只能有16Byte可以使用
#define SERIAL_TX_BUFFER_SIZE 16
#else
#define SERIAL_TX_BUFFER_SIZE 64 //更改為128
#endif
#endif
#if !defined(SERIAL_RX_BUFFER_SIZE)
#if ((RAMEND - RAMSTART) < 1023) //如果arduino記憶體可用空間小於1KB 只能有16Byte可以使用
#define SERIAL_RX_BUFFER_SIZE 16
#else
#define SERIAL_RX_BUFFER_SIZE 64 //更改為128
#endif
#endif
```

### Controller bug
目前嚴重問題UART<br>
RS485半雙工 因為線路共用 會有衝突 解決了換成全雙工就沒問題<br> 



# Receiver<h2>是遠端車子端
### Receiver程式分類

DHT libraries 要改
DHT.cpp 86行 delay(250) 改成delay(10) 