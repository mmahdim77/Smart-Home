#include <Arduino.h>
#include <Ticker.h>
#include "Floor0.h"
#include "rtwtypes.h"
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"
#include <ArduinoJson.h>


static Floor0ModelClass rtObj;
// Instance of model class
SoftwareSerial BusSerial(BUSRX, BUSTX);
SoftwareSerial NodeMCUSerial(NODERX, NODETX);
DHT dht(DHTPIN, DHTTYPE);
char bufferWrite;
char bufferRead;
int bits[8];
char frames[3];
char registerAddress;
int optimalTempreture = 24;
float currTemprature=0;
float prevtemprature=0;

int button[4] = {0};
boolean selectedByMaster = false;
boolean shuldSendTemper = false;
void writeToBus(char registerAdd, int data)
{
  if (selectedByMaster)
  {
    // Serial.println("send status");
    char deviceAddress = 0b00001111;
    digitalWrite(BUSCTRL, HIGH);
    BusSerial.write(deviceAddress);
    BusSerial.write(registerAdd);
    BusSerial.write((char)data);
    // BusSerial.flush();
    digitalWrite(BUSCTRL, LOW);
    delay(10);
  }
}
void sendTemperStatus(){
  Serial.println("shuldSendTemper");
  Serial.println(shuldSendTemper);
  if(shuldSendTemper){
    Serial.println("send temper status func");
    Serial.print("temperatuere: ");
    Serial.println(currTemprature);
    Serial.print("optimal temperature: ");
    Serial.println(optimalTempreture);
    Serial.println(selectedByMaster);
    writeToBus(8, currTemprature);
    writeToBus(9, optimalTempreture);
  }
}

void InterruptSrevicesHandlerRead(char data, int pol)
{
  button[pol] = data;
  switch (pol)
  {
  case 0:
    rtObj.rtY.l1 = data ? true : false;
    digitalWrite(LED1, rtObj.rtY.l1 == true ? HIGH : LOW);
    break;

  case 1:
    rtObj.rtY.l2 = data ? true : false;
    digitalWrite(LED2, rtObj.rtY.l2 == true ? HIGH : LOW);
    break;
  case 2:
    rtObj.rtY.l3 = data ? true : false;
    digitalWrite(LED3, rtObj.rtY.l3 == true ? HIGH : LOW);
    break;
  case 3:
    rtObj.rtY.l4 = data ? true : false;
    digitalWrite(LED4, rtObj.rtY.l4 == true ? HIGH : LOW);
    break;

  default:
    break;
  }
}

void InterruptSrevicesHandler(int param)
{
  button[param-1] = !button[param-1];
  rtObj.changeButtonState(param);
  delay(2);
  if(param==1)
    registerAddress = 0b00000001;
  else if(param==2)
    registerAddress = 0b00000010;
  else if(param==3)
    registerAddress = 0b00000011;
  else if(param==4)
    registerAddress = 0b00000100;
  else
  // Serial.println("InterruptSrevicesHandler");
  writeToBus(registerAddress, button[param-1]);
}

void changeOptimalTemp(char param){
  // Serial.print("changeoptimal");
  if(param=='U')
    optimalTempreture++;
  else if (param=='D')
    optimalTempreture--;
  sendTemperStatus();
}

void rt_OneStep(void);
void rt_OneStep(void)
{
  static boolean_T OverrunFlag = false;

  // Disable interrupts here

  // Check for overrun
  if (OverrunFlag)
  {
    rtmSetErrorStatus(rtObj.getRTM(), "Overrun");
    return;
  }

  OverrunFlag = true;

  // Save FPU context here (if necessary)
  // Re-enable timer or interrupt here
  // Set model inputs here

  // Step the model
  rtObj.step();

  // Get model outputs here

  // Indicate task complete
  OverrunFlag = false;

  // Disable interrupts here
  // Restore FPU context here (if necessary)
  // Enable interrupts here
}

void sendStatusToMaster()
{
  int data = 0;
  data += (rtObj.rtY.l1 ? 1 : 0);
  data += (rtObj.rtY.l2 ? 2 : 0);
  data += (rtObj.rtY.l3 ? 4 : 0);
  data += (rtObj.rtY.l4 ? 8 : 0);
  writeToBus(0b10000000, data);
}



Ticker timer1(rt_OneStep, 200);

