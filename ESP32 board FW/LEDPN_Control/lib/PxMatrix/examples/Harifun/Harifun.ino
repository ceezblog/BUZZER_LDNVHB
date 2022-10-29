#include <PxMatrix.h>
#define SCAN_RATE 16

#include <Fonts/FreeSansBoldOblique9pt7b.h>
#include <Fonts/FreeSansBoldOblique12pt7b.h>

#include <Ticker.h>
Ticker display_ticker;



#define P_LAT 4
#define P_A 15
#define P_B 2
#define P_C 12
#define P_D 0
#define P_OE 16

// Pins for LED MATRIX
PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);

// ISR for display refresh
void display_updater()
{
  //display.displayTestPattern(70);
  display.display(70);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  display.begin(SCAN_RATE);

  display.setFastUpdate(true);
  display.clearDisplay();
  display_ticker.attach(0.002, display_updater);

  delay(1000);
}

void DisplayHariFun()
{
  display.fillScreen(display.color565(0, 0,0));
  
  display.setTextSize(1);
  
  display.setFont(&FreeSansBoldOblique9pt7b);
  display.setTextColor(display.color565(0, 0, 255));
  display.setCursor(14,31-18); display.print("Hari");
  
  display.setFont(&FreeSansBoldOblique12pt7b);
  display.setTextColor(display.color565(255,255,0));
  display.setCursor(31-26,31); display.print("FUN");
}

void Chord(int r, float rot)
{
  int nodes = 6;
  float x[nodes];
  float y[nodes];
  for (int i=0; i<nodes; i++)
  {
    float a = rot + (PI*2*i/nodes);
    x[i] = 31+3 + cos(a)*r;
    y[i] = 16 + sin(a)*r;
  }

  display.fillScreen(display.color565(0, 0,0));
  for (int i=0; i<(nodes-1); i++)
    for (int j=i+1; j<nodes; j++)
      display.drawLine(x[i],y[i], x[j],y[j], display.color565(0, 255,0));
}

void loop() {
  DisplayHariFun();
  delay(1500);
 
  float rot;
  float rotationSpeed = PI/15;
  for (int r=1; r<44; r+=3) {
    Chord(r, rot+=rotationSpeed);
    delay(50);
  }

  for (int r=1; r<44; r+=3) {
    Chord(44-r, rot-=rotationSpeed);
    delay(30);
  }
}