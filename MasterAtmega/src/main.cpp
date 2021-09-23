#include <Arduino.h>
#include <Ticker.h>
#include "Floor0.h"
#include "rtwtypes.h"
#include <SoftwareSerial.h>

static Toggle rtObj;
// Instance of model class
SoftwareSerial BusSerial(BUSRX, BUSTX);
char registerAddress;
char deviceAddress;
char bufferRead;
int bits[4];
int button[4] = {0};
char received[4];
float temperature;
int optimalTemperature;

int curr_dip = 0, last_dip = 0;

void writeToBus(char registerAddress, char deviceAddress, int data)
{
  digitalWrite(BUSCTRL, HIGH);
  Serial.println("write");
  Serial.println(int(deviceAddress));
  Serial.println(int(registerAddress));
  Serial.println(data);
  BusSerial.write(deviceAddress);
  BusSerial.write(registerAddress);
  BusSerial.write((char)data);
  // BusSerial.flush();
  digitalWrite(BUSCTRL, LOW);
  delay(10);
}

void InterruptSrevicesHandler1()
{
  button[0] = !button[0];
  rtObj.changeButtonState(1);
  delay(2);
  Serial.println("InterruptSrevicesHandler1");
  registerAddress = 0b00000001;
  writeToBus(registerAddress, deviceAddress, button[0]);
}
void InterruptSrevicesHandler2()
{
  button[1] = !button[1];
  rtObj.changeButtonState(2);
  delay(2);
  Serial.println("InterruptSrevicesHandler2");
  registerAddress = 0b00000010;
  writeToBus(registerAddress, deviceAddress, button[1]);
}
void InterruptSrevicesHandler3()
{
  button[2] = !button[2];
  rtObj.changeButtonState(3);
  delay(2);
  Serial.println("InterruptSrevicesHandler3");
  registerAddress = 0b00000011;
  writeToBus(registerAddress, deviceAddress, button[2]);
}

void InterruptSrevicesHandler4()
{
  button[3] = !button[3];
  rtObj.changeButtonState(4);
  delay(2);
  Serial.println("InterruptSrevicesHandler4");
  registerAddress = 0b00000100;
  writeToBus(registerAddress, deviceAddress, button[3]);
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

void synchToSlave(int salveNumber)
{
  writeToBus(0b10000000, char(salveNumber), 0b00000000);
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

Ticker timer1(rt_OneStep, 200);

void setup()
{
  // put your setup code here, to run once:
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  pinMode(BUTTON3, INPUT);
  pinMode(BUTTON4, INPUT);
  pinMode(DIPSELECT1, INPUT);
  pinMode(DIPSELECT2, INPUT);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  pinMode(BUSCTRL, OUTPUT);
  digitalWrite(BUSCTRL, LOW);

  Serial.begin(9600);
  Serial.setTimeout(100);

  rtObj.initialize();
  timer1.start();

  BusSerial.begin(9600);
  BusSerial.setTimeout(100);
  // delay(2);
  // Serial.println("Chera?");
}

void syncWithSlave(char data)
{

  button[0] = (data & 0b00000001) >> 0;
  button[1] = (data & 0b00000010) >> 1;
  button[2] = (data & 0b00000100) >> 2;
  button[3] = (data & 0b00001000) >> 3;
  InterruptSrevicesHandlerRead((data & 0b00000001) >> 0, 0);
  InterruptSrevicesHandlerRead((data & 0b00000010) >> 1, 1);
  InterruptSrevicesHandlerRead((data & 0b00000100) >> 2, 2);
  InterruptSrevicesHandlerRead((data & 0b00001000) >> 3, 3);
}
void synctemperWithSlave(float temper, int optimal){
  temperature = temper;
  optimalTemperature = optimal;
  Serial.print("temperature: ");
  Serial.println(temperature);
  Serial.print("temperature: ");
  Serial.println(optimalTemperature);
}

void bus(int phy, int reg, int dat)
{
  digitalWrite(10, HIGH);
  Serial.write(phy);
  Serial.write(reg);
  Serial.write(dat);
  Serial.flush();
  digitalWrite(10, LOW);
  delay(100);
}

void loop()
{
  // put your main code here, to run repeatedly:
  // Serial.print(BusSerial.available());
  curr_dip = (digitalRead(DIPSELECT2) * 2) + (digitalRead(DIPSELECT1));
  if (last_dip != curr_dip)
  {
    Serial.println("change dip switch");
    Serial.println(curr_dip);


    if (digitalRead(DIPSELECT1) && !digitalRead(DIPSELECT2))
    {
      deviceAddress = 0b00000001;
      //sync with slave
      writeToBus(0b10000000, char(1), 0b00000000);
    }
    else if (digitalRead(DIPSELECT2) && !digitalRead(DIPSELECT1))
    {
      deviceAddress = 0b00000010;
      writeToBus(0b10000000, char(2), 0b00000000);
    }
    else if (digitalRead(DIPSELECT2) && digitalRead(DIPSELECT1))
    {
      deviceAddress = 0b00000011;
      writeToBus(0b10000000, char(3), 0b00000000);
    }
    else
    {
      writeToBus(0b10000000, char(-1), 0b00000000);
    }
    last_dip = curr_dip;
  }
  if (BusSerial.available() > 0)
  {
    delay(2);

    BusSerial.readBytes(received, 3);
    if (received[0] == 15)
    {
      Serial.println("15 recived");
      switch (received[1])
      {
      case 1:
        InterruptSrevicesHandlerRead(received[2], 0);
        break;
      case 2:
        InterruptSrevicesHandlerRead(received[2], 1);
        break;
      case 3:
        InterruptSrevicesHandlerRead(received[2], 2);
        break;
      case 4:
        InterruptSrevicesHandlerRead(received[2], 3);
        break;
      case 8:
        Serial.print("temper recived : ");
        temperature = received[2];
        Serial.println(temperature);
        break;
      case 9:
        Serial.print("opttemper recived : ");
        optimalTemperature = received[2];
        Serial.println(optimalTemperature);
        break;
      case -128:
          syncWithSlave(received[2]);
        break;
      
      default:
        break;
      }
    }
    // buffer is your received data...
  }
  else if (digitalRead(BUTTON1))
  {
    InterruptSrevicesHandler1();
    while (digitalRead(BUTTON1))
      ;
  }
  else if (digitalRead(BUTTON2))
  {
    InterruptSrevicesHandler2();
    while (digitalRead(BUTTON2))
      ;
  }
  else if (digitalRead(BUTTON3))
  {
    InterruptSrevicesHandler3();
    while (digitalRead(BUTTON3))
      ;
  }
  else if (digitalRead(BUTTON4))
  {
    InterruptSrevicesHandler4();
    while (digitalRead(BUTTON4))
      ;
  }

  delay(2);
  timer1.update();
}

