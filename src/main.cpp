#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include <Fonts/Arimo_Regular_10.h>
#include <Fonts/Arimo_Regular_16.h>
#include <Fonts/Arimo_Regular_18.h>
#include <Fonts/Arimo_Regular_20.h>
#include <Fonts/Arimo_Bold_20.h>
#include <Fonts/Arimo_Regular_22.h>
#include <Fonts/Arimo_Regular_24.h>
#include <Fonts/Arimo_Bold_24.h>
#include <Fonts/Arimo_Regular_30.h>
#include <Fonts/Arimo_Bold_30.h>
#include <Fonts/Syncopate_Bold_36.h>
#include <Fonts/Permanent_Marker_Regular_36.h>
#include <Fonts/Lato_Black_34.h>

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 100
#define TS_MAXX 920
#define TS_MINY 70
#define TS_MAXY 920

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
MCUFRIEND_kbv tft;

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
// #define iZettleGreen      0x6ED3
#define iZettleGreen      0x4ECC
#define iZettleRed        0xFBCC
#define iZettleRedLite    0xFCD1
#define iZettleBlue       0x4EDE
#define iZettleGrey       0xCE7A
#define iZettleGreyLite   0xDEFB

#define BOXSIZE 40
#define PENRADIUS 3
#define MINPRESSURE 5
#define MAXPRESSURE 2000

void drawArrows();
void drawGrid();
void touchScreen();
void startScreen();
void startScreenTouch(TSPoint&);
void manualScreen();
void manualScreenTouch(TSPoint&);
void autoScreen();
void autoScreenTouch(TSPoint&);
void autoConfigScreen();
void autoConfigScreenTouch(TSPoint&);
void homeRail();
void toggleJoystick();
void toggleShutter();
void setShutterDelay();
void dryRun();
int arrowsTouch(TSPoint&, int val);

unsigned long Timer = 0;
int activeScreen = 1;
bool joystickState = 1;
bool shutterState = 1;
bool arrowsActive = 0;
int shutterDelay = 3;
int prevDelay = 3;
bool editDelay = 0;
bool editStart = 0;
bool editEnd = 0;

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
}

void loop() {
  Timer = millis();

  // take touch reading
  if (Timer % 50 == 0) {
    touchScreen();
  }
}

void touchScreen() {
  TSPoint point = ts.getPoint();

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (point.z > MINPRESSURE && point.z < MAXPRESSURE) {
    if (activeScreen == 1) {
      startScreenTouch(point);
    } else if (activeScreen == 2) {
      manualScreenTouch(point);
    } else if (activeScreen == 3) {
      autoScreenTouch(point);
    } else if (activeScreen == 4) {
      autoConfigScreenTouch(point);
    }
  }
}

void startScreen() {
  activeScreen = 1;
  tft.fillScreen(BLACK);

  tft.setTextColor(WHITE);
  tft.setCursor(30, 45);
  tft.setFont(&Permanent_Marker_Regular_36);
  tft.println("MacroStepper");

  tft.setTextColor(BLACK);
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
  // tft.fillRect(0, 200, 320, 40, iZettleBlue);
  tft.setCursor(20,225);
  tft.setFont(&Arimo_Bold_20);
  tft.setTextColor(iZettleGreen, BLACK);
  tft.println("Re-home");

  // Move-to option
  tft.setCursor(220,225);
  tft.setFont(&Arimo_Bold_20);
  if (joystickState == 1) {
    tft.setTextColor(iZettleGreen, BLACK);
  } else if (joystickState == 0) {
    tft.setTextColor(iZettleRed, BLACK);
  }
  tft.println("Joystick");
}

