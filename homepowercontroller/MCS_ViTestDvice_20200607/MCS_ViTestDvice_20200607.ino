#include <WiFi.h>
#include <MCS.h>
#include <PZEM004T.h>
HardwareSerial PzemSerial2(2);
PZEM004T pzem(&PzemSerial2); // (RX,TX) connect to TX,RX of PZEM

int channelNo=1;      //0606計算顯示第幾電力支路變數
IPAddress pzemip(192,168,1,1);

float cur =0;      //電流
float vol=0;      //電壓
float powe;     //功率
float pe;
float pw=0;
float pw1=0;
float pw2=0;
float kwh;      //總消耗功率
float temp=0;   //0412暫存功率計算的變數
float temp1;    //暫存總消耗功率的值，以防有0.01出現
float tp=0;     //總消耗功率
int  t=0;
int   x=0;      //0412計算總消耗功率的迴圈變數
int   cc=600;   //0606契約容量(contract capacity)，超過契約容量系統開始做電力管制
int   pc=0;    //20200607電力支路1的電力控制(power controll)標籤,pc>0代表現在系統正在管控電力
const char* ssid = "iphone";
const char* pass = "00000000";

MCSDevice mcs("DbreJyFf", "70czsc6qoroyAJju");
MCSControllerOnOff switch1("switch1");    //20200510彈性用電支路1網路開關
MCSControllerOnOff switch2("switch2");    //20200606彈性用電支路2網路開關
MCSControllerOnOff switch3("switch3");    //20200606彈性用電支路3網路開關
MCSControllerOnOff pwr_monitor("pwr_monitor");    //20200707是否自動省電的網路開關
MCSDisplayString pwr_channel("pwr_channel");  //20200606顯示第幾電力支路字串
MCSDisplayFloat v("v");     //MCS電壓
MCSDisplayFloat i("i");     //MCS電流
MCSDisplayFloat p("p");     //MCS功率
MCSDisplayFloat e("e");     //MCS總消耗功率
//==============================================================================
void DigitalWrite(int pinNumber, boolean status)
{
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, status);
}
void setup()
{
  Serial.begin(115200);
  mcs.addChannel(v);
  mcs.addChannel(i);
  mcs.addChannel(p);
  mcs.addChannel(e);
  mcs.addChannel(switch1);    //20200510
  mcs.addChannel(switch2);    //20200606
  mcs.addChannel(switch3);    //20200606
  mcs.addChannel(pwr_channel);  //20200606
  mcs.addChannel(pwr_monitor);  //20200707
  Serial.print("Wi-Fi connecting to");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wi-Fi connected successfully!");
  while (true) {
      Serial.print("Connecting to PZEM...");
      if(pzem.setAddress(pzemip))
        break;
      Serial.print(".");
      delay(500);
   }
  while (!mcs.connected()) {
    mcs.connect();
  }
  Serial.println("MCS connected successfully!");
}
//============================================================================
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
  MCS();
}




