#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>

MCUFRIEND_kbv tft;

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 100
#define TS_MAXX 920

#define TS_MINY 70
#define TS_MAXY 900

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 50);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define BOXSIZE 40
#define PENRADIUS 3
#define MINPRESSURE 10
#define MAXPRESSURE 1000
int oldcolor, currentcolor;

void draw(TSPoint& point);
void startScreen();
void drawArrows();

void setup(void) {
  Serial.begin(9600);
  tft.reset();

  uint16_t identifier = tft.readID();
  if (identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else {
    Serial.print(F("No driver found"));
  }

  tft.begin(identifier);
  tft.setRotation(0);
  tft.fillScreen(WHITE);

// led
//  pinMode(13, OUTPUT);

  tft.setRotation(1);
  startScreen();
}

void loop() {
  unsigned long timer = millis();
  TSPoint point = ts.getPoint();

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (point.z > MINPRESSURE && point.z < MAXPRESSURE) {
    draw(point);
  }
}

void startScreen() {
  tft.fillScreen(WHITE);
  tft.setTextColor(BLACK);

  // drawArrows
  drawArrows();

  // step distance
  tft.setCursor(25,25);
  tft.setTextSize(2);
  tft.println("Step Dist.");
  tft.drawFastHLine(25, 25+(10*2), (10*6*2), BLACK);
  tft.setTextSize(3);
  tft.setCursor(25, 25 + (10*2) + 5);
  tft.println("0.025mm");
  // tft.drawRect(25-8, 25-8, (7*6*3)+10, (10*5)+10, BLACK);

  // rail position
  tft.setCursor(25,100);
  tft.setTextSize(2);
  tft.println("Rail Pos.");
  tft.drawFastHLine(25, 100+(10*2), (9*6*2), BLACK);
  tft.setTextSize(3);
  tft.setCursor(25, 100 + (10*2) + 5);
  tft.println("35.625mm");
  // tft.drawRect(25-8, 100-8, (8*6*3)+10, (10*5)+10, BLACK);

  // step progress
  tft.setCursor(25,175);
  tft.setTextSize(2);
  tft.println("Progress");
  tft.drawFastHLine(25, 175+(10*2), (8*6*2), BLACK);
  tft.setTextSize(3);
  tft.setCursor(25, 175 + (10*2) + 5);
  tft.println("19/72");
  // tft.drawRect(25-8, 175-8, (5*6*3)+10, (10*5)+10, BLACK);
}

void drawArrows() {
  tft.fillTriangle(230, 50, 255, 15, 280, 50, BLACK);
  tft.fillRect(245, 50, 20, 30, BLACK);

  tft.drawRect(225, 90, 60, 40, BLACK);
  tft.setTextSize(2);
  tft.setCursor(231, 105);
  tft.println("AUTO");

  tft.fillRect(245, 140, 20, 30, BLACK);
  tft.fillTriangle(230, 170, 255, 205, 280, 170, BLACK);

}

void draw(TSPoint& point) {
  tft.fillScreen(BLACK);

  // scale from 0->1023 to tft.width
  point.x = map(point.x, TS_MINX, TS_MAXX, tft.width(), 0);
  point.y = (tft.height()-map(point.y, TS_MINY, TS_MAXY, tft.height(), 0));

   currentcolor = RED;
//   tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);

  if (((point.y-PENRADIUS) > BOXSIZE) && ((point.y+PENRADIUS) < tft.height())) {
    tft.fillCircle(point.x, point.y, PENRADIUS, currentcolor);
  }
}
