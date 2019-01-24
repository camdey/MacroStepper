// STEP = one full step of the stepper stepper motor
// MOVEMENT = one full movement of linear rail by specified distance, consisting of multiple steps
// PROECDURE = one completed stack procedure, consisting of multiple movements

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include <AccelStepper.h>
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

// touch screen orientation definitions
#define TS_MINX 100
#define TS_MAXX 920
#define TS_MINY 70
#define TS_MAXY 920

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
MCUFRIEND_kbv tft;

// tft definitions
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

// Definitions for some common 16-bit color values:
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
#define iZettleGreen      0x4ECC
#define iZettleRed        0xFBCC
#define iZettleRedLite    0xFCD1
#define iZettleBlue       0x4EDE
#define iZettleGrey       0xCE7A
#define iZettleGreyLite   0xDEFB

// touch definitions
#define minPressure 5
#define maxPressure 2000

// The X Stepper pins
#define stepperDirPin 27
#define stepperStepPin 29
#define stepperSleepPin 30
#define stepperEnablePin 41
AccelStepper stepper(AccelStepper::DRIVER, stepperStepPin, stepperDirPin);

#define rearLimitPin 51
#define forwardLimitPin 49
#define xStickPin A12 // analog pin connected to X output
#define yStickPin A13
#define zStickPin A14 //68
#define shutterPin 45

// --- Printing screen functions --- //
void startScreen();
void manualScreen();
void autoScreen();
void autoConfigScreen();
void drawPlayPause(bool greyPlay, bool greyPause);
void drawArrows();
void displayPosition();
void estimateDuration();
// void drawGrid();
// --- Touch screen functions --- //
void touchScreen();
void startScreenTouch(TSPoint&);
void manualScreenTouch(TSPoint&);
void autoScreenTouch(TSPoint&);
void autoConfigScreenTouch(TSPoint&);
int arrowsTouch(TSPoint&, bool stepMotor, int val);
// --- Stepper motion functions --- //
void autoStack();
void homeRail();
void dryRun();
void stepperStep(int direction, int delay);
// --- Enable/Disable functions --- //
void toggleJoystick();
void toggleShutter();
// --- Inputs and Outputs --- //
void joyStick();
void triggerShutter(bool lowHigh);
void limitSwitch();
// --- Set value functions --- //
void setStepDistance();
void setShutterDelay();
void setAutoStackPositions(bool setStart, bool setEnd);


// --- currentTimes and elapsed times --- //
unsigned long currentTime = 0;        // current time in millis()
unsigned long subRoutine1Time = 0;    // time subroutine1 last ran
unsigned long subRoutine2Time = 0;    // time subroutine2 last ran
unsigned long prevStepTime = 0;       // previous step time reading
unsigned long prevShutterTime = 0;    // previous time shutter triggered
unsigned long prevTouchTime = 0;      // previous time touch registered
unsigned long prevGenericTime = 0;    // generic timer for toggles and such
int genericTouchDelay = 200;          // 200ms touch delay for toggles
int prevMinutes = 1;                  // duration of autoStack
int prevSeconds = 1;                  // duration of autoStack

// --- Screen flags --- //
int activeScreen = 1;           // currently displayed screen
bool arrowsActive = 0;          // take arrow touch readings
bool editShutterDelay = 0;      // set shutter delay time
bool editStartPosition = 0;     // set start point for auto mode
bool editEndPosition = 0;       // set end point for auto mode
bool editMovementDistance = 0;  // set step distance in any mode
// --- Input and Output values --- //
int xStickPos = 0;              // ADC value of x-axis joystick
int zStickVal = 1;              // increment count of z-axis button press
int prevZStickVal = 1;          // only increment per button press
int shutterDelay = 3;           // delay between step and shutter trigger
int prevDelay = 3;              // previous delay value
// --- Enable/Disable functionality --- //
bool bootFlag = 0;              // runs rehoming sequence
bool goToStart = 1;             // move to start for autoStack procedure
bool joystickState = 1;         // enabled/disabled
bool autoStackFlag = 0;         // enables function for stack procedure
bool pauseAutoStack = 0;        // pause stack procedure
bool shutterState = 0;          // disabled/enabled
bool stepDue = 0;               // step is due when movement complete
bool targetFlag = 0;            // resets stepper target
// --- Position values --- //
long rearLimitPos = 0;          // limit switch nearest stepper
long forwardLimitPos = 10000;   // limit switch furtherest from stepper
int startPosition = 0;          // start position for stack procedure
int prevStartPosition = 0;
int endPosition = 0;            // end position for stack procedure
int prevEndPosition = 0;
int prevStepperPosition = 1;    // used for showing position of stepper if changed
int manualMovementCount = 0;    // count of manual movements
int prevManualMovementCount = 0;
// --- Stepper motor variables --- //
int stepsPerMovement = 1;       // number of steps required for a given distance
int numMovements = 0;           // equals total distance / step multiplier
int prevNumMovements = 0;       // previous numMovements value
int stepCount = 0;              // number of steps taken in a given movement
double distancePerMovement = 2.50; // distance in micrometres travelled per movement
double prevDistance = 2.50;     // previous step distance in micrometres
int movementProgress = 0;       // number of completed movements
int prevMovementProgress = 0;   // used for overwriting prev movement progress
// --- Shutter variables --- //