void setup()
{
  // put your setup code here, to run once:
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  pinMode(BUTTON3, INPUT);
  pinMode(BUTTON4, INPUT);
  pinMode(10, OUTPUT);
  digitalWrite(10,LOW);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  
  pinMode(COOLER, OUTPUT);
  // pinMode(11, OUTPUT);

  pinMode(BUSCTRL, OUTPUT);
  digitalWrite(BUSCTRL, LOW);

  Serial.begin(9600);
  Serial.setTimeout(100);
  rtObj.initialize();
  timer1.start();



  // NodeMCUSerial.begin(9600);
  // NodeMCUSerial.setTimeout(100);

  BusSerial.begin(9600);
  BusSerial.setTimeout(100);


  dht.begin();
  delay(2);

}

void loop()
{
  BusSerial.listen();
  prevtemprature = currTemprature;     // prev temperature
  currTemprature = dht.readTemperature();     // read temperature
  if(currTemprature>optimalTempreture){
    digitalWrite(COOLER, HIGH);
  }
  else{
    digitalWrite(COOLER, LOW);
  }
  
  // put your main code here, to run repeatedly:
  if(currTemprature != prevtemprature && shuldSendTemper){
    // Serial.println("send temper for master");

    sendTemperStatus();
    // delay(5);
    // NodeMCUSerial.listen();
    // delay(5);
    // if(NodeMCUSerial.available()>0)
    // {
    //   char c = NodeMCUSerial.read();
    //   Serial.println("write on nodemcu");
    //   if(c=='s')
    //   {
    //     Serial.println("s detected");
    //     StaticJsonDocument<200> doc;
    //     doc["temperature"] = currTemprature;
    //     doc["optimal"] = optimalTempreture;
    //     serializeJson(doc, NodeMCUSerial);
    //   }
    // }
    // delay(5);
    // NodeMCUSerial.flush();
    // BusSerial.listen();
  }
  if (BusSerial.available() > 0)
  {
    delay(2);
    Serial.println("bufferRead");

    int count =BusSerial.readBytes(frames, 3);
    // Serial.println("count:");
    // Serial.println(count);

    if(count==3){
      if (frames[0] == 2)
      {
        // Serial.print("frame 2");
        shuldSendTemper=false;
        selectedByMaster = true;
        if (frames[1] == -128)
        {
          sendStatusToMaster();
        }
        else
        {
          // Serial.println(int(frames[1]));
          switch (frames[1])
          {
          case 1:

            InterruptSrevicesHandlerRead(frames[2], 0);
            break;
          case 2:
            InterruptSrevicesHandlerRead(frames[2], 1);
            break;
          case 3:
            InterruptSrevicesHandlerRead(frames[2], 2);
            break;
          case 4:
            InterruptSrevicesHandlerRead(frames[2], 3);
            break;

          default:
            break;
          }
        }
      }
      else if (frames[0]==3){
        // Serial.print("frame 3 ");
        selectedByMaster = true;
        shuldSendTemper = true;
        if (frames[1] == -128)
        {
          // Serial.println("send temper");
          sendTemperStatus();
        }
        else if (frames[1]==1){
          changeOptimalTemp('U');
        }
        else if (frames[1]==2){
          changeOptimalTemp('D');
        }
      }
      else if((int)frames[0]!=0)
      {
        Serial.print("cancel: ");
        Serial.println((int)frames[0]);
        Serial.println(frames[1]);
        Serial.println(frames[2]);
        selectedByMaster = false;
        shuldSendTemper=false;
      }

    }
  }
  else if (digitalRead(BUTTON1))
  {
    InterruptSrevicesHandler(1);
    while (digitalRead(BUTTON1))
      ;
  }
  else if (digitalRead(BUTTON2))
  {
    InterruptSrevicesHandler(2);
    while (digitalRead(BUTTON2))
      ;
  }
  else if (digitalRead(BUTTON3))
  {
    InterruptSrevicesHandler(3);
    while (digitalRead(BUTTON3))
      ;
  }
  else if (digitalRead(BUTTON4))
  {
    InterruptSrevicesHandler(4);
    while (digitalRead(BUTTON4))
      ;
  }
  else if (digitalRead(TEMPU))
  {
    changeOptimalTemp('U');
    while (digitalRead(TEMPU))
      ;
  }
  else if (digitalRead(TEMPD))
  {
    changeOptimalTemp('D');
    while (digitalRead(TEMPD))
      ;
  }

  delay(5);
  timer1.update();
}