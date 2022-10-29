#include <PxMatrix.h>

#ifdef ESP32
//CEEZ CONFIG for CBUZZ3.3
#define P_A 15
#define P_B 4
#define P_C 17
#define P_D 16
#define P_E -1
#define P_OE 26
#define P_LAT 27

/* 
// HW SPI PINS - Just to remind 
#define SPI_BUS_CLK 14
#define SPI_BUS_MOSI 13
#define SPI_BUS_MISO 12
#define SPI_BUS_SS 4
*/

hw_timer_t * scanning_timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D);

void IRAM_ATTR display_updater() {
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  display.display(55);
  portEXIT_CRITICAL_ISR(&timerMux);
}

#define start_ledpn display_attach_updater
#define stop_ledpn display_detach_updater

void display_attach_updater()
{ 
  scanning_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(scanning_timer, &display_updater, true);
  timerAlarmWrite(scanning_timer, 2000, true);
  timerAlarmEnable(scanning_timer);
}
void display_detach_updater()
{
  timerDetachInterrupt(scanning_timer);
}

#endif