void loop()
{
  pe=0;
  DigitalWrite(12, HIGH); //電力支路1
  while (t < 5)
  {
    mcsswitch();
    t++;
    delay(1000);
  }
  t =0;
  getdata();
  pw1 = pw;
  DigitalWrite(12, LOW); 

  DigitalWrite(14, HIGH); //電力支路2
  while (t < 5)
  {
    mcsswitch();
    t++;
    delay(1000);
  }
  t =0;
  getdata();
  pw2 = pw;
  DigitalWrite(14, LOW); 
  
  DigitalWrite(27, HIGH); //電力支路3
  while (t < 5)
  {
    mcsswitch();
    t++;
    delay(1000);
  }
  t =0;
  getdata();
  pe = pw1 + pw2 + pw; //總消耗功率
  DigitalWrite(27, LOW); 
}
void MCS()
{
while (!mcs.connected()) {
    mcs.connect();
    if (mcs.connected()) {
      Serial.println("MCS 已重新連線");
    }
  }
  mcs.process(100);
//------------------------------功耗頻道--------------------------------
  Serial.print("Power Channel NO : ");
  Serial.println(channelNo);
  //------------------------------功耗統計--------------------------------
  powe = (cur * vol);
  temp = powe;
  tp = temp + tp;
  x = x + 1;
  //------------------------------功耗總計--------------------------------
  if (x == 3)
  {
    kwh = tp;
    x = 0;
    temp = 0;
    tp = 0;
    }
 //-----------------------------MCS電壓--------------------------------   
  if(vol>=0) 
  {
    v.set(vol);
    Serial.print("voltage : ");
    Serial.println(vol);
  }
//------------------------------MCS電流--------------------------------
  if(cur>=0)
  {
    i.set(cur);   
    Serial.print("current :");
    Serial.println(cur);
  }
 //------------------------------MCS功耗-------------------------------- 
  if(powe>=0)
  {
    p.set(powe);
    Serial.print("power :");
    Serial.println(powe);
  }
//------------------------------MCS總額----------------------------------  
  if(kwh>=0)
  {
    if(kwh==0.01)
    {
     e.set(temp1);  
//     mcsswitch();       
     Serial.print("total power :");
     Serial.println(temp1);
    Serial.println();
    }
  else
    {
     e.set(kwh);
     disp_channel();
     temp1=kwh;
     mcsswitch();
     Serial.print("total power :");
     Serial.println(kwh);
     Serial.println();
    }
    channelNo = channelNo + 1;
     if (channelNo == 4)
     channelNo = 1;
  }
  if(pwr_monitor.value())   //自動節電開關
    {
      if (temp1 > cc)
      {
        if (pc < 3)
        {
          Serial.println("Port01");
          DigitalWrite(5, HIGH); //電力支路1(p2)斷電
          DigitalWrite(18, LOW);  //電力支路2(p3)繼續供電
          DigitalWrite(19, LOW);  //電力支路3(p4)繼續供電
        }
        //-----------------------------------------------------------------------------------
        if (pc >= 3 and pc < 5)
        {
          DigitalWrite(5, HIGH); //電力支路1(p2)斷電
          DigitalWrite(18, HIGH); //電力支路2(p3)斷電
          DigitalWrite(19, LOW);  //電力支路3(p4)繼續供電
        }
        //-----------------------------------------------------------------------------------
        if (pc == 5)
        {
          DigitalWrite(5, HIGH); //電力支路1(p2)斷電
          DigitalWrite(18, HIGH); //電力支路2(p3)斷電
          DigitalWrite(19, HIGH); //電力支路3(p4)斷電
        }
        pc= pc +1;
        if (pc > 5)
          pc = 5;
        //-----------------------------------------------------------------------------------
      }
          else
          {
            DigitalWrite(5, LOW);    //電力支路1(p2)繼續供電
            DigitalWrite(18, LOW);    //電力支路2(p3)繼續供電
            DigitalWrite(19, LOW);    //電力支路3(p4)繼續供電
            pc=0;
          } 
    }
//-----------------------------------------------------------------------------------
if(pwr_monitor.updated())
  {
        if(pwr_monitor.value())   //自動節電開關
        {
              if (temp1 > cc)    
              {
                  if(pc<3)
                  {
                      DigitalWrite(5, HIGH);   //電力支路1(p2)斷電
                      DigitalWrite(18, LOW);    //電力支路2(p3)繼續供電
                      DigitalWrite(19, LOW);    //電力支路3(p4)繼續供電
                  } 
    //-----------------------------------------------------------------------------------
                  if(pc>=3 && pc<5)
                  {
                      DigitalWrite(5, HIGH);   //電力支路1(p2)斷電
                      DigitalWrite(18, HIGH);   //電力支路2(p3)斷電
                      DigitalWrite(19, LOW);    //電力支路3(p4)繼續供電           
                  } 
    //-----------------------------------------------------------------------------------
                  if(pc==5)
                  {
                      DigitalWrite(5, HIGH);   //電力支路1(p2)斷電
                      DigitalWrite(18, HIGH);   //電力支路2(p3)斷電
                      DigitalWrite(19, HIGH);   //電力支路3(p4)斷電
                  } 
                  pc=pc+1;
                  if(pc>5)
                  pc=5;
    //-----------------------------------------------------------------------------------
              }  
              else
              {
                DigitalWrite(5, LOW);    //電力支路1(p2)繼續供電
                DigitalWrite(18, LOW);    //電力支路2(p3)繼續供電
                DigitalWrite(19, LOW);    //電力支路3(p4)繼續供電
                pc=0;
              } 
        }    
  }
//----------------------------------------------------------------------------------
  Serial.println(pc);
}
//==========================================================================

