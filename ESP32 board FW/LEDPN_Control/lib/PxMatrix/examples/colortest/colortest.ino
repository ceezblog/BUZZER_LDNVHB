////////////////////////////////////////////////////////////////////////////
// 32x64 P5 panel - COLOR TEST
// Cee'z 2019 
// Using PxMatrix by Dominic Buchstaller @ https://github.com/2dom/PxMatrix
// WARNING: COULD GO UP TO 3A IF USE FULL BRIGHTNESS

#include <PxMatrix.h>
#include "COLORREF.h"
#ifdef ESP32

#define P_A 15
#define P_B 4
#define P_C 16
#define P_D 17
#define P_E -1
#define P_OE 26
#define P_LAT 27

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

#endif

#ifdef ESP8266

#include "COLORREF.h"
#include <Ticker.h>
Ticker display_ticker;

/* //original
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2*/

//cee'z mod
#define P_LAT   4   // D2
#define P_A     15  // D8
#define P_B     2   // D4
#define P_C     12  // D6
#define P_D     0   // D3
#define P_OE    16  // D0

#endif

//#define P_CLK 14
//#define P_R1 13
PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D);


#ifdef ESP8266
// ISR for display refresh
void display_updater()
{
  //display.displayTestPattern(70);
  display.display(50);
}
#endif

#ifdef ESP32
void IRAM_ATTR display_updater() {
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  display.display(50);
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  display.begin(16);

#ifdef ESP8266
  display_ticker.attach(0.002, display_updater);
#endif

#ifdef ESP32
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &display_updater, true);
  timerAlarmWrite(timer, 2000, true);
  timerAlarmEnable(timer);
#endif
}

void loop() {
  display.fillScreen(0);
  delay(200);
  display.fillScreen(CR_RED);
  delay(200);
  display.fillScreen(CR_GREEN);
  delay(200);
  display.fillScreen(CR_BLUE);
  delay(200);
  display.fillScreen(CR_WHITE);
  delay(200);
  display.fillScreen(CR_ORANGE);
  delay(200);
  display.fillScreen(CR_DARKGREEN);
  delay(200);
  display.fillScreen(CR_LIGHTGREY);
  delay(200);
  display.fillScreen(CR_MAGENTA);
  delay(200);
  display.fillScreen(CR_PINK);
  delay(200);
}