void setup(void) {
  Serial.begin(9600);
  tft.reset();

  pinMode(rearLimitPin, INPUT);
  pinMode(forwardLimitPin, INPUT);
  pinMode(zStickPin, INPUT_PULLUP);

  pinMode(shutterPin, OUTPUT);
  pinMode(stepperSleepPin, OUTPUT);
  digitalWrite(stepperSleepPin, HIGH);
  pinMode(stepperEnablePin, OUTPUT);
  digitalWrite(stepperEnablePin, LOW);

  stepper.setMaxSpeed(1600);
  stepper.setAcceleration(1400);

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
  currentTime = millis();

  // run AutoStack sequence if enabled
  if (autoStackFlag == 1 && pauseAutoStack == 0) {
    autoStack();
  }
  // needs to be called every loop
  stepper.run();
  // take touch reading
  if (currentTime - subRoutine1Time >= 50) {
    touchScreen();
    subRoutine1Time = millis();
  }
  // take joystick and limit switch reading, put stepper to sleep
  if (currentTime - subRoutine2Time >= 100) {
    // check joystick for movement
    xStickPos = analogRead(xStickPin);
    // move if past threshold and not in autoStack mode
    if ((xStickPos >= 600 || xStickPos <= 400) && autoStackFlag == 0) {
      joyStick();
    }
    // check limit switches
    if (digitalRead(rearLimitPin) + digitalRead(forwardLimitPin) < 2) {
      limitSwitch();
    }
    // sleep if stepper inactive, update position on manual screen
    if (stepper.isRunning() == 0 && (autoStackFlag == 0 || pauseAutoStack == 1) && digitalRead(stepperSleepPin) == HIGH) {
      digitalWrite(stepperSleepPin, LOW);
      // refresh position on manual screen after stepping completed
      if (prevStepperPosition != stepper.currentPosition() && activeScreen == 2) {
        displayPosition();
      }
    }
    // only increment per button click
    if (digitalRead(zStickPin) == HIGH) {
      prevZStickVal = zStickVal;
    }
    // increment joystick button for changing stepper speed
    if (digitalRead(zStickPin) == LOW && prevZStickVal == zStickVal) {
      zStickVal++;
      if (zStickVal > 3) {
        zStickVal = 1;
      }
    }

    subRoutine2Time = millis();
  }
  // reset target to currentPosition
  if (targetFlag == 1) {
    stepper.move(0);
    stepper.setSpeed(0);
    targetFlag = 0;
  }
  // run homing sequence if first loop
  if (bootFlag == 0) {
    bootFlag = 1;
    // homeRail();
  }
}

void touchScreen() {
  currentTime = millis(); // call frequently just in case

  TSPoint point = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (point.z > minPressure && point.z < maxPressure) {
    switch (activeScreen) {
      case 1:
        startScreenTouch(point); // main screen
        break;
      case 2:
        manualScreenTouch(point); // screen for manual mode
        break;
      case 3:
        autoScreenTouch(point); // screen for autoStack
        break;
      case 4:
        autoConfigScreenTouch(point); // autoStack config
        break;
    }
    // reset timer
    prevTouchTime = millis();
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
  }
}

