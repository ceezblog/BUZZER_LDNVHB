#define DEBUG

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#define _TRACE(x) if (DEBUG_TEST) Serial.print(x)
#define _TRACELN(x) if (DEBUG_TEST) Serial.println(x)

#ifndef _M_ARDUINO
#define _M_ARDUINO
#include <Arduino.h>
#endif

#ifndef _M_WIFI
#define _M_WIFI
#include <WiFi.h>
#include <WiFiUdp.h>
#endif

#ifndef _M_UDP_HELPER
#define _M_UDP_HELPER
#include "bz_udp_helper.h"
#endif

#include "_esp32_PxLED.h"
#include "_colorDef16.h"
#include "_str_helper.h"

#include <EEPROM.h>

#define bstrcpy(dest, src) strcpy((char*) dest, (char*) src)


#define EEPROM_SIZE 512 //let use 1k, data_text size is 52
#define EEAD_TEAM 0
#define EEAD_PLAYER1 20
#define EEAD_PLAYER2 20
#define EEAD_AUX 20

// good font:
// FreeMonoBoldOblique9pt7b.h //5 chars
// FreeSansBoldOblique9pt7b.h //5 chars
// FreeSerif9pt7b.h //6 chars
// FreeSerifBoldItalic9pt7b.h // 6 chars
//

//custom char 19x24 // vertical scan instead // each vertical line = 3 bytes
char bell[57] = { 
  0x00, 0x00, 0x04, 
  0x00, 0x00, 0x07,
  0x00, 0xC0, 0x08,
  0x80, 0x3F, 0x08,
  0x60, 0x40, 0x08,
  0x10, 0x40, 0x18,
  0x10, 0x40, 0x28,
  0x08, 0x40, 0x28,
  0x0E, 0x40, 0x28,
  0x09, 0x40, 0x28,
  0x0E, 0x40, 0x28,
  0x08, 0x40, 0x28,
  0x10, 0x40, 0x28,
  0x10, 0x40, 0x18,
  0x60, 0x40, 0x08,
  0x80, 0x3F, 0x08,
  0x00, 0xC0, 0x08,
  0x00, 0x00, 0x07,
  0x00, 0x00, 0x04,
  };

#include <Fonts/FreeSerifBold9pt7b.h>
#define mfont FreeSerifBold9pt7b
#include <Fonts/FreeSansOblique12pt7b.h>
#define bfont FreeSansOblique12pt7b

#include <Fonts/Lato_Medium_Italic_26.h>
#define xbfont Lato_Medium_Italic_26

BZ_DATAGRAM recv_dgram; // for receiving data, zero it after use
BZ_DATAGRAM send_dgram; // for sending data, zero it after use

struct INSTANT_TEXT
{
  BZ_EX_DATA extra;     // 4 bytes extra data
  byte text[BZ_DATALENGTH]; // size 12 + 1 extra for null terminated maybe
};

// ESP32 self management 
struct DATAMEM_TEXTS{
  //BYTE_LAYOUT layout; // layout setting
  byte team[BZ_DATALENGTH]; 
  byte name1[BZ_DATALENGTH];
  byte name2[BZ_DATALENGTH];
  byte auxtext[BZ_DATALENGTH];
};

//create UDP instance
WiFiUDP udp;

// DEBUG PURPOSE
// REMOVE THIS WHEN RELEASE
//#define CEEZ_WIFI

#ifdef CEEZ_WIFI

#define WIFI_PASS "1234567890"
#define AP_SSID  "MyWifi"
// Setup Static IP
IPAddress _local_IP(192, 168, 0, PANEL_BASE_ADDR + PANEL_ID);
IPAddress _gateway(192, 168, 0, 1);
IPAddress _subnet(255, 255, 255, 0);
IPAddress _primaryDNS(8, 8, 8, 8);   //optional
IPAddress _secondaryDNS(8, 8, 4, 4); //optional

#else //CEEZ_WIFI

//#define STA_SSID "SWBOX"
#define WIFI_PASS "12344321"
#define AP_SSID  "BUZZER"
// Setup Static IP
IPAddress _local_IP(10, 10, 10, PANEL_BASE_ADDR + PANEL_ID);
IPAddress _gateway(10, 10, 10, 1);
IPAddress _subnet(255, 255, 255, 0);
IPAddress _primaryDNS(8, 8, 8, 8);   //optional
IPAddress _secondaryDNS(8, 8, 4, 4); //optional

#endif //CEEZ_WIFI

// data memmory
// global variable for use
DATAMEM_TEXTS data_text;
INSTANT_TEXT insText;