void startScreenTouch(TSPoint &point) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());

  // manual box - tft.fillRoundRect(20, 90, 130, 60, 8, iZettleRed);
  if ((xPos >= 20 && xPos <= 150) && (yPos >= 90 && yPos <= 150)) {
    manualScreen();
  }
  // auto box - tft.fillRoundRect(170, 90, 130, 60, 8, iZettleGreen);
  if ((xPos >= 170 && xPos <= 300) && (yPos >= 90 && yPos <= 150)) {
    autoScreen();
  }

  // homing func - tft.setCursor(20,225);
  if ((xPos >= 20 && xPos <= 120) && (yPos >= 200 && yPos <= 240)) {
    tft.setFont(&Arimo_Bold_20);
    tft.setCursor(20,225);
    tft.setTextColor(iZettleRed, BLACK);
    tft.println("Re-home");
    homeRail();
    tft.setCursor(20,225);
    tft.setTextColor(iZettleGreen, BLACK);
    tft.println("Re-home");
  }

  // joystick on/off - tft.setCursor(20,225);
  if ((xPos >= 220 && xPos <= 320) && (yPos >= 200 && yPos <= 240)) {
    toggleJoystick();
    tft.setCursor(220,225);
    tft.setFont(&Arimo_Bold_20);
    if (joystickState == 1) {
      tft.setTextColor(iZettleGreen, BLACK);
    } else if (joystickState == 0) {
      tft.setTextColor(iZettleRed, BLACK);
    }
    tft.println("Joystick");
    delay(300);
  }
}

void manualScreen() {
  activeScreen = 2;
  tft.fillScreen(BLACK);
  tft.setTextColor(BLACK);

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
  tft.setCursor(150, 45);
  tft.setFont(&Arimo_Bold_20);
  tft.setTextColor(iZettleRed);
  if (shutterState == 1) {
    tft.setTextColor(iZettleGreen, BLACK);
  } else if (shutterState == 0) {
    tft.setTextColor(iZettleRed, BLACK);
  }
  tft.println("Shutter");

  // reset steps
  tft.setCursor(150, 125);
  tft.setFont(&Arimo_Bold_20);
  tft.setTextColor(WHITE);
  tft.println("Reset");

  // back to main screen
  tft.setCursor(150, 205);
  tft.setFont(&Arimo_Bold_20);
  tft.setTextColor(WHITE);
  tft.println("Back");

  drawArrows();
}

void manualScreenTouch(TSPoint &point) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());

  // shutter toggle - tft.setCursor(150, 45);
  if ((xPos >= 150 && xPos <= 230) && (yPos >= 32 && yPos <= 48)) {
    toggleShutter();
    tft.setCursor(150, 45);
    tft.setFont(&Arimo_Bold_20);
    if (shutterState == 1) {
      tft.setTextColor(iZettleGreen, BLACK);
    } else if (shutterState == 0) {
      tft.setTextColor(iZettleRed, BLACK);
    }
    tft.println("Shutter");
    delay(100);
  }

  // back button - tft.setCursor(150, 205);
  if ((xPos >= 150 && xPos <= 200) && (yPos >= 192 && yPos <= 208)) {
    startScreen();
  }
}

void autoScreen() {
  activeScreen = 3;
  tft.fillScreen(BLACK);
  tft.setTextColor(BLACK);

  // Step progress
  tft.fillRoundRect(5, 5, 125, 65, 14, iZettleGrey);
  tft.setCursor(16, 30);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Progress");
  // tft.drawFastHLine(20,30,100,RED); // middle is 50 pixels
  tft.setCursor(66-42, 60);
  tft.setFont(&Arimo_Bold_30);
  tft.println("38/73");
  // tft.drawFastHLine(50, 65, 85, RED); // middle is 42 pixels

  // Estimated time remaining
  tft.fillRoundRect(5, 85, 125, 65, 14, iZettleGrey);
  tft.setCursor(66-54, 110);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Est. Time");
  // tft.drawFastHLine(20, 115, 108, RED); // middle is 54 pixels
  tft.setCursor(66-30, 140);
  tft.setFont(&Arimo_Bold_30);
  tft.println("4:53");
  // tft.drawFastHLine(20, 145, 60, RED); // middle is 30 pixels

  // Step distance
  tft.fillRoundRect(5, 165, 125, 65, 14, iZettleGrey);
  tft.setCursor(66-56,190);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Dist.");
  // tft.drawFastHLine(20, 195, 112, RED); // middle is 56 pixels
  tft.setCursor(66-48, 220);
  tft.setFont(&Arimo_Bold_30);
  tft.println("0.0025");
  // tft.drawFastHLine(20, 225, 95, RED); // middle is 48 pixels

  // auto shutter
  tft.setCursor(150, 45);
  tft.setFont(&Arimo_Bold_20);
  if (shutterState == 1) {
    tft.setTextColor(iZettleGreen, BLACK);
  } else if (shutterState == 0) {
    tft.setTextColor(iZettleRed, BLACK);
  }
  tft.println("Shutter");

  // enter auto config
  tft.setCursor(150, 125);
  tft.setFont(&Arimo_Bold_20);
  tft.setTextColor(WHITE);
  tft.println("Config");

  // back to main screen
  tft.setCursor(150, 205);
  tft.setFont(&Arimo_Bold_20);
  tft.setTextColor(WHITE);
  tft.println("Back");

  drawArrows();
}