void manualScreen() {
  activeScreen = 2;
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);

  // Step Distance
  // tft.fillRoundRect(5, 5, 125, 65, 14, iZettleGrey);
  tft.fillRoundRect(-10, 5, 135, 65, 15, iZettleBlue);
  tft.setCursor(5, 30);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Dist.");

  tft.setCursor(10, 60);
  tft.setFont(&Arimo_Bold_30);
  tft.println(String(distancePerMovement/1000, 4));

  // Step Number
  // tft.fillRoundRect(5, 85, 125, 65, 14, iZettleGrey);
  tft.fillRoundRect(-10, 85, 135, 65, 15, iZettleBlue);
  tft.setCursor(5, 110);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Nr.");

  tft.setCursor(30, 140);
  tft.setFont(&Arimo_Bold_30);
  tft.println(manualMovementCount);

  // Rail Posoition
  // tft.fillRoundRect(5, 165, 125, 65, 14, iZettleGrey);
  tft.fillRoundRect(-10, 165, 135, 65, 15, iZettleBlue);
  tft.setCursor(5, 190);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Rail Pos.");
  tft.setCursor(5, 220);
  tft.setFont(&Arimo_Bold_30);
  tft.println(String(stepper.currentPosition()*0.0025, 4));

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
  tft.setTextColor(iZettleRed);
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

  // toggle shutter on/off
  if ((xPos >= 145 && xPos <= 230) && (yPos >= 30 && yPos <= 50)) {
    if (int(currentTime - prevGenericTime) >= genericTouchDelay) {
      toggleShutter();
      tft.setCursor(150, 45);
      tft.setFont(&Arimo_Bold_20);
      if (shutterState == 1) {
        tft.setTextColor(iZettleGreen, BLACK);
      } else if (shutterState == 0) {
        tft.setTextColor(iZettleRed, BLACK);
      }
      tft.println("Shutter");

      prevGenericTime = millis();
    }
  }
  // reset manual movement count
  if ((xPos >= 145 && xPos <= 225) && (yPos >= 120 && yPos <= 140)) {
    int16_t x1, y1;
    uint16_t w, h;
    tft.setFont(&Arimo_Bold_30);
    tft.getTextBounds(String(manualMovementCount), 30, 140, &x1, &y1, &w, &h);
    tft.fillRect(x1, y1, w, h, iZettleBlue);

    manualMovementCount = 0;
    prevManualMovementCount = 0;
    tft.setTextColor(WHITE);
    tft.setCursor(30, 140);
    tft.println(manualMovementCount);
  }
  // step distance button
  if ((xPos >= 5 && xPos <= 130) && (yPos >= 5 && yPos <= 70)) {
    if (int(currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editMovementDistance = !editMovementDistance;

      if (arrowsActive == 0 && editMovementDistance == 0) {
        tft.setTextColor(WHITE);
        tft.setCursor(5, 30);
        tft.setFont(&Arimo_Regular_24);
        tft.println("Step Dist.");
        tft.setCursor(10, 60);
        tft.setFont(&Arimo_Bold_30);
        tft.println(String(distancePerMovement/1000, 4));
      }
      if (arrowsActive == 1 && editMovementDistance == 1) {
        tft.setTextColor(YELLOW);
        tft.setCursor(5, 30);
        tft.setFont(&Arimo_Regular_24);
        tft.println("Step Dist.");
        tft.setCursor(10, 60);
        tft.setFont(&Arimo_Bold_30);
        tft.println(String(distancePerMovement/1000, 4));
      }

      prevGenericTime = millis();
    }
  }
  // set step size
  if (arrowsActive == 1 && editMovementDistance == 1) {
    if (prevDistance != distancePerMovement) {
      prevDistance = distancePerMovement;
    }

    stepsPerMovement = arrowsTouch(point, 0, stepsPerMovement);
    setStepDistance();
  }
  // step motor by specified distance
  if (arrowsActive == 0) {
    int val = arrowsTouch(point, 1, 0);
  }
  // back button
  if ((xPos >= 145 && xPos <= 200) && (yPos >= 190 && yPos <= 210) && arrowsActive == 0) {
    startScreen();
  }
}

void autoScreen() {
  activeScreen = 3;
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);

  // Step Distance
  tft.fillRoundRect(-10, 5, 135, 65, 15, iZettleBlue);
  tft.setCursor(5, 30);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Dist.");

  tft.setCursor(10, 60);
  tft.setFont(&Arimo_Bold_30);
  tft.println(String(distancePerMovement/1000, 4));

  // Estimated time remaining
  tft.fillRoundRect(-10, 85, 135, 65, 15, iZettleBlue);
  tft.setCursor(5, 110);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Est. Time");

  // tft.setCursor(25, 140);
  // tft.setFont(&Arimo_Bold_30);
  // tft.println("4:53");
  estimateDuration();

  // Step progress
  tft.fillRoundRect(-10, 165, 135, 65, 15, iZettleBlue);
  tft.setCursor(5, 190);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Progress");

  tft.setCursor(20, 220);
  tft.setFont(&Arimo_Bold_24);
  tft.println(movementProgress);
  tft.setCursor(60, 220);
  tft.println("/");
  tft.setCursor(70, 220);
  tft.setFont(&Arimo_Bold_24);
  tft.println(numMovements);

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

  if (pauseAutoStack == 1 && autoStackFlag == 1) {
    drawPlayPause(0, 1);
  } else if (pauseAutoStack == 0 && autoStackFlag == 1) {
    drawPlayPause(1, 0);
  } else if (pauseAutoStack == 0 && autoStackFlag == 0) {
    drawPlayPause(0, 0);
  }
}

