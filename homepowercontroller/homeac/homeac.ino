#include <WiFi.h>
#include <PZEM004T.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

//雙核運行
TaskHandle_t Task1;

//PZEM
HardwareSerial PzemSerial2(2);
PZEM004T pzem(&PzemSerial2); // (RX,TX) connect to TX,RX of PZEM
int channelNo=1;      //0606計算顯示第幾電力支路變數
const IPAddress pzemip(192,168,1,1);

#define MQTT_VERSION MQTT_VERSION_3_1_1

// Wifi: SSID and password
const char* WIFI_SSID = "IoT office";
const char* WIFI_PASSWORD = "Iot5195911";

// MQTT: ID, server IP, port, username and password
const PROGMEM char* MQTT_CLIENT_ID = "office_switch";
const PROGMEM char* MQTT_SERVER_IP = "120.106.21.240";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "[Redacted]";
const PROGMEM char* MQTT_PASSWORD = "[Redacted]";

// MQTT: topics
const char* MQTT_LIGHT_STATE_TOPIC[3] = {"office/switch1/status","office/switch2/status","office/switch3/status"};
const char* MQTT_LIGHT_COMMAND_TOPIC[3] = {"office/switch1/switch","office/switch2/switch","office/switch3/switch"};

const char* MQTT_SENSOR_TOPIC[3] = {"office/sensor/switch1","office/sensor/switch2","office/sensor/switch3"};

// payloads by default (on/off)
const char* LIGHT_ON = "ON";
const char* LIGHT_OFF = "OFF";
const PROGMEM uint8_t PZEM_PIN[3] = {12,14,27};
const PROGMEM uint8_t LED_PIN[3] = {19,18,5};

boolean m_switch_state[3] = {false,false,false}; // switch is turned off by default

uint delaytime = 5000;
float cur = 0;      //電流
float vol = 0;      //電壓
float powe = 0;     //功率
float powehr = 0;


WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void publishData(uint8_t p_i, float p_voltage, float p_current, float p_power, float p_energy) {
  // create a JSON object
  // doc : https://github.com/bblanchon/ArduinoJson/wiki/API%20Reference
  StaticJsonDocument<200> jsonBuffer;
  JsonObject root = jsonBuffer.to<JsonObject>();
  // INFO: the data must be converted into a string; a problem occurs when using floats...
  root["value"] =  (String)p_i+1;
  root["voltage"] = (String)p_voltage;
  root["current"] = (String)p_current;
  root["power"] = (String)p_power;
  root["energy"] = (String)p_energy;
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
  client.publish(MQTT_SENSOR_TOPIC[p_i], data, true);
  yield();
}

void publishLightState(uint8_t i) {
  if (m_switch_state[i]) {
    client.publish(MQTT_LIGHT_STATE_TOPIC[i], LIGHT_ON, true);
  } else {
    client.publish(MQTT_LIGHT_STATE_TOPIC[i], LIGHT_OFF, true);
  }
}

// function called to turn on/off the switch
void setLightState(uint8_t i) {
  if (m_switch_state[i]) {
    DigitalWrite(LED_PIN[i], HIGH);
    Serial.println("INFO: Turn Switch" + String(i+1) + " on...");
  } else {
    DigitalWrite(LED_PIN[i], LOW);
    Serial.println("INFO: Turn Switch" + String(i+1) + " off...");
  }
}

// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
  // concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  
  // handle message topic
  for (uint8_t i = 0; i < 3; i++)
  {
    if (String(MQTT_LIGHT_COMMAND_TOPIC[i]).equals(p_topic))
    {
      // test if the payload is equal to "ON" or "OFF"
      if (payload.equals(String(LIGHT_ON)))
      {
        if (m_switch_state[i] != true)
        {
          m_switch_state[i] = true;
          setLightState(i);
          publishLightState(i);
        }
      }
      else if (payload.equals(String(LIGHT_OFF)))
      {
        if (m_switch_state[i] != false)
        {
          m_switch_state[i] = false;
          setLightState(i);
          publishLightState(i);
        }
      }
    }
  }
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID)) {
      Serial.println("INFO: connected");
      // Once connected, publish an announcement...
      // ... and resubscribe
      for (size_t i = 0; i < 3; i++)
      {
        //publishLightState(i); //發布者有啟用保留可以在重起時讀取舊數據可以不用預設關閉
        client.subscribe(MQTT_LIGHT_COMMAND_TOPIC[i]);
      }
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  xTaskCreatePinnedToCore(//雙核運行參數
             Task1code, /* Task function. */
             "Task1",   /* name of task. */
             10000,     /* Stack size of task */
             NULL,      /* parameter of the task */
             1,         /* priority of the task */
             &Task1,    /* Task handle to keep track of created task */
             0);        /* pin task to core 0 */
  setup_wifi();
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
  Serial.print("Connecting to PZEM...");
  while (true)
  {
    if (pzem.setAddress(pzemip))
      break;
    Serial.print(".");
    delay(500);
  }
}


void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void getdata(uint8_t i)
{
  cur = pzem.current(pzemip);
  Serial.print("Pzem Current(A): ");
  Serial.println(cur);
  vol = pzem.voltage(pzemip);
  Serial.print("Pzem Current(V): ");
  Serial.println(vol);
  powe = pzem.power(pzemip);
  Serial.print("Pzem Current(W): ");
  Serial.println(powe);
  powehr = pzem.energy(pzemip);
  Serial.print("Pzem Current(Wh): ");
  Serial.println(powehr);
  if (vol < 0) vol = 0;
  if (cur < 0) cur = 0;
  if (powe < 0) powe = 0;
  if (powehr < 0) powehr = 0;
  publishData(i, vol, cur, powe, powehr);
}

void EnergySwitch(uint8_t i ,uint TIME)
{
  DigitalWrite(PZEM_PIN[i],HIGH);
  delay(TIME);
  getdata(i);
  DigitalWrite(PZEM_PIN[i],LOW);
}

void Task1code(void *pvParameters) //雙核運行
{
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;)
  {
    for (size_t i = 0; i < 3; i++)
    {
      EnergySwitch(i, delaytime);
    }
  }
}

void DigitalWrite(int pinNumber, boolean status)
{
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, status);
}