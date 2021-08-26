#include <WiFi.h>
#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Float64.h>
#include <rosserial_arduino/Adc.h>
#include <geometry_msgs/Twist.h>

//wifi賬號密碼
const char* ssid = "罐頭new";
const char* password = "nick520301";

IPAddress server(192, 168, 1, 116);//ros伺服器
IPAddress ip_address;//ip地址
int status = WL_IDLE_STATUS;//它是調用WiFi .begin()時分配的臨時狀態，並保持活動狀態直到嘗試次數到期

WiFiClient client;//創建一個客戶端，該客戶端可以連接到client.connect() 中定義的指定 Internet IP 地址和端口。

//連接ros伺服器
class WiFiHardware {

  public:
  WiFiHardware() {};

  void init() {
    // 連接到ROS伺服器
    client.connect(server, 11411);
  }


  int read() {
    return client.read(); //讀取ROS伺服器字節
  }


  void write(uint8_t* data, int length) {
    // 將數據寫錄
    for(int i=0; i<length; i++)
      client.write(data[i]);
  }

  unsigned long time() {
     return millis(); //回傳扳子執行時間
  }
};

void setupWiFi()
{
  WiFi.begin(ssid, password);//連接wifi(賬號,密碼)
  Serial.print("\nConnecting to "); Serial.println(ssid);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
  if(i == 21){
    Serial.print("Could not connect to"); Serial.println(ssid);
    while(1) delay(500);
  }
  Serial.print("Ready! Use ");
  Serial.print(WiFi.localIP());
  Serial.println(" to access client");
}

std_msgs::String str_msg;
ros::NodeHandle_<WiFiHardware> nh;//創建節點
ros::Subscriber<std_msgs::String> sub("message", &chatterCallback);


void chatterCallback(const std_msgs::String& msg) {
  String T = msg.data;
  Serial.print(T);
}

void setup() {

  Serial.begin(115200);
  setupWiFi(); // 連接wifi
  delay(2000);

  nh.initNode(); //節點初始化
  nh.subscribe(sub); //添加主節點
}

void loop(){
    nh.spinOnce();
    delay( 10 );
}