void autoScreenTouch(TSPoint &point) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());

  // shutter toggle
  if ((xPos >= 150 && xPos <= 230) && (yPos >= 32 && yPos <= 48)) {
    if (int(currentTime - prevGenericTime) >= genericTouchDelay) {
      toggleShutter();
      tft.setCursor(150, 45);
      tft.setFont(&Arimo_Bold_20);
      if (shutterState == 1) {
        tft.setTextColor(iZettleGreen, BLACK);
      } else if (shutterState == 0) {
        tft.setTextColor(iZettleRed, BLACK);
      }
      tft.println("Shutter");
      prevGenericTime = millis();
    }
  }
  // config button
  if ((xPos >= 150 && xPos <= 220) && (yPos >= 112 && yPos <= 128)) {
    autoConfigScreen();
  }
  // step distance button
  if ((xPos >= 5 && xPos <= 130) && (yPos >= 5 && yPos <= 70)) {
    if (int(currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editMovementDistance = !editMovementDistance;

      if (arrowsActive == 0 && editMovementDistance == 0) {
        tft.setTextColor(WHITE);
        tft.setCursor(5, 30);
        tft.setFont(&Arimo_Regular_24);
        tft.println("Step Dist.");
        tft.setCursor(10, 60);
        tft.setFont(&Arimo_Bold_30);
        tft.println(String(distancePerMovement/1000, 4));
      }
      if (arrowsActive == 1 && editMovementDistance == 1) {
        tft.setTextColor(YELLOW);
        tft.setCursor(5, 30);
        tft.setFont(&Arimo_Regular_24);
        tft.println("Step Dist.");
        tft.setCursor(10, 60);
        tft.setFont(&Arimo_Bold_30);
        tft.println(String(distancePerMovement/1000, 4));
      }

      prevGenericTime = millis();
    }
  }
  // set step size
  if (arrowsActive == 1 && editMovementDistance == 1) {
    if (prevDistance != distancePerMovement) {
      prevDistance = distancePerMovement;
    }

    stepsPerMovement = arrowsTouch(point, 0, stepsPerMovement);
    setStepDistance();
  }
  // run AutoStack sequence
  if (arrowsActive == 0 && (xPos >= 230 && xPos <= 310) && (yPos >= 30 && yPos <= 120)) {
    autoStackFlag = 1;
    pauseAutoStack = 0; // re-enable autoStack + pause button
    drawPlayPause(1, 0); // grey out play, recolour pause button
  }
  // pause/resume AutoStack sequence
  if (arrowsActive == 0 && pauseAutoStack == 0 && (xPos >= 230 && xPos <= 310) && (yPos >= 135 && yPos <= 225)) {
    pauseAutoStack = 1;
    drawPlayPause(0, 1); // recolour play, grey out pause button
  }
  // back button
  if ((xPos >= 150 && xPos <= 200) && (yPos >= 192 && yPos <= 208) && arrowsActive == 0) {
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
  tft.println(startPosition);

  // Set end point
  tft.setCursor(20, 130);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(iZettleRed);
  tft.println("END");
  tft.setCursor(35, 145);
  tft.setFont(&Arimo_Regular_16);
  tft.setTextColor(WHITE);
  tft.println(endPosition);

  // Run from start to end and return
  // move back from end point to start point (with accel)
  // then run from start to end with no accel
  tft.setCursor(20, 210);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(iZettleBlue);
  tft.println("RUN");
  tft.setCursor(35, 225);
  tft.setFont(&Arimo_Regular_16);
  tft.setTextColor(WHITE, BLACK);
  tft.println(stepper.currentPosition());

  // set delay
  tft.setCursor(150, 125);
  tft.setFont(&Arimo_Bold_24);
  tft.setTextColor(WHITE);
  tft.println("Delay");
  tft.setCursor(155, 143);
  tft.setFont(&Arimo_Regular_20);
  tft.setTextColor(WHITE, BLACK);
  tft.println(shutterDelay);
  tft.setCursor(175, 143);
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

  // start button
  if ((xPos >= 10 && xPos <= 140) && (yPos >= 10 && yPos <= 80) && (editShutterDelay + editEndPosition) == 0) {
    if (int(currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editStartPosition = !editStartPosition;

      if (arrowsActive == 0 && editStartPosition == 0) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, BLACK);
        tft.fillRoundRect(10, 24, 5, 40, 4, BLACK);
      }
      if (arrowsActive == 1 && editStartPosition == 1) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, YELLOW);
        tft.fillRoundRect(10, 24, 5, 40, 4, YELLOW);
      }

      prevGenericTime = millis();
    }
  }
  // set start position
  if (editStartPosition == 1 && arrowsActive == 1) {
    if (prevStartPosition != startPosition) {
      prevStartPosition = startPosition;
    }
    int val = arrowsTouch(point, 1, 0);
    setAutoStackPositions(1, 0); //set start but not end position
  }
  // end button
  if ((xPos >= 10 && xPos <= 120) && (yPos >= 90 && yPos <= 160) && (editShutterDelay + editStartPosition) == 0) {
    if (int(currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editEndPosition = !editEndPosition;

      if (arrowsActive == 0 && editEndPosition == 0) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, BLACK);
        tft.fillRoundRect(10, 104, 5, 40, 4, BLACK);
      }
      if (arrowsActive == 1 && editEndPosition == 1) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, YELLOW);
        tft.fillRoundRect(10, 104, 5, 40, 4, YELLOW);
      }

      prevGenericTime = millis();
    }
  }
  // set end position
  if (editEndPosition == 1 && arrowsActive == 1) {
    if (prevEndPosition != endPosition) {
      prevEndPosition = endPosition;
    }

    int val = arrowsTouch(point, 1, 0);
    setAutoStackPositions(0, 1); //set end but not start position
  }
  // run button
  if ((xPos >= 10 && xPos <= 120) && (yPos >= 170 && yPos <= 240) && (editShutterDelay + editStartPosition + editEndPosition) == 0) {
    tft.fillRoundRect(10, 184, 5, 35, 4, YELLOW);
    dryRun();
    tft.fillRoundRect(10, 184, 5, 35, 4, BLACK);
  }
  // delay button
  if ((xPos >= 150 && xPos <= 220) && (yPos >= 110 && yPos <= 145) && (editStartPosition + editEndPosition) == 0) {
    if (int(currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editShutterDelay = !editShutterDelay;

      if (arrowsActive == 0 && editShutterDelay == 0) {
        // tft.drawRoundRect(145, 100, 78, 50, 4, BLACK);
        tft.fillRoundRect(140, 108, 5, 35, 4, BLACK);
      }
      if (arrowsActive == 1 && editShutterDelay == 1) {
        // tft.drawRoundRect(145, 100, 78, 50, 4, YELLOW);
        tft.fillRoundRect(140, 108, 5, 35, 4, YELLOW);
      }

      prevGenericTime = millis();
    }
  }
  // set shutter delay
  if (editShutterDelay == 1 && arrowsActive == 1) {
    if (prevDelay != shutterDelay) {
      prevDelay = shutterDelay;
    }
    shutterDelay = arrowsTouch(point, 0, shutterDelay);
    setShutterDelay();
  }
  // show current position of stepper
  displayPosition();
  // back button - tft.setCursor(150, 205);
  if ((xPos >= 150 && xPos <= 200) && (yPos >= 192 && yPos <= 208) && arrowsActive == 0) {
    autoScreen();
  }

  Serial.print("editStartPosition: ");
  Serial.println(editStartPosition);
  Serial.print("editEndPosition: ");
  Serial.println(editEndPosition);
  Serial.print("editShutterDelay: ");
  Serial.println(editShutterDelay);
  Serial.print("arrowsActive: ");
  Serial.println(arrowsActive);
}

