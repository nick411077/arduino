#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

//雙核運行
TaskHandle_t Task1;

//WiFi設置
const char* ssid     = "TankWeb";
const char* password = "12345678";

//WebServer設置
AsyncWebServer server(80);

// Decode HTTP GET 設置
String Car = String(5);
String Pow = String(50); //網站請求的出力變數
//將 String轉換成int 
int CarValue = Car.toInt();
int PowValue = Pow.toInt();

uint8_t status = 0; //為了loop不要重複運行設定狀態變數只運行一次

void setup() {
  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }
  Serial.begin(115200);
  // Connect to Wi-Fi network with SSID and password
  xTaskCreatePinnedToCore(//雙核運行參數
             Task1code, /* Task function. */
             "Task1",   /* name of task. */
             10000,     /* Stack size of task */
             NULL,      /* parameter of the task */
             1,         /* priority of the task */
             &Task1,    /* Task handle to keep track of created task */
             0);        /* pin task to core 0 */
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_AP); //WIFI模式
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    int args = request->args();
    Serial.println(args);
    for (int i = 0; i < args; i++){
      Serial.print("Param name: ");
      Serial.println(request->argName(i));
      Serial.print("Param value: ");
      Serial.println(request->arg(i));
      Serial.println("------");
      if (request->argName(i) == "car")
      {
        Car = request->arg(i);
        CarValue = Car.toInt();//將 String轉換成int
        status = 1;//更新狀態
      }
      else if (request->argName(i) == "pow")//GET網站出力狀態
      {
        Pow = request->arg(i);
        PowValue = Pow.toInt();//將 String轉換成int
      }
    }
    request->send(SPIFFS, "/index.html", "text/html");
  });
  //server.onRequestBody([](AsyncWebServerRequest *request));
  server.on("/jquery-3.3.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/jquery-3.3.1.min.js", "text/javascript");
  });
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop()
{
}

void Task1code(void *pvParameters)//雙核運行
{
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;)
  {
    if (status == 1) //讀取狀態
    {
      moto(CarValue,PowValue);
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
    AnalogWrite(25, 1, PW2);
    DigitalWrite(32, LOW);
    DigitalWrite(33, HIGH);
    AnalogWrite(26, 2, PW1);
    break;
  case 2:
    DigitalWrite(18, HIGH);
    DigitalWrite(5, LOW);
    AnalogWrite(25, 1, PW2);
    DigitalWrite(32, LOW);
    DigitalWrite(33, HIGH);
    AnalogWrite(26, 2, PW2);
    break;
  case 3:
    DigitalWrite(18, HIGH);
    DigitalWrite(5, LOW);
    AnalogWrite(25, 1, PW1);
    DigitalWrite(32, LOW);
    DigitalWrite(33, HIGH);
    AnalogWrite(26, 2, PW2);
    break;
  case 4:
    DigitalWrite(18, HIGH);
    DigitalWrite(5, LOW);
    AnalogWrite(25, 1, PW2);
    DigitalWrite(32, HIGH);
    DigitalWrite(33, LOW);
    AnalogWrite(26, 2, PW2);
    break;
  case 5:
    DigitalWrite(18, HIGH);
    DigitalWrite(5, HIGH);
    AnalogWrite(25, 1, 0);
    DigitalWrite(32, HIGH);
    DigitalWrite(33, HIGH);
    AnalogWrite(26, 2, 0);
    break;
  case 6:
    DigitalWrite(18, LOW);
    DigitalWrite(5, HIGH);
    AnalogWrite(25, 1, PW2);
    DigitalWrite(32, LOW);
    DigitalWrite(33, HIGH);
    AnalogWrite(26, 2, PW2);
    break;
  case 7:
    DigitalWrite(18, LOW);
    DigitalWrite(5, HIGH);
    AnalogWrite(25, 1, PW2);
    DigitalWrite(32, HIGH);
    DigitalWrite(33, LOW);
    AnalogWrite(26, 2, PW1);
    break;
  case 8:
    DigitalWrite(18, LOW);
    DigitalWrite(5, HIGH);
    AnalogWrite(25, 1, PW2);
    DigitalWrite(32, HIGH);
    DigitalWrite(33, LOW);
    AnalogWrite(26, 2, PW2);
    break;
  case 9:
    DigitalWrite(18, LOW);
    DigitalWrite(5, HIGH);
    AnalogWrite(25, 1, PW1);
    DigitalWrite(32, HIGH);
    DigitalWrite(33, LOW);
    AnalogWrite(26, 2, PW2);
    break;
  }
  status = 0;
}