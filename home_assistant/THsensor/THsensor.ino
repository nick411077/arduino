#include <WiFi.h>
#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient
#include "DHT.h" //https://github.com/adafruit/DHT-sensor-library and https://github.com/adafruit/Adafruit_Sensor
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson

#define MQTT_VERSION MQTT_VERSION_3_1_1

// Wifi: SSID and password
const char* WIFI_SSID = "罐頭new";
const char* WIFI_PASSWORD = "nick520301";

// MQTT: ID, server IP, port, username and password
const PROGMEM char* MQTT_CLIENT_ID = "office_dht22";
const PROGMEM char* MQTT_SERVER_IP = "192.168.1.116";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "mqtt";
const PROGMEM char* MQTT_PASSWORD = "mqtt";

// MQTT: topic
const PROGMEM char* MQTT_SENSOR_TOPIC = "office/dht11";

// sleeping time
const PROGMEM uint16_t SLEEPING_TIME_IN_SECONDS = 10; // 10 minutes x 60 seconds

// DHT - D1/GPIO5
#define DHTPIN 33
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
WiFiClient wifiClient;
PubSubClient client(wifiClient);

// function called to publish the temperature and the humidity
void publishData(float p_temperature, float p_humidity) {
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
  client.publish(MQTT_SENSOR_TOPIC, data, true);
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

  dht.begin();

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
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("ERROR: Failed to read from DHT sensor!");
    return;
  } else {
    Serial.println(h);
    Serial.println(t);
    publishData(t, h);
  }

  Serial.println("INFO: Closing the MQTT connection");
  client.disconnect();

  Serial.println("INFO: Closing the Wifi connection");
  WiFi.disconnect();

  ESP.deepSleep(SLEEPING_TIME_IN_SECONDS * 1000000);
  delay(500); // wait for deep sleep to happen
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