void drawArrows() {
  tft.fillTriangle(230, 65, 260, 15, 290, 65, iZettleGreen);
  tft.fillRoundRect(245, 69, 30, 36, 4, iZettleGreen);

  tft.fillTriangle(230, 175, 260, 225, 290, 175, iZettleRed);
  tft.fillRoundRect(245, 135, 30, 36, 4, iZettleRed);
}

void drawPlayPause(bool greyPlay = 0, bool greyPause = 0) {
  if (greyPlay == 1) {
    tft.fillCircle(270, 75, 35, iZettleGreyLite);
    tft.fillTriangle(255, 50, 295, 75, 255, 100, WHITE);
  } else {
    tft.fillCircle(270, 75, 35, iZettleGreen);
    tft.fillTriangle(255, 50, 295, 75, 255, 100, WHITE);
  }

  if (greyPause == 1) {
    tft.fillCircle(270, 180, 35, iZettleGreyLite);
    tft.fillRoundRect(250, 155, 15, 50, 4, WHITE);
    tft.fillRoundRect(275, 155, 15, 50, 4, WHITE);
  } else {
    tft.fillCircle(270, 180, 35, iZettleBlue);
    tft.fillRoundRect(250, 155, 15, 50, 4, WHITE);
    tft.fillRoundRect(275, 155, 15, 50, 4, WHITE);
  }
}