void autoScreenTouch(TSPoint &point) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());

  // shutter toggle - tft.setCursor(150, 45);
  if ((xPos >= 150 && xPos <= 230) && (yPos >= 32 && yPos <= 48)) {
    toggleShutter();
    tft.setCursor(150, 45);
    tft.setFont(&Arimo_Bold_20);
    if (shutterState == 1) {
      tft.setTextColor(iZettleGreen, BLACK);
    } else if (shutterState == 0) {
      tft.setTextColor(iZettleRed, BLACK);
    }
    tft.println("Shutter");
    delay(100);
  }
  // config button - tft.setCursor(150, 125);
  if ((xPos >= 150 && xPos <= 220) && (yPos >= 112 && yPos <= 128)) {
    autoConfigScreen();
  }
  // back button - tft.setCursor(150, 205);
  if ((xPos >= 150 && xPos <= 200) && (yPos >= 192 && yPos <= 208)) {
    startScreen();
  }
}

void autoConfigScreen() {
  activeScreen = 4;
  tft.fillScreen(BLACK);
  tft.setTextColor(BLACK);

  // set start point
  tft.setCursor(20, 50);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(iZettleGreen);
  tft.println("START");
  tft.setCursor(35, 65);
  tft.setFont(&Arimo_Regular_16);
  tft.setTextColor(WHITE);
  tft.println("3252");

  // Set end point
  tft.setCursor(20, 130);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(iZettleRed);
  tft.println("END");
  tft.setCursor(35, 145);
  tft.setFont(&Arimo_Regular_16);
  tft.setTextColor(WHITE);
  tft.println("3310");

  // Run from start to end and return
  // move back from end point to start point (with accel)
  // then run from start to end with no accel
  tft.setCursor(20, 210);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(iZettleBlue);
  tft.println("RUN");

  // set delay
  tft.setCursor(150, 125);
  tft.setFont(&Arimo_Bold_24);
  tft.setTextColor(WHITE);
  tft.println("Delay");
  tft.setCursor(155, 143);
  tft.setFont(&Arimo_Regular_20);
  tft.setTextColor(WHITE, BLACK);
  tft.println(shutterDelay);
  tft.setCursor(165, 143);
  tft.println(" sec");

  // back to auto screen
  tft.setCursor(150, 205);
  tft.setFont(&Arimo_Bold_20);
  tft.setTextColor(WHITE);
  tft.println("Back");

  drawArrows();
}

