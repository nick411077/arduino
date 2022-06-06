#include <WiFi.h>
#include <PubSubClient.h>
#include <RCSwitch.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson

#define MQTT_VERSION MQTT_VERSION_3_1_1

// Wifi: SSID and password
const char* WIFI_SSID = "Wang_Home";
const char* WIFI_PASSWORD = "Nick520301";

// MQTT: ID, server IP, port, username and password
const PROGMEM char* MQTT_CLIENT_ID = "garage_door";
const PROGMEM char* MQTT_SERVER_IP = "192.168.50.5";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "mqtt";
const PROGMEM char* MQTT_PASSWORD = "mqtt";

// MQTT: topics
const PROGMEM char* MQTT_SWITCH_STATUS_TOPIC = "homeassistant/cover/state";
const PROGMEM char* MQTT_SWITCH_COMMAND_TOPIC = "home/cover/set";
const PROGMEM char* MQTT_CONFIG_TOPIC = "homeassistant/cover/garage/config";

// default payload
const PROGMEM char* COVER_OPEN = "OPEN";
const PROGMEM char* COVER_CLOSE = "CLOSE";
const PROGMEM char* COVER_STOP = "STOP";

const PROGMEM int Hall_OPEN = 0;
const PROGMEM int Hall_CLOSED = 1;
const PROGMEM int Hall_CLOSING = 2;
const PROGMEM int Hall_OPENING = 3;
const PROGMEM int Hall_STOP = 4;

int m_cover_state = 0; // light is turned off by default

int SensorOpen = 26;
int SensorClose = 25;

int lastOpenState = Hall_STOP;
int lastCloseState = Hall_STOP;

long previousTime = 0;  // 用來保存前一次狀態的時間
long interval = 40000;   // 讀取間隔時間，單位為毫秒(miliseconds)
unsigned long currentTime;
boolean Timeout = false;

WiFiClient wifiClient;
PubSubClient client(wifiClient);
RCSwitch mySwitch = RCSwitch();

// function called on button press
// toggle the state of the switch

// function called to publish the state of the switch (on/off)
void current_state_set(int new_state)
{
  if (m_cover_state != new_state)
  {
    m_cover_state = new_state;
    stateChanged();
  }
}
boolean hall_state_changed()
{
  int SOpenState = digitalRead(SensorOpen);
  int SCloseState = digitalRead(SensorClose);
  if (SOpenState != lastOpenState || SCloseState != lastCloseState)
  {
    return true;
  }
  return false;
}

void hall_handle_state_change()
{
  const PROGMEM char* statusMsg = "OPEN";
  int OpenState = lastOpenState;
  int CloseState = lastCloseState;
  lastOpenState = digitalRead(SensorOpen);
  lastCloseState = digitalRead(SensorClose);
  if (OpenState!=lastOpenState)
  {
    if (lastOpenState == 1)
    {
      statusMsg = "change open";
      current_state_set(Hall_OPEN);
      Timeout = false;
    }
    else
    {
      Timeout = true;
      previousTime = currentTime;
      statusMsg = "change Time";
    }
    
    
  }
  if (CloseState!=lastCloseState)
  {
    if (lastCloseState == 1)
    {
      statusMsg = "change close";
      current_state_set(Hall_CLOSED);
      Timeout = false;
    }
    else
    {
      Timeout = true;
      previousTime = currentTime;
      statusMsg = "change Time";
    }
  }
  client.publish(MQTT_CONFIG_TOPIC, statusMsg, true);
}

void stateChanged() {

  const PROGMEM char* statusMsg = "OPEN";
  
  switch(m_cover_state) {
    case Hall_OPEN:
      statusMsg = "open";
      
      break;
    case Hall_CLOSED:
      statusMsg = "closed";
      
      break;
    case Hall_CLOSING:
      statusMsg = "closing";
      mySwitch.send("011111000100110000110000");
      delay(100);
      mySwitch.send("011111000100110011000000");
      digitalWrite(2,LOW);
      break;
    case Hall_OPENING:
      statusMsg = "opening";
      mySwitch.send("011111000100110000110000");
      delay(100);
      mySwitch.send("011111000100110000001100");
      digitalWrite(2,HIGH);
      break;
    case Hall_STOP:
      statusMsg = "stopped";
      mySwitch.send("011111000100110000110000");
      break;
  }
  client.publish(MQTT_SWITCH_STATUS_TOPIC, statusMsg, true);
}


// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
  // concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  // handle message topic
  if (String(MQTT_SWITCH_COMMAND_TOPIC).equals(p_topic)) {
    // test if the payload is equal to "ON" or "OFF"
    if (payload.equals(String(COVER_OPEN))) {
      if (m_cover_state != Hall_OPEN) {
        current_state_set(Hall_OPENING);
      }
    } else if (payload.equals(String(COVER_CLOSE))) {
      if (m_cover_state != Hall_CLOSED) {
        current_state_set(Hall_CLOSING);
      }
    } else if (payload.equals(String(COVER_STOP))) {
      if (m_cover_state != Hall_STOP) {
        current_state_set(Hall_STOP);
      }
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
      
      // Once connected, publish an announcement...
      // publish the initial values
      //

      // ... and resubscribe
      client.subscribe(MQTT_SWITCH_COMMAND_TOPIC);
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // init the serial
  Serial.begin(115200);
  //RF setup
  mySwitch.enableTransmit(27);
  mySwitch.setProtocol(1);
  mySwitch.setPulseLength(268);
  mySwitch.setRepeatTransmit(15);
  pinMode(2,OUTPUT);
  pinMode(SensorOpen,INPUT_PULLDOWN);
  pinMode(SensorClose,INPUT_PULLDOWN);
  
  // link the click function to be called on a single click event.   

  // init the WiFi connection
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  WiFi.mode(WIFI_STA);
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.println("INFO: IP address: ");
  Serial.println(WiFi.localIP());

  // init the MQTT connection
  
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
}

void loop() {
  currentTime = millis(); // 獲取時間用與判斷是否更新
  // keep watching the push button:
  delay(10);
  
  if (!client.connected()) {
    reconnect();
  }
  if (hall_state_changed())
  {
    hall_handle_state_change();
  }
  if (Timeout)
  {
    if (currentTime - previousTime > interval)
    {
    current_state_set(Hall_OPEN);
    Timeout = false;
    }
  }
  
  client.loop();
}