// for LCD control
bool isHeaderOn;
bool isCenterOn;
bool isFooterOn;
bool isMessageOn;
IPAddress _hostIP; //use arduino IPAddress class for eaiser with UDP 

bool StartUDP();

void setIP()
{
  _TRACELN("Set up static IP");
  WiFi.config(_local_IP, _gateway, _subnet, _primaryDNS, _secondaryDNS);
}

#define WIFI_TRIES 10 // 10x500ms = 5s
bool Connect_Wifi_BZ()
{
  bool wConnected =0;
  int i=0;
    
  WiFi.begin(AP_SSID, WIFI_PASS);
  
  //try to connect wifi upto 20s then hang up with raindrops
  while (i <= WIFI_TRIES) {
    if (WiFi.status() == WL_CONNECTED) {wConnected = 1; break;}
    delay(500);
    i++;
  }

  // Stop wifi and end wifi object, to rain drops!
  if (!wConnected) { 
    WiFi.mode(WIFI_OFF);
    _TRACELN("WIFI fail!");
  }
  else
  {
    _TRACE("Connected using IP: ");
    _TRACELN(WiFi.localIP());
    _TRACELN("Starting UDP...");
    StartUDP();
  }
  return wConnected;
}

bool StartUDP()
{
    udp.begin(LOCAL_PORT);  //start udp socket
    return true;
}

#define showHeader(/*in char* */ text) _showHeader((byte*) text)
#define showCenter(/*in char* */ text) _showCenter((byte*) text)
#define showFooter(/*in char* */ text) _showFooter((byte*) text)
#define clearScreen() _clearScreen()

void _clearScreen()
{
  display.clearDisplay();
}

// small font only
void _showHeader(byte datax[BZ_DATALENGTH])
{
    display.setFont(0);
    int16_t  x1, y1;
    uint16_t w, h;
    display.getTextBounds((char*)datax, 0, 0, &x1, &y1, &w, &h);
    display.setTextColor(display.color565(0, 255, 255));
    display.setCursor((64-w)/2, 1); 
    display.print((char*)datax);
//    Serial.print(w);
}

// small font only
void _showCenter(byte datax[BZ_DATALENGTH])
{
    display.setFont(0);
    int16_t  x1, y1;
    uint16_t w, h;
    display.getTextBounds((char*)datax, 0, 0, &x1, &y1, &w, &h);
    display.setTextColor(display.color565(0, 255, 255));
    display.setCursor((64-w)/2, (32-h)/2); 
    display.print((char*)datax);
}

// small font only
void _showFooter(byte datax[BZ_DATALENGTH])
{
    display.setFont(0);
    int16_t  x1, y1;
    uint16_t w, h;
    display.getTextBounds((char*)datax, 0, 0, &x1, &y1, &w, &h);
    display.setTextColor(display.color565(0, 255, 255));
    display.setCursor((64-w)/2, 16+8); 
    display.print((char*)datax);
}

// small font or big font
void _showMessage(INSTANT_TEXT itxt)
{
  int16_t  x=0,y=0, x1, y1;
  uint16_t w, h;
  int16_t offset_y = 0, offset_x = 0;
  bool clear = 0;
  //if (insTxt.extra.cxtxt.size ) display.setFont(&mfont);
  if (itxt.extra.cxtxt.size == 0) {display.setFont(0); offset_y = -5; offset_x = 1;} //h=8
  if (itxt.extra.cxtxt.size == 1) {display.setFont(&mfont); offset_y = +5; offset_x = -2;} //h=12
  if (itxt.extra.cxtxt.size == 2) {display.setFont(&bfont); offset_y = +7; offset_x = -3;} //h=16
  if (itxt.extra.cxtxt.size == 3) {display.setFont(&xbfont); offset_y = +10; offset_x = -2;} //h = 25
  display.getTextBounds((char*)itxt.text, x, y, &x1, &y1, &w, &h);
  x = (63-w)/2; // center x
  y = 16 + h/2;// center y, default TT_CENTER
  // if (itxt.extra.cxtxt.type == TT_CENTER) y = 16; // default
  clear = 1;
  if (itxt.extra.cxtxt.type == TT_FULLSCREEN) _clearScreen(); // clear all
  if (itxt.extra.cxtxt.type == TT_FOOTER) y = 31 - h/2; // should be font size 0
  if (itxt.extra.cxtxt.type == TT_FOOTER1) 
  {
    y = 31 - h/2; // should be font size 0
    clear = 0;
  }
  if (itxt.extra.cxtxt.type == TT_FOOTER2) 
  {
    y = 16 + h/2 -3 ;  // should be font size 0
    clear = 0;
  }
  
  if (clear) display.fillRect(0, 9, 63, 31, CR16_BLACK); // clear center, footer
  display.setTextColor(itxt.extra.cxtxt.cref);
  
  display.setCursor(x + offset_x, y + offset_y); 
  display.print((char*)itxt.text);

  _TRACELN((char*)itxt.text);
}

