# ESP FS檔案系統

## 讓ESP Arduino-ide能夠使用而外檔案

從GitHub下載
[ESP8266FS](https://github.com/esp8266/arduino-esp8266fs-plugin/tags)
[ESP32FS](https://github.com/me-no-dev/arduino-esp32fs-plugin/tags)
檔案燒錄工具

## 解壓縮燒錄工具

將燒錄工具解壓縮到 Arduino/tools/資料夾裡
![img](./img/tools.png)

## 使用Arduino

重新開啟Arduino 檢查工具是否有出現 ESP32/ESP8266 Sketch Data Upload
![img](./img/Sketch_Data_Upload.png)

### 上傳測試

點擊ESP32/ESP8266 Sketch Data Upload
![img](./img/upload.png)  
確認是否你要的檔案

![done](./img/done.png)  
確認是否有成功

## 已知問題

不能在序列埠監控視窗開啟時燒錄