void powercontroller()
{
  //------------------------超過契約容量(cc)系統開始做電力管制------------------------------------------------
    
}

void mcsswitch() //20200510 mcs的開關控制
{

  //---------------------------0506 IoT插座啟閉控制程式------------------------------------------------------------
  if (switch1.value())
  {
    if (pc <= 0)
    {
      DigitalWrite(5, LOW); //電力支路1(p2)繼續供電
    }
  }
  else
  {
    DigitalWrite(5, HIGH); //電力支路1(p2)斷電
  }
  //----------------------------0506 IoT插座啟閉控制程式--------------------------------------------
  if (switch2.value())
  {
    if (pc <= 3 && pc >= 5)
    {
      DigitalWrite(18, LOW); //電力支路2(p3)繼續供電
    }
  }
  else
  {
    DigitalWrite(18, HIGH); //電力支路2(p3)斷電
  }
  //----------------------------0506 IoT插座啟閉控制程式--------------------------------------------
  if (switch3.value()){
      if (pc < 5)
      {
        DigitalWrite(19, LOW); //電力支路3(p4)繼續供電
      }
  }
      else
      {
        DigitalWrite(19, HIGH); //電力支路3(p4)斷電
      }
  //------------------------------------------------------------------------

  if (switch1.updated())
  {
    if (switch1.value()){
        if (pc <= 0)
        {
          DigitalWrite(5, LOW); //電力支路1(p2)繼續供電
        }
    }
        else
        {
          DigitalWrite(5, HIGH); //電力支路1(p2)斷電
        }
  }
  //-----------------------------------------------------------------------

  if (switch2.updated())
  {
    if (switch2.value()){
      if (pc <= 3 && pc >= 5)
      {
        DigitalWrite(18, LOW); //電力支路2(p3)繼續供電
      }
    }
      else
      {
        DigitalWrite(18, HIGH); //電力支路2(p3)斷電
      }
  }
  //------------------------------------------------------------------------

  if (switch3.updated())
  {
    if (switch3.value()){
        if (pc < 5)
        {
          DigitalWrite(19, LOW); //電力支路3(p4)繼續供電
        }
    }
        else
        {
          DigitalWrite(19, HIGH); //電力支路3(p4)斷電
        }
  }
  //-------------------------------------------------------------------------
  /*  if(pwr_monitor.updated())
  {
        if(pwr_monitor.value())   //自動節電開關
        {
                if (temp1 > cc)    //超過契約容量(cc)系統開始做電力管制
                {
                    if(pc==0)
                    {
                        DigitalWrite(5, HIGH);   //電力支路1(p2)斷電
                        DigitalWrite(18, LOW);    //電力支路2(p3)繼續供電
                        DigitalWrite(19, LOW);    //電力支路3(p4)繼續供電
                        pc=pc+1;                 //pc=1              
                    } 
      //-----------------------------------------------------------------------------------
                    if(pc==1)
                    {
                        DigitalWrite(5, HIGH);   //電力支路1(p2)斷電
                        DigitalWrite(18, HIGH);   //電力支路2(p3)斷電
                        DigitalWrite(19, LOW);    //電力支路3(p4)繼續供電
                        pc=pc+1;                 //pc=2              
                    } 
      //-----------------------------------------------------------------------------------
                    if(pc==2)
                    {
                        DigitalWrite(5, HIGH);   //電力支路1(p2)斷電
                        DigitalWrite(18, HIGH);   //電力支路2(p3)斷電
                        DigitalWrite(19, LOW);    //電力支路3(p4)斷電
                                                 //pc=2              
                    } 
      //-----------------------------------------------------------------------------------
                }  
                else
                {
                  DigitalWrite(5, LOW);    //電力支路1(p2)繼續供電
                  DigitalWrite(18, LOW);    //電力支路2(p3)繼續供電
                  DigitalWrite(19, LOW);    //電力支路3(p4)繼續供電
                } 
        }
  }*/
  //-------------------------------------------------------------------------------
}
//===============================================================================

void disp_channel() //20200606 計算並顯示第幾個電力支路
{
  if (channelNo == 1)
    pwr_channel.set("電力支路1"); //20200606
  if (channelNo == 2)
    pwr_channel.set("電力支路2"); //20200606
  if (channelNo == 3)
    pwr_channel.set("電力支路3"); //20200606
}
//=========================================================================
