#include <WiFi.h>
#include <SPIFFS.h>            //檔案系統函數庫  
#include <AsyncTCP.h>          //異步處理函數庫 download https://github.com/me-no-dev/AsyncTCP
#include <ESPAsyncWebServer.h> //異步處理網頁伺服器函數庫 download https://github.com/me-no-dev/ESPAsyncWebServer
#include <ArduinoJson.h>



//雙核運行
TaskHandle_t Task1;

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
//將 String轉換成int 
byte DIR = DirValue.toInt();
byte PWM = PwmValue.toInt();
boolean mode = ModeValue.toInt();

uint8_t status = 0; //為了loop不要重複運行設定狀態變數只運行一次

unsigned long lastTime = 0;
unsigned long DataDelay = 1000;

void setup()
{
    if (!SPIFFS.begin())
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    Serial.begin(115200); //
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
                status = 1;//更新狀態  
            }
            else if (request->argName(i) == "pwm") //GET網站方向狀態
            {
                PwmValue = request->arg(i);
                PWM = PwmValue.toInt();//將 String轉換成int
                status = 1;//更新狀態  
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
        if (status == 1) //讀取狀態
        {
            moto(DIR, PWM);
        }
        delay(1);
    }
}

void DigitalWrite(uint8_t pinNumber, uint8_t status)
{
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, status);
}

void AnalogWrite(int pinNumber, int LEDChannel, int value)
{
    ledcSetup(LEDChannel, 15000, 8);
    ledcAttachPin(pinNumber, LEDChannel);
    ledcWrite(LEDChannel, value);
}

void moto(int Value, int Power)
{
  byte PW = map(Power,0,100,0,250);
  byte PW1 = PW-100;
  byte PW2 = PW;
  if (PW<=100){PW1=0;}
  
  Serial.println(PW);
  Serial.println(PW1);
  Serial.println(PW2);
  switch (Value)
  { //控制
  case 1:
    DigitalWrite(18, HIGH);
    DigitalWrite(5, LOW);
    AnalogWrite(17, 1, PW2);
    DigitalWrite(32, LOW);
    DigitalWrite(33, HIGH);
    AnalogWrite(25, 2, PW1);
    break;
  case 2:
    DigitalWrite(18, HIGH);
    DigitalWrite(5, LOW);
    AnalogWrite(17, 1, PW2);
    DigitalWrite(32, LOW);
    DigitalWrite(33, HIGH);
    AnalogWrite(25, 2, PW2);
    break;
  case 3:
    DigitalWrite(18, HIGH);
    DigitalWrite(5, LOW);
    AnalogWrite(17, 1, PW1);
    DigitalWrite(32, LOW);
    DigitalWrite(33, HIGH);
    AnalogWrite(25, 2, PW2);
    break;
  case 4:
    DigitalWrite(18, HIGH);
    DigitalWrite(5, LOW);
    AnalogWrite(17, 1, PW2);
    DigitalWrite(32, HIGH);
    DigitalWrite(33, LOW);
    AnalogWrite(25, 2, PW2);
    break;
  case 5:
    DigitalWrite(18, HIGH);
    DigitalWrite(5, HIGH);
    AnalogWrite(17, 1, 0);
    DigitalWrite(32, HIGH);
    DigitalWrite(33, HIGH);
    AnalogWrite(25, 2, 0);
    break;
  case 6:
    DigitalWrite(18, LOW);
    DigitalWrite(5, HIGH);
    AnalogWrite(17, 1, PW2);
    DigitalWrite(32, LOW);
    DigitalWrite(33, HIGH);
    AnalogWrite(25, 2, PW2);
    break;
  case 7:
    DigitalWrite(18, LOW);
    DigitalWrite(5, HIGH);
    AnalogWrite(17, 1, PW2);
    DigitalWrite(32, HIGH);
    DigitalWrite(33, LOW);
    AnalogWrite(25, 2, PW1);
    break;
  case 8:
    DigitalWrite(18, LOW);
    DigitalWrite(5, HIGH);
    AnalogWrite(17, 1, PW2);
    DigitalWrite(32, HIGH);
    DigitalWrite(33, LOW);
    AnalogWrite(25, 2, PW2);
    break;
  case 9:
    DigitalWrite(18, LOW);
    DigitalWrite(5, HIGH);
    AnalogWrite(17, 1, PW1);
    DigitalWrite(32, HIGH);
    DigitalWrite(33, LOW);
    AnalogWrite(25, 2, PW2);
    break;
  }
  status = 0;
}