int arrowsTouch(TSPoint &point, bool stepMotor, int val = 0) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());

  if ((xPos >= 230 && xPos <= 290) && (yPos >= 15 && yPos <= 105)) {
    val++;
    if (stepMotor == 1) { // if stepping required, move motor
      if (shutterState == 1 && activeScreen == 2) {
        triggerShutter(1);
        delay(300);
        triggerShutter(0);
      }
      stepperStep(1, 500); // forward
    }
  }
  if ((xPos >= 230 && xPos <= 290) && (yPos >= 135 && yPos <= 225)) {
    val--;
    if (stepMotor == 1) { // if stepping required, move motor
      stepperStep(-1, 500); // reverse
    }
  }

  return val;
}

void autoStack() {
  // wake stepper from sleep
  if (digitalRead(stepperSleepPin) == LOW) {
    digitalWrite(stepperSleepPin, HIGH);
  }
  // move stepper to start if not already there
  if (goToStart == 1) {
    // Serial.println("moving to start");
    while (stepper.currentPosition() != startPosition) {
      // move to start
      stepper.setSpeed(-600);
      stepper.moveTo(startPosition);
      stepper.runSpeedToPosition();

      goToStart = 0;
      stepCount = 0;
      stepDue = 1;
      movementProgress = 0;
    }
    stepper.move(0);
    stepper.setSpeed(0);
    // Serial.println("arrived at start");
  }
  // take photo and increment progress for first step of each movement
  if (stepCount == 0 && stepDue == 1) {
    triggerShutter(1); // set trigger to high
    // delay(300);
    movementProgress++;

    // make sure correct screen is displaying
    if (activeScreen == 3) { // auto screen
      int16_t x1, y1;
      uint16_t w, h;
      tft.setFont(&Arimo_Bold_24);
      tft.getTextBounds(String(prevMovementProgress), 20, 220, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, iZettleBlue);
      tft.setTextColor(WHITE, iZettleBlue);
      tft.setCursor(20, 220);
      tft.println(movementProgress);
    }
    // prev movement progress value
    prevMovementProgress = movementProgress;
  }
  // keep stepping if not at end and haven't travelled a full movement
  if (movementProgress <= numMovements && stepDue == 1 && stepCount <= stepsPerMovement) {
    stepCount++;
    stepperStep(1, 0); // forward direction, no delay in ms
  }
  // reset stepCount for next movement
  if (stepCount > stepsPerMovement) {
    estimateDuration();
    // only reset after 400ms to give time for shutter release unless shutter disabled
    if (shutterState == 1 && int(currentTime - prevShutterTime) >= 1000) {
      stepCount = 0;
      stepDue = 0;
      prevShutterTime = millis();
      // reset trigger by setting low
      triggerShutter(0);
    } else if (shutterState == 0) {
      stepCount = 0;
      stepDue = 0;
    }
  }
  // wait delay period before re-enabling stepping
  if (stepDue == 0) {
    stepperStep(0, shutterDelay*1000); // don't step and wait for delay
  }
  // stop AutoStack sequence if end reached
  if (movementProgress >= numMovements) {
    autoStackFlag = 0;
    goToStart = 1;
    movementProgress = 0;
    drawPlayPause(0, 0); // reset play to green
    triggerShutter(0); // reset trigger by setting low
  }
}

void homeRail() {
  // wake stepper from sleep
  if(digitalRead(stepperSleepPin) == LOW) {
    digitalWrite(stepperSleepPin, HIGH);
  }

  while (digitalRead(rearLimitPin) == 1) { // start in reverse
    stepper.moveTo(-100000);
    stepper.run();
  }
  // stops motor instantly
  stepper.setSpeed(0);
  rearLimitPos = 0;
  stepper.setCurrentPosition(0); // set position

  while (digitalRead(forwardLimitPin) == 1) { // move to forward limit
    stepper.moveTo(100000);
    stepper.run();
  }
  // stops motor instantly
  stepper.setSpeed(0);
  // set forward position, move back to middle of rail
  forwardLimitPos = stepper.currentPosition(); // set position
  stepper.moveTo(forwardLimitPos / 2); // return to the middle
  stepper.runToPosition();
}

void dryRun() {
  // wake stepper from sleep
  if (digitalRead(stepperSleepPin) == LOW) {
    digitalWrite(stepperSleepPin, HIGH);
  }

  while (stepper.currentPosition() > startPosition) {
    stepper.setSpeed(-600);
    stepper.moveTo(startPosition);
    stepper.runSpeedToPosition();
  }

  displayPosition();

  while (stepper.currentPosition() < endPosition) {
    stepper.setSpeed(300);
    stepper.moveTo(endPosition);
    stepper.runSpeedToPosition();
  }

  displayPosition();

  while (stepper.currentPosition() > startPosition) {
    stepper.setSpeed(-600);
    stepper.moveTo(startPosition);
    stepper.runSpeedToPosition();
  }

  // stops motor instantly
  stepper.setSpeed(0);
  targetFlag = 1;
  movementProgress = 0;
  displayPosition();
}

