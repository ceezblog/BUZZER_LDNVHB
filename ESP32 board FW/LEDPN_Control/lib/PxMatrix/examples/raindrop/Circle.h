// Adafruit GFX need to be initialized first

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define DROP_MAX 3

class Circle {
  public:
    Circle(int x_, int y_, int r_, uint16_t c_);
    int x;
    int y;
    int r;
    uint16_t c;
  private:
};

Circle::Circle(int x_, int y_, int r_, uint16_t c_)
{
  x = x_;
  y = y_;
  r = r_;
  c = c_;
}

class RainDrop {
	public:
	  void init();
    void dropAnother();
    void animateCircles();

  private:
    Circle *circle[DROP_MAX];
    int dropRateMin = 200;
    int dropRateMax = 800;
    unsigned long timeToDrop;
	  int dropCount = -1;
};

void RainDrop::dropAnother()
{
  if (millis()>timeToDrop) {
    if (++dropCount>=DROP_MAX) dropCount=0;
    Circle *aDrop = circle[dropCount];
    aDrop->x = random(0,SCREEN_WIDTH);
    aDrop->y = random(0,SCREEN_HEIGHT);
    aDrop->r = 1;
    aDrop->c = display.color565(random(8,32), random(8,64), random(8,32));
    timeToDrop = millis() + random(dropRateMin,dropRateMax);
  }
}

void RainDrop::animateCircles()
{
  display.fillScreen(0);
  for (int i=0; i<DROP_MAX; i++) {
    Circle aCircle = *(circle[i]);
    if (aCircle.r != 0) {
      display.drawCircle(aCircle.x, aCircle.y, aCircle.r, aCircle.c);
      circle[i]->r += 1;
    }
  }
  delay(10);
}

void RainDrop::init()
{
  for (int i=0; i<DROP_MAX; i++) {
    circle[i] = new Circle(0,0,0,0);
  }
}
