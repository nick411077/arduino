/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <WiFi.h>
#include <Servo.h>



Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

// GPIO the servo is attached to

// Replace with your network credentials
const char* ssid     = "Lavender";
const char* password = "12345678";
const int servoPin = 27;

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Decode HTTP GET value
String valueString = String(90);
String BtnValue = String(5);
int pos1 = 0;
int pos2 = 0;
int pos4 = 0;
int pos5 = 0;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void DigitalWrite(int pinNumber, boolean status)
{
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, status);
}
void AnalogWrite(int pinNumber, int LEDChannel, int value)
{
  ledcSetup(LEDChannel, 5000, 8);
  ledcAttachPin(pinNumber, LEDChannel);
  ledcWrite(LEDChannel, value);
}

void setup() {
  Serial.begin(115200);
  myservo.attach(servoPin,5,0,180,500,2400);  // attaches the servo on the servoPin to the servo object
  myservo.write(valueString.toInt());
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_AP); //WIFI模式
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  // 序列阜顯示WiFi的連接狀況&WiFi的IP
  Serial.println("");
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(myIP);
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}");
            client.println(".slider { width: 300px; }</style>");
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
                     
            // Web Page
            client.println("</head><body><h1>ESP32 with Servo</h1>");
            client.println("<input type=\"button\" style=\"width:100px;height:100px;font-size:40px;\" class=\"push1\" id=\"motoSlider1\" value=\"\\\" name=\"1\"/>");
            client.println("<input type=\"button\" style=\"width:100px;height:100px;font-size:40px;\" class=\"push1\" id=\"motoSlider2\" value=\"^\" name=\"2\"/>");
            client.println("<input type=\"button\" style=\"width:100px;height:100px;font-size:40px;\" class=\"push1\" id=\"motoSlider3\" value=\"/\" name=\"3\"/><br>");
            client.println("<input type=\"button\" style=\"width:100px;height:100px;font-size:40px;\" class=\"push1\" id=\"motoSlider4\" value=\"<\" name=\"4\"/>");
            client.println("<input type=\"button\" style=\"width:100px;height:100px;font-size:40px;\" class=\"push1\" id=\"motoSlider5\" value=\"Stop\" name=\"5\"/>");
            client.println("<input type=\"button\" style=\"width:100px;height:100px;font-size:40px;\" class=\"push1\" id=\"motoSlider6\" value=\">\" name=\"6\"/><br>");
            client.println("<input type=\"button\" style=\"width:100px;height:100px;font-size:40px;\" class=\"push1\" id=\"motoSlider7\" value=\"/\" name=\"7\"/>");
            client.println("<input type=\"button\" style=\"width:100px;height:100px;font-size:40px;\" class=\"push1\" id=\"motoSlider8\" value=\"v\" name=\"8\"/>");
            client.println("<input type=\"button\" style=\"width:100px;height:100px;font-size:40px;\" class=\"push1\" id=\"motoSlider9\" value=\"\\\" name=\"9\"/>");
            client.println("<p>Position: <span id=\"servoPos\"></span> Moto: <span id=\"motoPos\"></span></p>");          
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider\" onchange=\"all_value(this.value,moto)\" value=\""+valueString+"\"/>");
            client.println("<script>");
            client.println("var push = document.getElementsByClassName(\"push1\");");
            client.println("var motoP = document.getElementById(\"motoPos\");");
            client.println("motoP.innerHTML = push[4].value;");
            client.println("for(var i=0;i<push.length;i++){");
            client.println("push[i].onclick = function (){");
            client.println("motoP.innerHTML = this.value; moto = this.name; all_value(slider.value,this.name);}}");
            client.println("var slider = document.getElementById(\"servoSlider\");");
            client.println("var servoP = document.getElementById(\"servoPos\");");
            client.println("servoP.innerHTML = slider.value;");
            client.println("slider.oninput = function() { slider.value = this.value; servoP.innerHTML = this.value; }");
            client.println("");
            client.println("$.ajaxSetup({timeout:1000}); function all_value(pos,pos3) { ");
            client.println("$.get(\"/?value=\" + pos + \"&moto=\" + pos3 + \"&\"); {Connection: close};}</script>");

            client.println("</body></html>");

            //GET /?value=180& HTTP/1.1
            if(header.indexOf("GET /?value=")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              pos4 = header.indexOf('=',16);
              pos5 = header.indexOf('&',17);
              valueString = header.substring(pos1+1, pos2);
              BtnValue = header.substring(pos4+1,pos5);
              
              //Rotate the servo
              Serial.println(valueString);
              Serial.println(BtnValue);
              myservo.write(valueString.toInt());
            }
            switch (BtnValue.toInt()) //控制
            {
            case 1: // 左前
              DigitalWrite(18, HIGH);
              DigitalWrite(5, LOW);
              AnalogWrite(25, 1,155);
              DigitalWrite(17, HIGH);
              DigitalWrite(16, LOW);
              AnalogWrite(26, 2,255);
              break;
            case 2: // 前
              DigitalWrite(18, HIGH);
              DigitalWrite(5, LOW);
              AnalogWrite(25, 1,255);
              DigitalWrite(17, HIGH);
              DigitalWrite(16, LOW);
              AnalogWrite(26, 2,255);
              break;
            case 3: // 右前
              DigitalWrite(18, HIGH);
              DigitalWrite(5, LOW);
              AnalogWrite(25, 1,255);
              DigitalWrite(17, HIGH);
              DigitalWrite(16, LOW);
              AnalogWrite(26, 2,155);
              break;
            case 4: // 左
              DigitalWrite(18, LOW);
              DigitalWrite(5, HIGH);
              AnalogWrite(25, 1,255);
              DigitalWrite(17, HIGH);
              DigitalWrite(16, LOW);
              AnalogWrite(26, 2,255);
              break;
            case 5: // 停
              DigitalWrite(18, HIGH);
              DigitalWrite(5, HIGH);
              AnalogWrite(25, 1,255);
              DigitalWrite(17, HIGH);
              DigitalWrite(16, HIGH);
              AnalogWrite(26, 2,255);
              break;
            case 6: // 右
              DigitalWrite(18, HIGH);
              DigitalWrite(5, LOW);
              AnalogWrite(25, 1,255);
              DigitalWrite(17, LOW);
              DigitalWrite(16, HIGH);
              AnalogWrite(26, 2,255);
              break;
            case 7: // 左後
              DigitalWrite(18, LOW);
              DigitalWrite(5, HIGH);
              AnalogWrite(25, 1,155);
              DigitalWrite(17, LOW);
              DigitalWrite(16, HIGH);
              AnalogWrite(26, 2,255);
              break;
            case 8: // 後
              DigitalWrite(18, LOW);
              DigitalWrite(5, HIGH);
              AnalogWrite(25, 1,255);
              DigitalWrite(17, LOW);
              DigitalWrite(16, HIGH);
              AnalogWrite(26, 2,255);
              break;
            case 9: // 右後
              DigitalWrite(18, LOW);
              DigitalWrite(5, HIGH);
              AnalogWrite(25, 1,255);
              DigitalWrite(17, LOW);
              DigitalWrite(16, HIGH);
              AnalogWrite(26, 2,155);
              break;
            }         
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
