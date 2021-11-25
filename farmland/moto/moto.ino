#include <WiFi.h>
#include <SPIFFS.h>            //檔案系統函數庫  
#include <AsyncTCP.h>          //異步處理函數庫 download https://github.com/me-no-dev/AsyncTCP
#include <ESPAsyncWebServer.h> //異步處理網頁伺服器函數庫 download https://github.com/me-no-dev/ESPAsyncWebServer
#include <ArduinoJson.h>



//雙核運行
TaskHandle_t Task1;

//微動開關設置
#define MS1 36
#define MS2 39
#define MS3 34
#define MS4 35

//WiFi設置
const char *ssid = "Lavender";
const char *password = "12345678";

//WebServer設置
AsyncWebServer server(80);
AsyncEventSource events("/events");

// Decode HTTP GET 設置
String DirValue = String(5);
String PwmValue = String(45);
String ModeValue = String(0);
String SwValue = String(30);
//將 String轉換成int 
byte DIR = DirValue.toInt();
byte PWM = PwmValue.toInt();
boolean mode = ModeValue.toInt();
byte SW = SwValue.toInt();

unsigned long lastTime = 0;
unsigned long DataDelay = 1000;

//收發Mega資料
byte WriteData[8] = {0x40,0x03,0x00,0x00,0x00,0x00,0x00,0x00};

byte ReadData[8];

void setup()
{
    if (!SPIFFS.begin())
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    pinMode(MS1,INPUT);
    pinMode(MS2,INPUT);
    pinMode(MS3,INPUT);
    pinMode(MS4,INPUT);
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
    server.addHandler(&events);
    server.begin();

    
}

void loop()
{
    if ((millis() - lastTime) > DataDelay)
    {
        // Send Events to the Web Server with the Sensor Readings
        events.send(getValue().c_str(), "data_readings", millis());
        lastTime = millis();
    }
}

byte MicroSwitchRead()
{
    byte ADIR = 0;
    if (digitalRead(MS1) == HIGH){ADIR+=1;}
    if (digitalRead(MS2) == HIGH){ADIR+=2;}
    if (digitalRead(MS3) == HIGH){ADIR+=4;}
    if (digitalRead(MS4) == HIGH){ADIR+=8;}
    return ADIR;
}

void MegaWrite()
{
    WriteData[2] = MicroSwitchRead();
    WriteData[3] = DIR;
    WriteData[4] = PWM;
    WriteData[5] = mode;
    WriteData[6] = SW;
    if (Serial2.availableForWrite() > sizeof(WriteData))
    {
        Serial2.write(WriteData,sizeof(WriteData));
        for (byte i = 0; i < sizeof(WriteData); i++)
        {
            Serial.print(WriteData[i],HEX);
            Serial.print(",");
        }
        Serial.println();
    }
}

void MegaRead()
{
    
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
            else if (request->argName(i) == "sw") //GET網站方向狀態
            {
                SwValue = request->arg(i);
                SW = SwValue.toInt();//將 String轉換成int  
            }
        }
        request->send(SPIFFS, "/index.html", "text/html");
    });
    //server.onRequestBody([](AsyncWebServerRequest *request));
    server.serveStatic("/", SPIFFS, "/");
    // Print local IP address and start web server
    server.on("/state", HTTP_GET, [] (AsyncWebServerRequest *request) 
    {
        request->send(200, "text/plain", getValue().c_str());
    });
    events.onConnect([](AsyncEventSourceClient *client)
    {
        if (client->lastId())
        {
            Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
        }
        // send event with message "hello!", id current millis
        // and set reconnect delay to 1 second
        client->send("hello!", NULL, millis(), 10000);
    });
}

String getValue()
{
    StaticJsonDocument<200> jsonBuffer;
  JsonObject root = jsonBuffer.to<JsonObject>();
  // INFO: the data must be converted into a string; a problem occurs when using floats...
  root["dir"] = (String)DirValue;
  root["pwm"] = (String)PwmValue;
  root["mode"] = (String)ModeValue;
  root["sw"] = (String)SwValue;
  serializeJsonPretty(root, Serial);
  Serial.println("");
  /*
     {
        "voltage": "123.20",
        "current": "3.70",
        "power": "5.60",
        "energy": "4.47"
     }
  */
  char data[200];
  serializeJson(root, data, measureJson(root) + 1);
  yield();
  return data;
}

void Task1code(void *pvParameters) //雙核運行
{
    Serial.print("Task1 running on core ");
    Serial.println(xPortGetCoreID());
    
    for (;;)
    {
        MegaWrite();
        delay(100);
    }
}