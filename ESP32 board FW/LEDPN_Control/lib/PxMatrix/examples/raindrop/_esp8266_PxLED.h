#include <PxMatrix.h>

#ifdef ESP8266
#include <Ticker.h>
Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D);

// ISR for display refresh
void display_updater()
{
  //display.displayTestPattern(70);
  display.display(70);
}

void display_attach_updater()
{ 
  display_ticker.attach(0.002, display_updater);
}


#endif