void _clearHeader()
{
  display.fillRect(0,0,63,8, CR16_BLACK);
}

void _clearCenter()
{
  display.fillRect(0,9,63,31, CR16_BLACK);
}


void _showBell(int16_t x, int16_t y, uint16_t colorx)
{
  uint16_t k =0, y1 = y, x1=x; // to 19
  for (k =0; k<57; k++)
  {
    y1=y + 8*(k%3);
    x1=x+k/3;
    for (int16_t j=0; j<8; j++)
    {
      if (bell[k] & (1<<j)) display.drawPixel(x1, y1+j, colorx);
    }
  }
  
}

// show aura-like, dir: 0/1 = left/right, ex: 1-2 // extend 1-2
void _showBellex(int16_t x, int16_t y, uint16_t colorx, bool dir, uint8_t ex)
{

  if (dir == 0)
  {
    display.drawLine(x+1, 9+5, x+2, 9+5+3, colorx );
    display.drawLine(x+1, 9+5+15, x+2, 9+5-3, colorx );
    if (ex < 2) return;
    display.drawLine(x+1, 9+5, x+8, 9+5+3, colorx );
    display.drawLine(x+1, 9+5+15, x+8, 9+5-3, colorx );
  }

  if (dir == 1)
  {
    display.drawLine(x+30, 9+5, x+30-2, 9+5+3, colorx );
    display.drawLine(x+30, 9+5+15, x+30-2, 9+5-3, colorx );
    if (ex < 2) return;
    display.drawLine(x+30, 9+5, x+30-8, 9+5+3, colorx );
    display.drawLine(x+30, 9+5+15, x+30-8, 9+5-3, colorx );
  }
}

// pos: 0 = center, 1 = left, 2  = right, 3+ = left+right
void showBell( uint8_t pos, uint16_t color, uint8_t ex)
{
  if (pos == 0) 
  {
    _showBell(23, 9, color);
    //_showBellex(23, 9, color, 0, ex);
    //_showBellex(23, 9, color, 1, ex);
  }
  if (pos == 1 || pos > 2) 
  {
    _showBell(7, 9, color);
    //_showBellex(7, 9, color, 0, ex);
    //_showBellex(7, 9, color, 1, ex);
  }
  if (pos == 2 || pos > 2) 
  {
    _showBell(38, 9, color);
    //_showBellex(38, 9, color, 0, ex);
    //_showBellex(38, 9, color, 1, ex);
  }
    
}

// dir = 0 = left + right, 1 = left, 2 = right
void _arrow(uint8_t dir, uint8_t step, uint16_t color)
{
  if (dir == 1 || dir == 0) 
  {
    display.drawLine(3 + step*5, 20, 9 + step*5, 14, color);
    display.drawLine(3 + step*5, 20, 9 + step*5, 26, color);
  }
  if (dir == 2 || dir == 0) 
  {
    display.drawLine(60 - step*5, 20, 54 - step*5, 14, color);
    display.drawLine(60 - step*5, 20, 54 - step*5, 26, color);
  }
}

void _showBuzz(uint8_t k = 0)
{
  showBell(3, CR16_BLANCHEDALMOND, k);
}

void _showBuzz1(uint8_t k = 0)
{
  showBell(1, CR16_BLANCHEDALMOND, k);
}

void _showBuzz2(uint8_t k = 0)
{
  showBell(2, CR16_BLANCHEDALMOND, k);
}

// player = 0, 1, 2 // 0 for whole team
void _showMC(uint8_t mcx, uint8_t player)
{
  if (mcx == 0|| mcx > 4) return;
  if (player > 2) return;

  display.setTextColor(CR16_GREEN);
  display.setFont(&bfont);
  
  if (player ==0) {display.setCursor(21, 29); display.fillRect(0,9, 63,31, CR16_BLACK);}
  if (player ==1) {display.setCursor(5, 29); display.fillRect(0,9, 31,31, CR16_BLACK);}
  if (player ==2) {display.setCursor(39, 29); display.fillRect(32,9, 63,31, CR16_BLACK);}
    
  if (mcx == 1) display.print('A');
  if (mcx == 2) display.print('B');
  if (mcx == 3) display.print('C');
  if (mcx == 4) display.print('D');
}




