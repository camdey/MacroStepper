#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
// #include <Adafruit_TFTLCD.h> // Hardware-specific library
// #include <ILI9341_t3.h>
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include <Fonts/Arimo_Regular_10.h>
#include <Fonts/Arimo_Regular_16.h>
#include <Fonts/Arimo_Regular_18.h>
#include <Fonts/Arimo_Regular_20.h>
#include <Fonts/Arimo_Regular_22.h>
#include <Fonts/Arimo_Regular_24.h>
#include <Fonts/Arimo_Bold_24.h>
#include <Fonts/Arimo_Regular_30.h>
#include <Fonts/Arimo_Bold_30.h>
#include <Fonts/Syncopate_Bold_36.h>
#include <Fonts/Permanent_Marker_Regular_36.h>

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
#define GRAY    0xE73C
#define iZettleGreenLite  0x9736
#define iZettleGreen      0x6ED3
#define iZettleRed        0xFBCC
#define iZettleRedLite    0xFCD1
#define iZettleBlue       0x4EDE
#define iZettleGrey       0xCE7A
#define iZettleGreyLite   0xDEFB

#define BOXSIZE 40
#define PENRADIUS 3
#define MINPRESSURE 10
#define MAXPRESSURE 1000

void startScreen();
void drawArrows();
void drawGrid();
void manualScreen();

void setup(void) {
  Serial.begin(9600);
  tft.reset();

  uint16_t identifier = tft.readID();
  if (identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else {
    Serial.print(F("No driver found"));
  }

  tft.setFont(&Arimo_Regular_24);
  tft.begin(identifier);
  tft.fillScreen(BLACK);
  tft.setRotation(1);

  startScreen();
  delay(1000);
  manualScreen();
}

void loop() {
  TSPoint point = ts.getPoint();

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
}

void startScreen() {
  // tft.fillScreen(GRAY);
  tft.setTextColor(BLACK);

  // grid patterns
  // drawGrid();

  tft.setCursor(30, 40);
  tft.setFont(&Permanent_Marker_Regular_36);
  tft.println("MacroStepper");

  // Manual option
  tft.fillRoundRect(20, 90, 130, 60, 8, iZettleRed);
  tft.setCursor(30,130);
  tft.setFont(&Arimo_Bold_30);
  tft.println("Manual");

  // Auto option
  tft.fillRoundRect(170, 90, 130, 60, 8, iZettleGreen);
  tft.setCursor(200,130);
  tft.setFont(&Arimo_Bold_30);
  tft.println("Auto");

  // Homing option
  tft.fillRect(0, 200, 320, 40, iZettleBlue);
  tft.setCursor(20,225);
  tft.setFont(&Arimo_Regular_20);
  tft.println("Re-home");

  // Move-to option
  // tft.fillRoundRect(0, 200, 320, 40, 8, iZettleBlue);
  tft.setCursor(220,225);
  tft.setFont(&Arimo_Regular_20);
  tft.println("Move-to");
}

void manualScreen() {
  tft.fillScreen(BLACK);
  tft.setTextColor(BLACK);

  // grid patterns
  // drawGrid();

  // Step Number
  tft.fillRoundRect(5, 5, 125, 65, 14, iZettleGrey);
  tft.setCursor(20,30);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Nr.");
  // tft.drawFastHLine(20,30,95,RED); // middle is 48 pixels
  tft.setCursor(68-18,60);
  tft.setFont(&Arimo_Bold_30);
  tft.println("38");
  // tft.drawFastHLine(50,65,35,RED); // middle is 18 pixels

  // Step Distance
  tft.fillRoundRect(5, 85, 125, 65, 14, iZettleGrey);
  tft.setCursor(68-58, 110);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Dist.");
  // tft.drawFastHLine(20,115,115,RED); // middle is 58 pixels
  tft.setCursor(68-50, 140);
  tft.setFont(&Arimo_Bold_30);
  tft.println("0.0375");
  // tft.drawFastHLine(20,145,100,RED); // middle is 50 pixels

  // Rail Posoition
  tft.fillRoundRect(5, 165, 125, 65, 14, iZettleGrey);
  tft.setCursor(68-50,190);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Rail Pos.");
  // tft.drawFastHLine(20,195,100,RED); // middle is 50 pixels
  tft.setCursor(68-50,220);
  tft.setFont(&Arimo_Bold_30);
  tft.println("78.545");
  // tft.drawFastHLine(20,225,100,RED); // middle is 50 pixels

  // auto shutter
  tft.setCursor(150, 40);
  tft.setFont(&Arimo_Regular_20);
  tft.setTextColor(WHITE);
  tft.println("Shutter");

  // reset steps
  tft.setCursor(150, 120);
  tft.setFont(&Arimo_Regular_20);
  tft.setTextColor(WHITE);
  tft.println("Reset");

  // back to main screen
  tft.setCursor(150, 200);
  tft.setFont(&Arimo_Regular_20);
  tft.setTextColor(WHITE);
  tft.println("Back");

  drawArrows();
}

void drawGrid() {
  for (int i = 1; i < 240/40; i++) {
    tft.drawFastHLine(0, 40*i, 320, WHITE);
  }
  for (int i = 1; i < 320/40; i++) {
    tft.drawFastVLine(40*i, 0, 240, WHITE);
  }
}

void drawArrows() {
  tft.fillTriangle(230, 65, 260, 15, 290, 65, iZettleGreen);
  tft.fillRoundRect(245, 69, 30, 36, 4, iZettleGreen);

  tft.fillTriangle(230, 175, 260, 225, 290, 175, iZettleRed);
  tft.fillRoundRect(245, 135, 30, 36, 4, iZettleRed);
}