void stepperStep(int stepDirection, int delay) {
  currentTime = millis();
  // step after elapsed amount of time
  if (int(currentTime - prevStepTime) > delay) {
    // wake stepper from sleep
    if (digitalRead(stepperSleepPin) == LOW) {
      digitalWrite(stepperSleepPin, HIGH);
    }

    int stepVelocity = stepDirection * stepsPerMovement;

    stepper.move(stepVelocity);
    stepper.setSpeed(40);
    stepper.runSpeed();
    // reset stepper target
    // targetFlag = 1;

    stepDue = 1;
    prevStepTime = currentTime;
  }
}

void joyStick() {
  // wake stepper from sleep
  if (digitalRead(stepperSleepPin) == LOW) {
    digitalWrite(stepperSleepPin, HIGH);
  }
  // while joystick is operated
  while (xStickPos >= 600 or xStickPos <= 400) {
    // move either -1, 0, or 1
    stepper.move(map(xStickPos, 0, 1023, -1, 1));
    stepper.runSpeed();

    // check stick position again
    xStickPos = analogRead(xStickPin);
    // reset stepper target
    targetFlag = 1;

    // check limit switches
    if (digitalRead(rearLimitPin) + digitalRead(forwardLimitPin) < 2) {
      stepper.stop();
      break;
    }

    // set speed based on joystick input
    int stepperSpeed = 600;
    switch (zStickVal) {
      case 1:
        stepperSpeed = 800;
        break;
      case 2:
        stepperSpeed = 600;
        break;
      case 3:
        stepperSpeed = 300;
        break;
    }

    // set speed direction
    if (xStickPos <= 400) {
      stepper.setSpeed(stepperSpeed*-1);
    }
    if (xStickPos >= 600) {
      stepper.setSpeed(stepperSpeed);
    }
  }

  // // check start/end position adjustment
  if (editStartPosition == 1 && arrowsActive == 1) {
    if (prevStartPosition != startPosition) {
      prevStartPosition = startPosition;
    }
    setAutoStackPositions(1, 0); //set start but not end position
  }
  if (editEndPosition == 1 && arrowsActive == 1) {
    if (prevEndPosition != endPosition) {
      prevEndPosition = endPosition;
    }
    setAutoStackPositions(0, 1); //set end but not start position
  }
  if (activeScreen == 2 or activeScreen == 4) {
    displayPosition();
  }
}

void toggleJoystick() {
  if (int(currentTime - prevGenericTime) >= genericTouchDelay) {
    joystickState = !joystickState;

    prevGenericTime = millis();
  }
}

void toggleShutter() {
  if (int(currentTime - prevGenericTime) >= genericTouchDelay) {
    shutterState = !shutterState;
    prevGenericTime = millis();
  }
}

