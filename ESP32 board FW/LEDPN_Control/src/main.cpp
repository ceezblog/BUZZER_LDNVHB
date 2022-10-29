// This is for platformIO 


// CHANGE THIS FOR EVER PANEL
#define PANEL_ID 1 // this is also server as adress, ie 101...


#include <Arduino.h>
#include <stdio.h>
#include "driver/uart.h"

#include <WiFi.h>
#include <WiFiUdp.h>
#include "bz_udp_helper.h"
#include "_data_processing.h"

//#define _RAIN_DROP_ENABLE

#ifdef _RAIN_DROP_ENABLE
#include "Circle.h"
void animateDrops();
RainDrop drops;
bool screensaver;
#endif

#define SERIAL1_TX 21 
#define SERIAL1_RX 18 

#define SERIAL2_TX 5 
#define SERIAL2_RX 23

enum BUTTON_TYPE : uint8_t
{ BY_NONE = 0, BY_A = 1, BY_B = 2, BY_C = 3, BY_D = 4, BY_CLEAR = 5, BY_BUZZ = 6};

#ifdef _RAIN_DROP_ENABLE
void animateDrops()
{
  drops.dropAnother();
  drops.animateCircles();
}
#endif

volatile bool WIFI_OK; // declare at wifi_control.h
volatile bool HOST_OK;
volatile uint8_t byteBuffer;


bool buzz1;
bool buzz2;
bool screensaver;

unsigned long bzmicro;
unsigned long mcmicro;


void setup() {

  //EEPROM.begin(EEPROM_SIZE);
  //EEPROM.get(0, data_text.team);
  //EEPROM.put(0, data_text.team);
  //EEPROM.commit();
  
  //pull up for SERIAL RXs, pull up doesn't work at all
  //pinMode(PULLUP_HIGH_PIN, OUTPUT);
  //digitalWrite(PULLUP_HIGH_PIN, HIGH);
  uint8_t mc1 = 0;
  uint8_t mc2 = 0;
 
  Serial.begin(115200);
  Serial1.begin(19200, SERIAL_8N1, SERIAL1_RX, SERIAL1_TX);
  Serial2.begin(19200, SERIAL_8N1, SERIAL2_RX, SERIAL2_TX);

  // init wifi
  btStop();
  setIP();
  
  display.begin(16); //scan 1/16
  start_ledpn();
  showCenter("---");
  delay(1000);

  stop_ledpn();

  WIFI_OK =  Connect_Wifi_BZ(); // 5s to connect to wifi
  buzz1 = 0;
  buzz2 = 0;
  screensaver = 0;

  start_ledpn();

  if (WIFI_OK) { showFooter("wifi ok"); }
  else showFooter("wifi off");
  delay(2000);
  _clearScreen();
  
  memcpy(data_text.team, "LD NVHB\0",8);
  memcpy(data_text.name1, "LD NVHB\0",8);
  memcpy(data_text.name2, "LD NVHB\0",8);
  showHeader(data_text.team);
  
  Serial.print("Init done, running buzzer.\n");
  
#ifdef _RAIN_DROP_ENABLE
  drops.init();
#endif

//infinity loop C++ style
  while(1)
  {
    //_showBuzz(); //debugging
    byteBuffer = 0;
    if (WIFI_OK) _read_UDP(); // read UDP only if wifi is OK
    
    // if player 1 send data
    while (Serial1.available()) {
      byteBuffer = Serial1.read();
      if (byteBuffer != Serial1.read()) //second byte isn't the same?
      {
        byteBuffer = 0;
        break; // second read get different, return to infinity loop
      }
      
      bool valid = 0; //interpret  message
      switch(byteBuffer)
      {
        case BY_A: //ABCD obviously
        case BY_B:
        case BY_C:
        case BY_D:          
          valid = 1;
          mcmicro = micros();
          if (mc1==byteBuffer) break; // if receiving the same as previous
          mc1 = byteBuffer;
          if (WIFI_OK) send_UDP_TRIGGER(M_TRIGGER_BZ + byteBuffer);
          _showMC(byteBuffer, 1);
          break;
        
        case BY_CLEAR: // clear display
          valid = 1;
          _clearCenter();
          break;
        
        case BY_BUZZ: // buzzing, display bell icon
          valid = 1;
          bzmicro = micros(); 
          if (buzz1) break;
          buzz1 = 1;
          buzz2 = 0;
          _clearCenter();
          if (WIFI_OK) send_UDP_TRIGGER(M_TRIGGER_BZ);
          _showBuzz1(); 
          break;

        default:
          //just do nothing if it is unwanted message
          break;
      }
      byteBuffer = 0;
      
      if (valid) // got valid, discard the rest
      {
        //delay(100);
        Serial1.flush();
        break;
      }
    }

    while (Serial2.available()) {
      byteBuffer = Serial2.read();
      if (byteBuffer != Serial2.read()) //second byte isn't the same?
      {
        byteBuffer = 0;
        break; // second read get different, return to infinity loop
      }
      bool valid = 0;
      switch(byteBuffer)
      {
        case BY_A:
        case BY_B:
        case BY_C:
        case BY_D:
          valid = 1;
          mcmicro = micros();
          if (mc2==byteBuffer) break;
          mc2 = byteBuffer;
          if (WIFI_OK) send_UDP_TRIGGER(M_TRIGGER_BZ + byteBuffer);
          _showMC(byteBuffer, 2);
          break;

        case BY_CLEAR:
          valid = 1;
          _clearCenter();
          break;
        
        case BY_BUZZ:
          valid = 1;
          bzmicro = micros(); 
          if (buzz2) break;
          buzz1 = 0;
          buzz2 = 1;
          _clearCenter();
          if (WIFI_OK) send_UDP_TRIGGER(M_TRIGGER_BZ);
          _showBuzz2(); 
          break;

        default:
          break;
      }
      byteBuffer = 0;
      
      if (valid)// got valid, discard the rest
      {
        //delay(100);
        Serial2.flush();
        break;
      }
    }

    if (buzz1 || buzz2) 
    { 
      if (micros() - bzmicro > 1000000) 
      {
        buzz1 = 0;
        buzz2 = 0;
        _clearCenter();
      } 
    }
    
    if (mc1 || mc2)
    {
      if (micros() - mcmicro > 1000000) 
      {
        mc1 = 0;
        mc2 = 0;
        _clearCenter();
      } 
    }

  } //infinity loop

} // end of setup()



// leave it empty since I have no use for this
void loop()
{
}



