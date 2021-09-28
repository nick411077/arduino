#include <WiFi.h>
#include <PZEM004T.h>
#include <PubSubClient.h>

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
const PROGMEM char* MQTT_CLIENT_ID = "office_light";
const PROGMEM char* MQTT_SERVER_IP = "120.106.21.240";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "[Redacted]";
const PROGMEM char* MQTT_PASSWORD = "[Redacted]";

// MQTT: topics
const char* MQTT_LIGHT_STATE_TOPIC[3] = {"office/light1/status","office/light2/status","office/light3/status"};
const char* MQTT_LIGHT_COMMAND_TOPIC[3] = {"office/light1/switch","office/light2/switch","office/light3/switch"};

// payloads by default (on/off)
const char* LIGHT_ON = "ON";
const char* LIGHT_OFF = "OFF";

const PROGMEM uint8_t LED_PIN[3] = {5,18,19};
boolean m_light_state[3] = {false,false,false}; // light is turned off by default

float cur =0;      //電流
float vol=0;      //電壓
float powe;     //功率
float pe;
float pw=0;

WiFiClient espClient;
PubSubClient client(espClient);

void DigitalWrite(int pinNumber, boolean status)
{
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, status);
}

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

void publishLightState(uint8_t i) {
  if (m_light_state[i]) {
    client.publish(MQTT_LIGHT_STATE_TOPIC[i], LIGHT_ON, true);
  } else {
    client.publish(MQTT_LIGHT_STATE_TOPIC[i], LIGHT_OFF, true);
  }
}

// function called to turn on/off the light
void setLightState(uint8_t i) {
  if (m_light_state[i]) {
    digitalWrite(LED_PIN[i], HIGH);
    Serial.println("INFO: Turn light on...");
  } else {
    digitalWrite(LED_PIN[i], LOW);
    Serial.println("INFO: Turn light off...");
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
        if (m_light_state[i] != true)
        {
          m_light_state[i] = true;
          setLightState(i);
          publishLightState(i);
        }
      }
      else if (payload.equals(String(LIGHT_OFF)))
      {
        if (m_light_state[i] != false)
        {
          m_light_state[i] = false;
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
        publishLightState(i);
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
  setup_wifi();
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
  while (true)
  {
    Serial.print("Connecting to PZEM...");
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

void getdata()
{
  cur = pzem.current(pzemip);
  //<<0609test
  Serial.print("Pzem Current(A): ");
  Serial.println(cur);
  //0609test>> 
       
  vol = pzem.voltage(pzemip);
//<<20200422
  if (vol < 0) vol = 0;
  if (cur < 0) cur = 0;
  if (pw < 0) pw = 0;

}