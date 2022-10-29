////////////////////////////////////////////////////////////////////////////
// Led matrix panel Test - 32x64 P5 1/16s
// Cee'z 2019 - Circle Drops from Hari Wiguna @ https://hackaday.io/hari
// Using PxMatrix by Dominic Buchstaller @ https://github.com/2dom/PxMatrix
// FOR ESP32-DEV MODULE ONLY <----- REMEMBER TO SWITCH BOARD

#include "_esp32_PxLED.h"
#include "Circle.h"

RainDrop drops;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  display.begin(16);
  display_attach_updater();

  drops.init();
  while(1) animateDrops();
}

void animateDrops()
{
  drops.dropAnother();
  drops.animateCircles();
}

void loop() {
  
}
