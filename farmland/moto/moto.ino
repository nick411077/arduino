#include <WiFi.h>
#include <SPIFFS.h>            //檔案系統函數庫  
#include <AsyncTCP.h>          //異步處理函數庫 download https://github.com/me-no-dev/AsyncTCP
#include <ESPAsyncWebServer.h> //異步處理網頁伺服器函數庫 download https://github.com/me-no-dev/ESPAsyncWebServer




//雙核運行
TaskHandle_t Task1;

//微動開關設置
#define MS1 17
#define MS2 16
#define MS3 15
#define MS4 14

//WiFi設置
const char *ssid = "Lavender";
const char *password = "12345678";

//WebServer設置
AsyncWebServer server(80);

// Decode HTTP GET 設置
String DirValue = String(5);
String PwmValue = String(45);
String ModeValue = String(1);
//將 String轉換成int 
byte DIR;
byte PWM;
boolean mode;

//資料傳送Mega
byte Data[8] = {0x40,0x03,0x00,0x00,0x00,0x00,0x00,0x00};

void setup()
{
    if (!SPIFFS.begin())
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    Serial.begin(115200); //
    Serial2.begin(115200);
    // Connect to Wi-Fi network with SSID and password
    Serial.print("Connecting to ");
    Serial.println(ssid);             //顯示SSID
    WiFi.mode(WIFI_AP);               //WIFI AP模式
    WiFi.softAP(ssid, password);      //設定SSID 及密碼
    IPAddress myIP = WiFi.softAPIP(); //顯示ESP IP
    xTaskCreatePinnedToCore(//雙核運行參數
             Task1code, /* Task function. */
             "Task1",   /* name of task. */
             10000,     /* Stack size of task */
             NULL,      /* parameter of the task */
             1,         /* priority of the task */
             &Task1,    /* Task handle to keep track of created task */
             0);        /* pin task to core 0 */
    WebServer();
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.gatewayIP());
    server.begin();
}

void loop()
{
}

byte MicroSwitchRead()
{
    byte DIR = 0;
    if (digitalRead(MS1) == HIGH){DIR=+1;}
    if (digitalRead(MS2) == HIGH){DIR=+2;}
    if (digitalRead(MS3) == HIGH){DIR=+4;}
    if (digitalRead(MS4) == HIGH){DIR=+8;}
    return DIR;
}

void MegaWrite()
{
    Data[2] = MicroSwitchRead();
    Data[3] = DIR;
    Data[4] = PWM;
    Data[5] = mode;
    if (Serial2.availableForWrite() > sizeof(Data))
    {
        Serial2.write(Data,sizeof(Data));
        for (byte i = 0; i < sizeof(Data); i++)
        {
            Serial.print(Data[i],HEX);
            Serial.print(",");
        }
        Serial.println();
    }
}

void WebServer()
{
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        int args = request->args();
        //Serial.println(args);
        for (int i = 0; i < args; i++)
        {
            Serial.print("Param name: "); //列出GET名稱
            Serial.println(request->argName(i));
            Serial.print("Param value: ");
            Serial.println(request->arg(i));
            Serial.println("------");
            if (request->argName(i) == "dir") //GET網站方向狀態
            {
                DirValue = request->arg(i);
                DIR = DirValue.toInt();//將 String轉換成int  
            }
            else if (request->argName(i) == "pwm") //GET網站方向狀態
            {
                PwmValue = request->arg(i);
                PWM = PwmValue.toInt();//將 String轉換成int  
            }
            else if (request->argName(i) == "mode") //GET網站方向狀態
            {
                ModeValue = request->arg(i);
                mode = ModeValue.toInt();//將 String轉換成int  
            }
        }
        request->send(SPIFFS, "/index.html", "text/html");
    });
    //server.onRequestBody([](AsyncWebServerRequest *request));
    server.on("/jquery-3.3.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/jquery-3.3.1.min.js", "text/javascript"); });
    // Print local IP address and start web server
}

void Task1code(void *pvParameters) //雙核運行
{
    Serial.print("Task1 running on core ");
    Serial.println(xPortGetCoreID());
    
    for (;;)
    {
        MegaWrite();
        delay(10);
    }
}