void autoConfigScreenTouch(TSPoint &point) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());

  if(Timer % 75 == 0) {
    // start button - tft.setCursor(20, 50);
    if ((xPos >= 10 && xPos <= 140) && (yPos >= 10 && yPos <= 70) && editDelay == 0) {
      arrowsActive = !arrowsActive;
      editStart = !editStart;

      if (arrowsActive == 0 && editStart == 0) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, BLACK);
        tft.fillRoundRect(10, 24, 5, 40, 4, BLACK);
      }
      if (arrowsActive == 1 && editStart == 1) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, YELLOW);
        tft.fillRoundRect(10, 24, 5, 40, 4, YELLOW);
      }
    }

    // end button - tft.setCursor(20, 130);
    if ((xPos >= 10 && xPos <= 120) && (yPos >= 90 && yPos <= 160) && editDelay == 0) {
      arrowsActive = !arrowsActive;
      editEnd = !editEnd;

      if (arrowsActive == 0 && editEnd == 0) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, BLACK);
        tft.fillRoundRect(10, 104, 5, 40, 4, BLACK);
      }
      if (arrowsActive == 1 && editEnd == 1) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, YELLOW);
        tft.fillRoundRect(10, 104, 5, 40, 4, YELLOW);
      }
    }

    // run button - tft.setCursor(20, 210);
    if ((xPos >= 10 && xPos <= 120) && (yPos >= 170 && yPos <= 240) && editDelay == 0) {
      tft.fillRoundRect(10, 184, 5, 27, 4, YELLOW);
      dryRun();
      tft.fillRoundRect(10, 184, 5, 27, 4, BLACK);
    }

    // delay button - tft.setCursor(150, 125);
    if ((xPos >= 150 && xPos <= 220) && (yPos >= 110 && yPos <= 145) && editStart == 0) {
      arrowsActive = !arrowsActive;
      editDelay = !editDelay;

      if (arrowsActive == 0 && editDelay == 0) {
        // tft.drawRoundRect(145, 100, 78, 50, 4, BLACK);
        tft.fillRoundRect(140, 108, 5, 35, 4, BLACK);
      }
      if (arrowsActive == 1 && editDelay == 1) {
        // tft.drawRoundRect(145, 100, 78, 50, 4, YELLOW);
        tft.fillRoundRect(140, 108, 5, 35, 4, YELLOW);
      }
    }

    // set shutter delay
    if (arrowsActive == 1 && editDelay == 1) {
      if (prevDelay != shutterDelay) {
        prevDelay = shutterDelay;
      }
      shutterDelay = arrowsTouch(point, shutterDelay);
      Serial.println(shutterDelay);
      setShutterDelay();
    }

    // back button - tft.setCursor(150, 205);
    if ((xPos >= 150 && xPos <= 200) && (yPos >= 192 && yPos <= 208) && arrowsActive == 0) {
      autoScreen();
    }

    // Serial.print("arrows: ");
    // Serial.println(arrowsActive);
    // Serial.print("start: ");
    // Serial.println(editStart);
    // Serial.print("shutter: ");
    // Serial.println(editDelay);
    // Serial.println("----------");
  }
}

int arrowsTouch(TSPoint &point, int val) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());

  if (Timer % 75 == 0) {
    // top arrow - tft.fillTriangle(230, 65, 260, 15, 290, 65, iZettleGreen);
    // tft.fillRoundRect(245, 69, 30, 36, 4, iZettleGreen);
    if ((xPos >= 230 && xPos <= 290) && (yPos >= 15 && yPos <= 105)) {
      val++;
    }
    // top arrow - tft.fillTriangle(230, 175, 260, 225, 290, 175, iZettleRed);
    // tft.fillRoundRect(245, 135, 30, 36, 4, iZettleRed);
    if ((xPos >= 230 && xPos <= 290) && (yPos >= 135 && yPos <= 225)) {
      val--;
    }
  }

  return val;
}

void homeRail() {
  delay(1000);
}

void toggleJoystick() {
  joystickState = !joystickState;
}

void toggleShutter() {
  shutterState = !shutterState;
}

void setShutterDelay() {

  if (shutterDelay < 0) {
    shutterDelay = 0;
  } else if (shutterDelay > 9) {
    shutterDelay = 9;
  }

  if (prevDelay != shutterDelay) {
    // print new delay value
    tft.setCursor(155, 143);
    tft.setFont(&Arimo_Regular_20);
    tft.setTextColor(BLACK, BLACK);
    tft.println(prevDelay);
    tft.setCursor(155, 143);
    tft.setTextColor(WHITE, BLACK);
    tft.println(shutterDelay);
  }
}

void dryRun() {
  delay(500);
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
