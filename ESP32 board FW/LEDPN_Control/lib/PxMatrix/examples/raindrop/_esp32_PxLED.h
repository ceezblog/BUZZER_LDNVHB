#include <PxMatrix.h>

#ifdef ESP32
/*
#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 2
//#define P_CLK 14
//#define P_R1 13 */

//CEEZ CONFIG
#define P_A 15
#define P_B 4
#define P_C 16
#define P_D 17
#define P_E -1
#define P_OE 26
#define P_LAT 27

/*
// HW SPI PINS
#define SPI_BUS_CLK 14
#define SPI_BUS_MOSI 13
#define SPI_BUS_MISO 12
#define SPI_BUS_SS 4
*/

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D);

void IRAM_ATTR display_updater() {
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  display.display(55);
  //display.displayTestPattern(70);
  portEXIT_CRITICAL_ISR(&timerMux);
}

void display_attach_updater()
{ 
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &display_updater, true);
  timerAlarmWrite(timer, 2000, true);
  timerAlarmEnable(timer);
}

#endif