void setShutterDelay() {

  if (shutterDelay < 1) {
    shutterDelay = 1;
  } else if (shutterDelay > 15) {
    shutterDelay = 15;
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

void triggerShutter(bool lowHigh) {
  if (shutterState == 1 && lowHigh == 1) {
    digitalWrite(shutterPin, HIGH);
  } else if (lowHigh == 0) {
    digitalWrite(shutterPin, LOW);
  }
}

void setStepDistance() {

  // constrain multiplier range
  if (stepsPerMovement < 1) {
    stepsPerMovement = 1;
  } else if (stepsPerMovement > 40) {
    stepsPerMovement = 40;
  }

  // stepper is 200 steps/rev, linear rail has 1mm pitch, 1/2 gear reduction
  // 1 step = 1/200/2 = 0.0025mm or 2.5μm
  distancePerMovement = 2.50 * stepsPerMovement;

  if (prevDistance != distancePerMovement) {
    // print new delay value
    int16_t x1, y1;
    uint16_t w, h;
    tft.setFont(&Arimo_Bold_30);
    tft.getTextBounds(String(prevDistance/1000, 4), 10, 60, &x1, &y1, &w, &h);
    tft.fillRect(x1, y1, w, h, iZettleBlue);
    tft.setCursor(10, 60);
    tft.setTextColor(YELLOW, iZettleBlue);
    tft.println(String(distancePerMovement/1000, 4));

    // calculate number of steps required to cover range
    numMovements = ceil((endPosition - startPosition)*1.00/stepsPerMovement);

    if (activeScreen == 3) {
      tft.setFont(&Arimo_Bold_24);
      tft.getTextBounds(String(prevNumMovements), 70, 220, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, iZettleBlue);
      tft.setTextColor(WHITE, iZettleBlue);
      tft.setCursor(70, 220);
      tft.println(numMovements);

      // update estimated time
      estimateDuration();
    }

    prevNumMovements = numMovements;
  }
}

void setAutoStackPositions(bool setStart, bool setEnd) {
  if (setStart == 1) {
    // lower limit
    if (startPosition < 0) {
      startPosition = 0;
    }
    // get new value
    startPosition = stepper.currentPosition();
    // print start point if changed
    if (prevStartPosition != startPosition) {
      int16_t x1, y1;
      uint16_t w, h;
      tft.getTextBounds(String(prevStartPosition), 35, 65, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, BLACK);
      tft.setCursor(35, 65);
      tft.setFont(&Arimo_Regular_16);
      tft.setTextColor(WHITE, BLACK);
      tft.println(startPosition);
    }
    // reset macroStepping
    goToStart = 1;
  }

  if (setEnd == 1) {
    // lower limit
    if (endPosition < 0) {
      endPosition = 0;
    }
    // get new value
    endPosition = stepper.currentPosition();
    // print end point if changed
    if (prevEndPosition != endPosition) {
      int16_t x1, y1;
      uint16_t w, h;
      tft.getTextBounds(String(prevEndPosition), 35, 145, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, BLACK);
      tft.setCursor(35, 145);
      tft.setFont(&Arimo_Regular_16);
      tft.setTextColor(WHITE, BLACK);
      tft.println(endPosition);
    }
  }

  // calculate number of steps required to cover range
  numMovements = ceil((endPosition - startPosition)/stepsPerMovement);
}

void displayPosition() {
  // update for new values
  if (prevStepperPosition != stepper.currentPosition()) {
    int16_t x1, y1;
    uint16_t w, h;

    if (activeScreen == 2) { // 2 = manual screen
      // update rail position value
      tft.setFont(&Arimo_Bold_30);
      tft.getTextBounds(String(prevStepperPosition*0.0025, 4), 5, 220, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, iZettleBlue);
      tft.setCursor(5, 220);
      tft.setTextColor(WHITE, iZettleBlue);
      tft.println(String(stepper.currentPosition()*0.0025, 4));

      // update movement count for manual screen
      manualMovementCount++;

      tft.setFont(&Arimo_Bold_30);
      tft.getTextBounds(String(prevManualMovementCount), 30, 140, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, iZettleBlue);
      tft.setTextColor(WHITE);
      tft.setCursor(30, 140);
      tft.println(manualMovementCount);

      prevManualMovementCount = manualMovementCount;
    }

    if (activeScreen == 4) { // 4 = autoConfigScreen
      tft.setFont(&Arimo_Regular_16);
      tft.getTextBounds(String(prevStepperPosition), 35, 225, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, BLACK);
      tft.setCursor(35, 225);
      tft.setTextColor(WHITE, BLACK);
      tft.println(stepper.currentPosition());
    }

    prevStepperPosition = stepper.currentPosition();
  }
}

void estimateDuration() {
  int duration = numMovements * shutterDelay;
  int elapsed = movementProgress * shutterDelay;
  int remaining = duration - elapsed;
  int minutes = remaining / 60;
  int seconds = remaining % 60;

  int16_t x, y;
  uint16_t w, h;
  tft.setFont(&Arimo_Bold_30);
  tft.setTextColor(WHITE);

  if (minutes != prevMinutes) {
    tft.getTextBounds(String(prevMinutes), 25, 140, &x, &y, &w, &h);
    tft.fillRect(x, y, w, h, iZettleBlue);
    tft.setCursor(25, 140);
    tft.println(minutes);

    prevMinutes = minutes;
  }
  tft.setCursor(50, 138);
  tft.println(":");
  if (seconds != prevSeconds) {
    tft.getTextBounds(String(prevSeconds), 65, 140, &x, &y, &w, &h);
    tft.fillRect(x, y, w, h, iZettleBlue);
    tft.setCursor(65, 140);
    tft.println(seconds);

    prevSeconds = seconds;
  }
}

void limitSwitch() {
  // wake stepper from sleep
  if(digitalRead(stepperSleepPin) == LOW) {
    digitalWrite(stepperSleepPin, HIGH);
  }

  while (digitalRead(rearLimitPin) == 0) { // switch closest to motor
    rearLimitPos = stepper.currentPosition();
    // move by 1 step until limit no longer triggered
    stepper.move(1);
    stepper.setSpeed(600);
    stepper.runSpeed();
    // reset stepper target
    targetFlag = 1;
  }

  while (digitalRead(forwardLimitPin) == 0) { //switch furtherest from motor
    forwardLimitPos = stepper.currentPosition();
    // move by -1 step until limit no longer triggered
    stepper.move(-1);
    stepper.setSpeed(-600);
    stepper.runSpeed();
    // reset stepper target
    targetFlag = 1;
  }
}