void _send_UDP(BZ_DATAGRAM txdg)
{
  if (!_hostIP) return; //no point to send data to the empty space
  udp.beginPacket(_hostIP, LOCAL_PORT);
  udp.write((uint8_t*)&txdg, sizeof(txdg));
  udp.endPacket();
  
}


// send a UDP package
void send_UDP_TRIGGER(uint8_t msgid)
{
  BZ_DATAGRAM sending_dg;
  memzero(sending_dg);

  sending_dg.mmsgid = (BYTE_MMSG_ID) msgid;
  sending_dg.extra.ul32 = PANEL_ID;

  _send_UDP(sending_dg);
}

// Read and parse incoming UDP messages
BYTE_MMSG_ID _read_UDP()
{
  BYTE_MMSG_ID ret = M_NULL;
  BZ_EX_DATA exData;
  zeromem(recv_dgram);
  zeromem(exData);
  
  udp.parsePacket();
  
  
  int k = udp.read((char*)&recv_dgram, sizeof(BZ_DATAGRAM));

  // got full  datagram, a valid package
  if (k == sizeof(BZ_DATAGRAM)) {
    // copy extra data
    exData = recv_dgram.extra;
    ret = recv_dgram.mmsgid;
    // decide what message it is
    switch (recv_dgram.mmsgid)
    {
    // Display instant message, clear the panel, 
    case M_SHOW_INSTANT_TEXT:
      bstrcpy(insText.text, recv_dgram.data);
      insText.extra = exData;
      _showMessage(insText);
      ret = M_SHOW_INSTANT_TEXT;
      break;
    
    // Toggle Team name, player name, or aux text
    // depend on what type of layout is currently using.
    case M_SHOW_TEXT:
      if (exData.cxtxt.type == TT_TEAM) 
      {
        _clearHeader();
        _showHeader(data_text.team);
      }
        
      
      if (exData.cxtxt.type == TT_PLAYER1) 
      {
        _clearHeader();
        _showCenter(data_text.name1);
      }

      if (exData.cxtxt.type == TT_PLAYER2) 
      {
        _clearHeader();
        _showCenter(data_text.name2);
      }

      if (exData.cxtxt.type == TT_AUXTEXT1) 
      {
        _clearCenter();
        _showCenter(data_text.auxtext);
      }

      _TRACELN("SHOW TEXT");
      ret = M_SHOW_TEXT;
      break;

    // Set define text
    // default value for Team should be Team1, Player1...
    case M_SET_TEXT:
      exData = recv_dgram.extra;
      if (exData.cxtxt.type == TT_TEAM) 
      {
        bstrcpy(data_text.team, recv_dgram.data);
        //EEPROM.put(EEAD_TEAM, data_text.team);
      }
      
      if (exData.cxtxt.type == TT_PLAYER1) 
      {
        bstrcpy(data_text.name1, recv_dgram.data);
        //EEPROM.put(EEAD_TEAM, data_text.team);
      }
      
      if (exData.cxtxt.type == TT_PLAYER2) 
      {
        bstrcpy(data_text.name2, recv_dgram.data);
        //EEPROM.put(EEAD_TEAM, data_text.team);
      }

      if (exData.cxtxt.type == TT_AUXTEXT1) 
      {
        bstrcpy(data_text.auxtext, recv_dgram.data);
        //EEPROM.put(EEAD_TEAM, data_text.team);
      }
      _TRACELN((char*)recv_dgram.data);
      return M_SET_TEXT;

    //case M_LAYOUT:
    //  return M_LAYOUT;

    // Disable team
    case M_DISABLE:
      return M_DISABLE;
    
    case M_ENABLE:
      return M_ENABLE;

    // clear screen
    case M_CLEARSCREEN:
      _clearScreen();
      return M_CLEARSCREEN;

    case M_CLEARHEADER:
      _clearHeader();
      return M_CLEARHEADER;

    case M_CLEARSCENTER:
      _clearCenter();
      return M_CLEARSCENTER;
          
    case M_RESET:
      ESP.restart(); //restart the panel
      return M_RESET;
    
    // display screensaver (rain drops)
    case M_SCREENSAVER_ON:
      return M_SCREENSAVER_ON;
      
    case M_SCREENSAVER_OFF:
      return M_SCREENSAVER_OFF;
      
    case M_HOST_IP: //assign host IP to report to, as we could use an extra PC for controlling text
      _hostIP = recv_dgram.extra.ul32;
      //HOST_OK = 1; // if ever host send it's IP
      return M_HOST_IP;

    default:
      return ret; // return unhandled message
      break;
    }
  }
  return ret;
}