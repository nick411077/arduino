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
    delay(5);  //目前問題加上了delay會有掉資料的問題
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

### Controller已知問題
增加delay會有掉接收資料問題<br>
如果沒有增加delay會有搖桿反應不佳<br>
delay5微秒大概是比較穩定的方法可以順利的讀取搖桿得值<br>
也可以比較減少掉資料問題(多少還是會掉資料)


# Receiver<h2>是遠端車子端
### Receiver程式分類
