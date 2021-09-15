#include <WiFi.h>
#include <PZEM004T.h>
#include <PubSubClient.h>

HardwareSerial PzemSerial2(2);
PZEM004T pzem(&PzemSerial2); // (RX,TX) connect to TX,RX of PZEM

int channelNo=1;      //0606計算顯示第幾電力支路變數
const IPAddress pzemip(192,168,1,1);
const IPAddress server(192,168,1,116);
const char* ssid = "IoT office";
const char* pass = "Iot5195911";

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
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(server, 1883);
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

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }
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