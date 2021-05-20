#include <Arduino.h>
#include <Arduino_JSON.h>
#include "WiFi.h"
#include "MCS.h"
#include "TinyGPS.h"
#include "MPU9250.h"
#include "painlessMesh.h"

//Mesh初始設定
#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

//Pin設定
int trigPin =27;                  //Trig Pin
int echoPin =26;                  //Echo Pin
int buzzerPin =32;
long Front,Back;

//
const char* ssid = "iphone";
const char* pass = "00000000";

//MCS初始設定
MCSDevice mcs("DzmJU7LW", "Qpf4twi0atPN0pOr");
MCSDisplayInteger front("Ultrasound");
MCSDisplayInteger back("radar");
MCSDisplayInteger PSI("psi");
MCSDisplayGPS GPS("GPS");
MCSDisplayInteger X("X");
MCSDisplayInteger Y("Y");
MCSDisplayInteger Z("Z");

TinyGPS gps;
MPU9250 mpu;
Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);
  Wire.begin();
  
  //MCS設定
  mcs.addChannel(front);
  mcs.addChannel(back);
  mcs.addChannel(PSI);
  mcs.addChannel(GPS);
  mcs.addChannel(X);
  mcs.addChannel(Y);
  mcs.addChannel(Z);

  //Mesh設定
  mesh.setDebugMsgTypes( ERROR | STARTUP );   // set before init() so that you can see startup messages
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6 );
  mesh.initOTAReceive("bridge");
  mesh.stationManual(ssid, pass);
  mesh.setRoot(true);
  mesh.setContainsRoot(true);
  mesh.onReceive(&receivedCallback);
  Serial.print("Wi-Fi connecting to");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wi-Fi connected successfully!");
  while (!mcs.connected()) {
    mcs.connect();
  }
  /*if (!mpu.setup(0x68)) {  // change to your own address
        while (1) {
            Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
            delay(5000);
        }
    }*/
  Serial.println("MCS connected successfully!");
}

void loop()
{
  while (!mcs.connected()) {
  	mcs.connect();
  	if (mcs.connected()) { Serial.println("MCS 已重新連線"); }
  }
  mcs.process(100);
  mesh.update();
  Front = Ultrasound(27,26);
  //front.set(Front);
  Serial.print("front : ");
  Serial.println(Front);
  Back = Ultrasound(13,14);
  //back.set(Back);
  Serial.print("back : ");
  Serial.println(Back);
  gpsdata();
  /*if (mpu.update()) {
        static uint32_t prev_ms = millis();
        if (millis() > prev_ms + 25) {
            print_roll_pitch_yaw();
            prev_ms = millis();
        }
  }*/
  delay(100);
}

void gpsdata()
{
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial2.available())
    {
      char c = Serial2.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
    //GPS.set(flat,flon,0);
  }
  
  gps.stats(&chars, &sentences, &failed);
  Serial.print(" CHARS=");
  Serial.print(chars);
  Serial.print(" SENTENCES=");
  Serial.print(sentences);
  Serial.print(" CSUM ERR=");
  Serial.println(failed);
  if (chars == 0)
    Serial.println("** No characters received from GPS: check wiring **");
}

int Ultrasound(int trigPin, int echoPin)
{
  long duration;
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  duration = (duration/2) / 29.1;
  return duration;
}

void print_roll_pitch_yaw(){
  float Yaw,Pitch,Roll;
  Yaw = (mpu.getYaw(),2);
  Pitch = (mpu.getPitch(),2);
  Roll = (mpu.getRoll(),2);
  Serial.print("Yaw, Pitch, Roll: ");
  Serial.print(Yaw);
  X.set(Yaw);
  Serial.print(", ");
  Serial.print(Pitch);
  Y.set(Pitch); 
  Serial.print(", ");
  Serial.println(Roll);
  Z.set(Roll);
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  JSONVar myObject = JSON.parse(msg.c_str());
  int psi = myObject["psi"];
  Serial.println(psi);
  //PSI.set(psi);
}
