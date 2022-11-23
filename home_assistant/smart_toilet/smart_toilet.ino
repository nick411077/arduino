#include <WiFi.h>
#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
//#define SHT
#ifdef SHT
#include <SHT1x-ESP.h> //https://github.com/beegee-tokyo/SHT1x-ESP
#else
#include "DHT.h" //https://github.com/adafruit/DHT-sensor-library and https://github.com/adafruit/Adafruit_Sensor
#endif

#define MQTT_VERSION MQTT_VERSION_3_1_1

// Wifi: SSID and password
const char* WIFI_SSID = "罐頭new";
const char* WIFI_PASSWORD = "nick520301";

// MQTT: ID, server IP, port, username and password
const PROGMEM char* MQTT_CLIENT_ID = "smart_toilet";
const PROGMEM char* MQTT_SERVER_IP = "192.168.1.116";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "mqtt";
const PROGMEM char* MQTT_PASSWORD = "mqtt";

// MQTT: topic
const PROGMEM char* MQTT_TH_SENSOR_TOPIC = "toilet/TH";
const PROGMEM char* MQTT_GAS_SENSOR_TOPIC = "toilet/GAS";
const PROGMEM char* MQTT_SWITCH_STATUS_TOPIC = "toilet/people";


//TGS2602
#define airpin 36 

//pir_butten
#define buttonpin 10  //buttenPIN
#define sensorpin 32    //hc-sr505pin

const PROGMEM int Hall_OPEN = 0;
const PROGMEM int Hall_CLOSED = 1;
const PROGMEM int Hall_PEOPLE = 2;

int lastButtonState = Hall_OPEN;  // 按鈕的前一個狀態
int lastSensorState = Hall_OPEN;
int SButtonState = Hall_OPEN;
int SSensorState = Hall_OPEN;
long previousTime = 0;
long interval = 10000;
unsigned long currentTime;
boolean Timeout = false;

//雙核運行
TaskHandle_t Task1;

#ifdef
// Specify data and clock connections and instantiate SHT1x object
#define dataPin  22
#define clockPin 21
SHT1x sht1x(dataPin, clockPin);
#else
#define DHTPIN 22
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
#endif

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// function called to publish the temperature and the humidity
void publishTHData(float p_temperature, float p_humidity) {
  // create a JSON object
  // doc : https://github.com/bblanchon/ArduinoJson/wiki/API%20Reference
  StaticJsonDocument<200> jsonBuffer;
  JsonObject root = jsonBuffer.to<JsonObject>();
  // INFO: the data must be converted into a string; a problem occurs when using floats...
  root["temperature"] = (String)p_temperature;
  root["humidity"] = (String)p_humidity;
  serializeJsonPretty(root, Serial);
  Serial.println("");
  /*
     {
        "temperature": "23.20" ,
        "humidity": "43.70"
     }
  */
  char data[200];
  serializeJson(root, data, measureJson(root) + 1);
  client.publish(MQTT_TH_SENSOR_TOPIC, data, true);
  yield();
}

void publishGasData(float p_gas) {
  // create a JSON object
  // doc : https://github.com/bblanchon/ArduinoJson/wiki/API%20Reference
  StaticJsonDocument<200> jsonBuffer;
  JsonObject root = jsonBuffer.to<JsonObject>();
  // INFO: the data must be converted into a string; a problem occurs when using floats...
  root["gas"] = (String)p_gas;
  serializeJsonPretty(root, Serial);
  Serial.println("");
  /*
     {
        "temperature": "23.20" ,
        "humidity": "43.70"
     }
  */
  char data[200];
  serializeJson(root, data, measureJson(root) + 1);
  client.publish(MQTT_GAS_SENSOR_TOPIC, data, true);
  yield();
}

// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID,MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
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
  xTaskCreatePinnedToCore(//雙核運行參數
             Task1code, /* Task function. */
             "Task1",   /* name of task. */
             10000,     /* Stack size of task */
             NULL,      /* parameter of the task */
             1,         /* priority of the task */
             &Task1,    /* Task handle to keep track of created task */
             0);        /* pin task to core 0 */

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
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  #ifdef SHT
  float h = sht1x.readTemperatureF();
  // Read temperature as Celsius (the default)
  float t = sht1x.readTemperatureC();
  #else
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  #endif

  if (isnan(h) || isnan(t)) {
    Serial.println("ERROR: Failed to read from DHT sensor!");
    return;
  } else {
    Serial.println(h);
    Serial.println(t);
    publishTHData(t, h);
  }
  float sensor_volt;
  float RS_gas;
  float ratio;
  float ppm;
  float R0 = 30.0; 
  int sensorValue = analogRead(airpin); 
  sensor_volt = ((float)sensorValue / 4095) * 3.3;
  RS_gas = (5.0-sensor_volt)/sensor_volt; // Depend on RL on yor module 
  ratio = RS_gas / R0; // ratio = RS/R0 
  ppm = -46.7741935483871000000000000* pow(ratio,1) + 38.4193548387097000000000000;

  delay(5000); // wait for deep sleep to happen
}

void Task1code(void *pvParameters) //雙核運行
{
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;)
  {
    currentTime = millis(); // 獲取時間用與判斷是否更新
  if (hall_state_changed())
  {
    hall_handle_state_change();
  }
  if (Timeout)
  {
    if (currentTime - previousTime > interval)
    {
    stateChanged(Hall_OPEN);
    Timeout = false;
    }
  }
  delay(10);
  client.loop();
  }
}


void stateChanged(int state)
{

  const PROGMEM char *statusMsg = "OPEN";

  switch (state)
  {
  case Hall_OPEN:
    statusMsg = "open";

    break;
  case Hall_CLOSED:
    statusMsg = "closed";

    break;
  case Hall_PEOPLE:
    statusMsg = "people";
    break;
  }
  client.publish(MQTT_SWITCH_STATUS_TOPIC, statusMsg, true);
}

boolean hall_state_changed()
{
  SButtonState = digitalRead(buttonpin);
  SSensorState = digitalRead(sensorpin);
  if (SButtonState != lastButtonState || SSensorState != lastSensorState)
  {
    return true;
  }
  return false;
}

void hall_handle_state_change()
{
  const PROGMEM char *statusMsg = "OPEN";
  int ButtonState = lastButtonState;
  int SensorState = lastSensorState;
  lastButtonState = SButtonState;
  lastSensorState = SSensorState;
  if (ButtonState != lastButtonState || SensorState != lastSensorState)
  {
    if (lastButtonState == 0)
    {
      stateChanged(Hall_OPEN);
      Timeout = false;
    }
    else
    {
      if (lastSensorState == 1)
      {
        stateChanged(Hall_PEOPLE);
        Timeout = false;
      }
      else
      {
        Timeout = true;
        previousTime = currentTime;
      }
    }
  }
}

/*
mqtt:
  broker: 192.168.1.116

sensor:
  - platform: mqtt
    name: "Temperature"
    state_topic: "office/dht11"
    unit_of_measurement: "°C"
    value_template: "{{ value_json.temperature }}"
  - platform: mqtt
    name: "Humidity"
    state_topic: "office/dht11"
    unit_of_measurement: "%"
    value_template: "{{ value_json.humidity }}"
*/