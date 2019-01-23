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
#define BOXSIZE 40
#define PENRADIUS 3
#define MINPRESSURE 5
#define MAXPRESSURE 2000

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
void drawPlayPause();
void drawArrows();
void displayPosition();
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
void triggerShutter();
void limitSwitch();
// --- Set value functions --- //
void setStepDistance();
void setShutterDelay();
void setAutoStackPositions(bool setStart, bool setEnd);


// --- Timers and elapsed times --- //
unsigned long Timer = 0; // timer for staggering readings
unsigned long prevStepTimer = 0; // previous Timer reading

// --- Screen flags --- //
int activeScreen = 1; // currently displayed screen
bool arrowsActive = 0; // take arrow touch readings
bool editShutterDelay = 0; // set shutter delay time
bool editStartPosition = 0; // set start point for auto mode
bool editEndPosition = 0; // set end point for auto mode
bool editMovementDistance = 0; // set step distance in any mode
// --- Input and Output values --- //
int xStickPos = 0; // ADC value of x-axis joystick
int zStickVal = 1; // increment count of z-axis button press
int prevZStickVal = 1; // only increment per button press
int shutterDelay = 3; // delay between step and shutter trigger
int prevDelay = 3; // previous delay value
// --- Enable/Disable functionality --- //
bool bootFlag = 0; // runs rehoming sequence
bool goToStart = 1; // move to start for autoStack procedure
bool joystickState = 1; // enabled/disabled
bool autoStackFlag = 0; // enables function for stack procedure
bool pauseAutoStack = 0; // pause stack procedure
bool shutterState = 0; // disabled/enabled
bool stepDue = 0; // step is due when movement complete
bool targetFlag = 0; // resets stepper target
// --- Position values --- //
long rearLimitPos = 0; // limit switch nearest stepper
long forwardLimitPos = 10000; // limit switch furtherest from stepper
int startPosition = 0; // start position for stack procedure
int prevStartPosition = 0;
int endPosition = 0; // end position for stack procedure
int prevEndPosition = 0;
int prevStepperPosition = 1; // used for showing position of stepper if changed
// --- Stepper motor variables --- //
int stepsPerMovement = 1; // number of steps required for a given distance
int numMovements = 0; // equals total distance / step multiplier
int stepCount = 0; // number of steps taken in a given movement
double distancePerMovement = 2.50; // distance in micrometres travelled per movement
double prevDistance = 2.50; // previous step distance in micrometres
int movementProgress = 0; // number of completed movements
int prevMovementProgress = 0; // used for overwriting prev movement progress


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
  Timer = millis();
  // run MacroStepper sequence if set
  if (autoStackFlag == 1 && pauseAutoStack == 0) {
    autoStack();
  }
  // needs to be called every loop
  stepper.run();
  // take touch reading
  if (Timer % 75 == 0) {
    touchScreen();
  }
  // take joystick and limit switch reading, put stepper to sleep
  if (Timer % 100 == 0) {
    // check joystick for movement
    xStickPos = analogRead(xStickPin); // joystick
    if (xStickPos >= 600 or xStickPos <= 400) {
      joyStick();
    }
    // check limit switches
    if (digitalRead(rearLimitPin) + digitalRead(forwardLimitPin) < 2) {
      limitSwitch();
    }
    // sleep if stepper inactive
    if(stepper.isRunning() == 0 && digitalRead(stepperSleepPin) == HIGH) {
      digitalWrite(stepperSleepPin, LOW);
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
  TSPoint point = ts.getPoint();

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (point.z > MINPRESSURE && point.z < MAXPRESSURE) {
    if (activeScreen == 1) { // startScreen
      startScreenTouch(point);
    } else if (activeScreen == 2) { // manualScreen
      manualScreenTouch(point);
    } else if (activeScreen == 3) { // autoScreen
      autoScreenTouch(point);
    } else if (activeScreen == 4 && Timer % 50 == 0) { // autoConfigScreen
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
  tft.setTextColor(WHITE);

  // Step Distance
  // tft.fillRoundRect(5, 5, 125, 65, 14, iZettleGrey);
  tft.fillRoundRect(-10, 5, 135, 65, 15, iZettleBlue);
  tft.setCursor(5, 30);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Dist.");

  tft.setCursor(23, 60);
  tft.setFont(&Arimo_Bold_30);
  tft.println(distancePerMovement);

  // Step Number
  // tft.fillRoundRect(5, 85, 125, 65, 14, iZettleGrey);
  tft.fillRoundRect(-10, 85, 135, 65, 15, iZettleBlue);
  tft.setCursor(5, 110);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Nr.");

  tft.setCursor(45, 140);
  tft.setFont(&Arimo_Bold_30);
  tft.println("38");

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
  // step distance button - tft.fillRoundRect(5, 5, 125, 65, 14, iZettleGrey);
  if ((xPos >= 5 && xPos <= 130) && (yPos >= 5 && yPos <= 70)) {
    arrowsActive = !arrowsActive;
    editMovementDistance = !editMovementDistance;

    if (arrowsActive == 0 && editMovementDistance == 0) {
      tft.setTextColor(WHITE);
      tft.setCursor(5, 30);
      tft.setFont(&Arimo_Regular_24);
      tft.println("Step Dist.");
      tft.setCursor(23, 60);
      tft.setFont(&Arimo_Bold_30);
      tft.println(distancePerMovement);
    }
    if (arrowsActive == 1 && editMovementDistance == 1) {
      tft.setTextColor(YELLOW);
      tft.setCursor(5, 30);
      tft.setFont(&Arimo_Regular_24);
      tft.println("Step Dist.");
      tft.setCursor(23, 60);
      tft.setFont(&Arimo_Bold_30);
      tft.println(distancePerMovement);
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

  // show current position of stepper
  displayPosition();

  // back button - tft.setCursor(150, 205);
  if ((xPos >= 150 && xPos <= 200) && (yPos >= 192 && yPos <= 208) && arrowsActive == 0) {
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

  tft.setCursor(23, 60);
  tft.setFont(&Arimo_Bold_30);
  tft.println(distancePerMovement);

  // Estimated time remaining
  tft.fillRoundRect(-10, 85, 135, 65, 15, iZettleBlue);
  tft.setCursor(5, 110);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Est. Time");

  tft.setCursor(31, 140);
  tft.setFont(&Arimo_Bold_30);
  tft.println("4:53");

  // Step progress
  tft.fillRoundRect(-10, 165, 135, 65, 15, iZettleBlue);
  tft.setCursor(5, 190);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Progress");

  tft.setCursor(20, 220);
  tft.setFont(&Arimo_Bold_24);
  tft.println(movementProgress);
  tft.setCursor(65, 220);
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

  // drawArrows();
  drawPlayPause();
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
  // step distance button - tft.fillRoundRect(5, 5, 125, 65, 14, iZettleGrey);
  if ((xPos >= 5 && xPos <= 130) && (yPos >= 5 && yPos <= 70)) {
    arrowsActive = !arrowsActive;
    editMovementDistance = !editMovementDistance;

    if (arrowsActive == 0 && editMovementDistance == 0) {
      tft.setTextColor(WHITE);
      tft.setCursor(5, 30);
      tft.setFont(&Arimo_Regular_24);
      tft.println("Step Dist.");
      tft.setCursor(23, 60);
      tft.setFont(&Arimo_Bold_30);
      tft.println(distancePerMovement);
    }
    if (arrowsActive == 1 && editMovementDistance == 1) {
      tft.setTextColor(YELLOW);
      tft.setCursor(5, 30);
      tft.setFont(&Arimo_Regular_24);
      tft.println("Step Dist.");
      tft.setCursor(23, 60);
      tft.setFont(&Arimo_Bold_30);
      tft.println(distancePerMovement);
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

  // run MacroStepper sequence
  if (arrowsActive == 0 && (xPos >= 230 && xPos <= 310) && (yPos >= 30 && yPos <= 120)) {
      autoStackFlag = 1;
  }
  // pause/resume MacroStepper
  if (arrowsActive == 0 && (xPos >= 230 && xPos <= 310) && (yPos >= 135 && yPos <= 225)) {
    pauseAutoStack = !pauseAutoStack;
  }

  // back button - tft.setCursor(150, 205);
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

  // start button - tft.setCursor(20, 50);
  if ((xPos >= 10 && xPos <= 140) && (yPos >= 10 && yPos <= 80) && (editShutterDelay + editEndPosition) == 0) {
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
  }
  // set start position
  if (editStartPosition == 1 && arrowsActive == 1) {
    if (prevStartPosition != startPosition) {
      prevStartPosition = startPosition;
    }
    int val = arrowsTouch(point, 1, 0);
    setAutoStackPositions(1, 0); //set start but not end position
  }
  // end button - tft.setCursor(20, 130);
  if ((xPos >= 10 && xPos <= 120) && (yPos >= 90 && yPos <= 160) && (editShutterDelay + editStartPosition) == 0) {
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
  }
  // set end position
  if (editEndPosition == 1 && arrowsActive == 1) {
    if (prevEndPosition != endPosition) {
      prevEndPosition = endPosition;
    }

    int val = arrowsTouch(point, 1, 0);
    setAutoStackPositions(0, 1); //set end but not start position
  }
  // run button - tft.setCursor(20, 210);
  if ((xPos >= 10 && xPos <= 120) && (yPos >= 170 && yPos <= 240) && (editShutterDelay + editStartPosition + editEndPosition) == 0) {
    tft.fillRoundRect(10, 184, 5, 35, 4, YELLOW);
    dryRun();
    tft.fillRoundRect(10, 184, 5, 35, 4, BLACK);
  }
  // delay button - tft.setCursor(150, 125);
  if ((xPos >= 150 && xPos <= 220) && (yPos >= 110 && yPos <= 145) && (editStartPosition + editEndPosition) == 0) {
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
}

void autoStack() {
  // wake stepper from sleep
  if (digitalRead(stepperSleepPin) == LOW) {
    digitalWrite(stepperSleepPin, HIGH);
  }
  // move stepper to start if not already there
  if (goToStart == 1) {
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
  }
  // take photo and increment progress for first step of each movement
  if (stepCount == 0 && stepDue == 1) {
    triggerShutter();
    movementProgress++;

    // make sure correct screen is displaying
    if (activeScreen == 3) { // auto screen
      int16_t x1, y1;
      uint16_t w, h;
      tft.setFont(&Arimo_Bold_24);
      tft.getTextBounds(String(prevMovementProgress), 10, 220, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, iZettleBlue);
      tft.setTextColor(WHITE, iZettleBlue);
      tft.setCursor(10, 220);
      tft.println(movementProgress);
    }
    // prev step progress value
    prevMovementProgress = movementProgress;
  }
  // keep stepping if not at end and haven't travelled a full step
  // (e.g. if one step now equals 40 single steps)
  if (movementProgress <= numMovements && stepDue == 1 && stepCount <= stepsPerMovement) {
    stepCount++;
    stepperStep(1, 0); // 1 step, no delay in ms
  }
  // reset stepCount for next full-step
  if (stepCount > stepsPerMovement) {
    stepCount = 0;
    stepDue = 0;
  }
  // wait delay period before re-enabling stepping
  if (stepDue == 0) {
    stepperStep(0, shutterDelay*1000); // don't step and wait for delay
  }
  // stop MacroStepper sequence if end reached
  if (movementProgress >= numMovements) {
    autoStackFlag = 0;
    goToStart = 1;
    movementProgress = 0;
  }
}

void drawArrows() {
  tft.fillTriangle(230, 65, 260, 15, 290, 65, iZettleGreen);
  tft.fillRoundRect(245, 69, 30, 36, 4, iZettleGreen);

  tft.fillTriangle(230, 175, 260, 225, 290, 175, iZettleRed);
  tft.fillRoundRect(245, 135, 30, 36, 4, iZettleRed);
}

void drawPlayPause() {
  tft.fillCircle(270, 75, 35, iZettleGreen);
  tft.fillTriangle(255, 50, 295, 75, 255, 100, WHITE);

  // tft.fillRoundRect(235, 150, 20, 55, 5, iZettleBlue);
  // tft.fillRoundRect(265, 150, 20, 55, 5, iZettleBlue);
  tft.fillCircle(270, 180, 35, iZettleBlue);
  tft.fillRoundRect(250, 155, 15, 50, 4, WHITE);
  tft.fillRoundRect(275, 155, 15, 50, 4, WHITE);
}

int arrowsTouch(TSPoint &point, bool stepMotor, int val = 0) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());


  if ((xPos >= 230 && xPos <= 290) && (yPos >= 15 && yPos <= 105)) {
    val++;
    if (stepMotor == 1) { // if stepping required, move motor
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

  forwardLimitPos = stepper.currentPosition(); // set position
  stepper.moveTo(forwardLimitPos / 2); // return to the middle
  stepper.runToPosition();
}

void joyStick() {
  // wake stepper from sleep
  if(digitalRead(stepperSleepPin) == LOW) {
    digitalWrite(stepperSleepPin, HIGH);
  }
  // while joystick is operated
  while(xStickPos >= 600 or xStickPos <= 400) {
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
  joystickState = !joystickState;
}

void toggleShutter() {
  shutterState = !shutterState;
  if(shutterState == 1){
    digitalWrite(shutterPin, HIGH);
  } else if (shutterState == 0) {
    digitalWrite(shutterPin, LOW);
  }
}

void triggerShutter() {
  if(shutterState == 1){
    digitalWrite(shutterPin, HIGH);
    delay(200); // wait 200ms, change this to elapsed time in the future
    digitalWrite(shutterPin, LOW);
  }
}

void setShutterDelay() {

  if (shutterDelay < 1) {
    shutterDelay = 1;
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
    tft.getTextBounds(String(prevDistance, 2), 23, 60, &x1, &y1, &w, &h);
    tft.fillRect(x1, y1, w, h, iZettleBlue);
    tft.setCursor(23, 60);
    tft.setTextColor(YELLOW, iZettleBlue);
    tft.println(distancePerMovement);

    // calculate number of steps required to cover range
    numMovements = ceil((endPosition - startPosition)*1.00/stepsPerMovement);
    tft.setTextColor(WHITE, iZettleBlue);
    tft.fillRect(65, 195, 45, 25, iZettleBlue);
    tft.setCursor(65, 220);
    tft.setFont(&Arimo_Bold_24);
    tft.println(numMovements);
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
  displayPosition();
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

void stepperStep(int stepDirection, int delay) {
  Timer = millis();
  // step after elapsed amount of time
  if (int(Timer - prevStepTimer) > delay) {
    // wake stepper from sleep
    if(digitalRead(stepperSleepPin) == LOW) {
      digitalWrite(stepperSleepPin, HIGH);
    }

    int stepVelocity = stepDirection * stepsPerMovement;

    stepper.move(stepVelocity);
    stepper.setSpeed(40);
    stepper.runSpeed();
    // reset stepper target
    // targetFlag = 1;

    stepDue = 1;
    prevStepTimer = Timer;
  }
}

void displayPosition() {
  if (prevStepperPosition != stepper.currentPosition()) {
    int16_t x1, y1;
    uint16_t w, h;

    if (activeScreen == 2) { // 2 = manual screen
      tft.setFont(&Arimo_Bold_30);
      tft.getTextBounds(String(prevStepperPosition*0.0025, 4), 5, 220, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, iZettleBlue);
      tft.setCursor(5, 220);
      tft.setTextColor(WHITE, iZettleBlue);
      tft.println(String(stepper.currentPosition()*0.0025, 4));
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

void drawGrid() {
  for (int i = 1; i < 240/40; i++) {
    tft.drawFastHLine(0, 40*i, 320, WHITE);
  }
  for (int i = 1; i < 320/40; i++) {
    tft.drawFastVLine(40*i, 0, 240, WHITE);
  }
}
