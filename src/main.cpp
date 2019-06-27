// ********* Macro Stepper v1.1 *********
// *** AUTHOR: Cam Dey
// *** DATE: 2019-01-27
// *** UPDATED: 2019-06-24
// **************************************

// Macro Stepper was written to automate the focus stacking procedure commonly used
// in macrophotography. The manual mode allows the user to input the distance to travel
// per step (here called a "Movement") and whether to trigger the camera via shutter cable
// or not. The automatic mode ("AutoStack") will make a specified number of Movements
// based on the user inputted "Start" and "End" points, as well as the distance specified
// per Movement. A shutter delay can also be specified so that there is a pause before each Movement.
// This delay can be used to allow your camera flash time to recharge before the next shot.

// DEFINITIONS:
// STEP = one microstep step of the stepper stepper motor
// MOVEMENT = one full movement of linear rail by specified distance, consisting of multiple steps
// PROECDURE = one completed stack procedure, consisting of multiple movements
// DISTANCE = distance travelled per movement = number of steps per movement * 0.0025mm
// MANUAL MODE = user input required to step motor by specified distance
// AUTOSTACK = automatic mode, stepper completes procedure without further input

#include <Arduino.h>
#include <Adafruit_GFX.h>    									// core graphics library
#include <TouchScreen.h>											// touchscreen library
#include <MCUFRIEND_kbv.h>										// display driver for IL9481
#include <AccelStepper.h>											// software stepping implementation
#include <TMC2130Stepper.h> 									// stepper driver library
#include <TMC2130Stepper_REGDEFS.h>	  				// stepper driver registry definitions
#include <Arimo_Regular_10.h>
#include <Arimo_Regular_16.h>
#include <Arimo_Regular_18.h>
#include <Arimo_Regular_20.h>
#include <Arimo_Regular_22.h>
#include <Arimo_Regular_24.h>
#include <Arimo_Regular_30.h>
#include <Arimo_Bold_20.h>
#include <Arimo_Bold_24.h>
#include <Arimo_Bold_30.h>
#include <Syncopate_Bold_36.h>
#include <Permanent_Marker_Regular_36.h>
#include <Lato_Black_34.h>
#include <Aperture_Icon.h>	  								// icon for shutter state
#include <Back_Arrow_Icon.h>									// icon for navigation back to previous screen
#include <Cog_Wheel_Icon.h>										// icon for AutoStack config
#include <Delay_Clock_Icon.h>									// icon for setting shutter delay
#include <House_Icon.h>												// icon for HomeRail on main screen
#include <Main_Logo_Icon.h>										// icon for main logo on home screen
#include <Reset_Icon.h>												// icon for reseting/clearing data
#include <Reset_40_Icon.h>										// icon for reseting/clearing data


// Definitions for some common 16-bit color values:
#define	BLACK   						0x0000
#define	BLUE    						0x001F
#define	RED     						0xF800
#define	GREEN   						0x07E0
#define CYAN    						0x07FF
#define MAGENTA 						0xF81F
#define YELLOW  						0xFFE0
#define WHITE   						0xFFFF
#define GRAY    						0xE73C
#define DARKGRAY						0x39E8
#define CUSTOM_GREEN_LITE		0x9736
#define CUSTOM_GREEN				0x4ECC
#define CUSTOM_RED					0xFBCC
#define CUSTOM_RED_LITE			0xFCD1
#define CUSTOM_BLUE					0x4EDE
#define CUSTOM_GREY					0xCE7A
#define CUSTOM_GREY_LITE		0xDEFB

// touch screen pressure values
#define minPressure 			5
#define maxPressure 			2000
// drive microsteps value
#define nrMicrosteps			4
#define microstepDistance 1.25

// definitions for touch screen orientation
#define TS_MINX 					100
#define TS_MAXX 					920
#define TS_MINY 					70
#define TS_MAXY 					920

// pin definitions for touch inputs
#define YP 								A3 			// must be an analog pin, use "An" notation!
#define XM 								A2 			// must be an analog pin, use "An" notation!
#define YM 								9 			// can be a digital pin
#define XP 								8 			// can be a digital pin

// pin definitions for tft screen
#define LCD_CS 						A3
#define LCD_CD 						A2
#define LCD_WR 						A1
#define LCD_RD						A0
#define LCD_RESET 				A4

// driver pins
#define DIR_PIN 					36
#define STEP_PIN 					34
#define EN_PIN 						42
#define DIAG_PIN 					18
#define CS_PIN 						53
#define MOSI_PIN 					51
#define MISO_PIN 					52
#define SCK_PIN 					52

// misc hardware pins
#define XSTICK_PIN 				A9 			// joystick X-axis pin (controls fwd and rev)
#define ZSTICK_PIN 				A8 			// button-press from joystick
#define FLASH_PIN 				A5 			// pin for light sensor
#define SHUTTER_PIN 			30 			// pin for pulling camera focus and shutter to GND via transistor

TouchScreen ts 					= TouchScreen(XP, YP, XM, YM, 300);
TMC2130Stepper driver 	= TMC2130Stepper(EN_PIN, DIR_PIN, STEP_PIN, CS_PIN);
AccelStepper stepper 		= AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);
MCUFRIEND_kbv tft;

// --- Printing screen functions --- //
void startScreen();
void manualScreen();
void autoScreen();
void autoConfigScreen();
void drawPlayPause(bool greyPlay, bool greyPause);
void drawArrows();
void displayPosition();
void updateProgress(bool screenRefresh);
void estimateDuration(bool screenRefresh);
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
bool stepperStep(int direction, unsigned long stepperDelay);
void changeDirection();
void stallDetection();
// --- Enable/Disable functions --- //
void toggleJoystick();
void toggleShutter();
void toggleStepper(bool enable);
void resetAutoStack();
// --- Inputs and Outputs --- //
void joyStick();
bool triggerShutter();
bool flashStatus();
// --- Set value functions --- //
void setStepDistance();
void setShutterDelay();
void setAutoStackPositions(bool setStart, bool setEnd);
void stallGuardConfig();
void silentStepConfig();


// --- currentTimes and elapsed times --- //
unsigned long currentTime 				= 0;        // current time in millis()
unsigned long subRoutine1Time 		= 0;    		// time subroutine1 last ran
unsigned long subRoutine2Time 		= 0;    		// time subroutine2 last ran
unsigned long prevStepTime 				= 0;       	// previous step time reading
unsigned long recycleTime 				= 0;    		// duration to take photo
unsigned long prevGenericTime 		= 0;    		// generic timer for toggles and such
unsigned long genericTouchDelay 	= 200; 			// 200ms touch delay for toggles
int prevMinutes 									= 1;        // duration of autoStack
int prevSeconds 									= 1;        // duration of autoStack
char prevTimeMinutesSeconds[6] 		= "00:00"; 	// previous duration time

// --- Screen flags --- //
int activeScreen 									= 1;        // currently displayed screen
bool arrowsActive 								= false;        // take arrow touch readings
bool editShutterDelay 						= false;      	// set shutter delay time
bool editStartPosition 						= false;     		// set start point for auto mode
bool editEndPosition 							= false;       	// set end point for auto mode
bool editMovementDistance 				= false;  			// set step distance in any mode
// --- Input and Output values --- //
int xStickPos 										= 0;        // ADC value of x-axis joystick
int zStickVal 										= 1;        // increment count of z-axis button press
int prevZStickVal 								= 1;        // only increment per button press
int shutterDelay 									= 1;        // delay between step and shutter trigger
int prevDelay 										= 1;        // previous delay value
int joystick_speed 								= 0;				// joystick speed value
// --- Enable/Disable functionality --- //
bool bootFlag 										= false;        // runs rehoming sequence
bool goToStart 										= true;        // move to start for autoStack procedure
bool joystickState 								= true;        // enabled/disabled
bool autoStackFlag 								= false;        // enables function for stack procedure
bool pauseAutoStack 							= false;        // pause stack procedure
bool shutterState 								= false;        // disabled/enabled
// bool stepDue 											= 0;        // step is due when movement complete // seems unused
bool targetFlag 									= false;        // resets stepper target
bool flashReady 									= false;				// flash ready for next photo
bool stallGuardConfigured 				= true;				// stallGuard config has run
// --- Position values --- //
int startPosition 								= 0;        // start position for stack procedure
int prevStartPosition 						= 0;
int endPosition 									= 0;        // end position for stack procedure
int prevEndPosition 							= 0;
int prevStepperPosition 					= 1;    		// used for showing position of stepper if changed
int manualMovementCount 					= 0;    		// count of manual movements
int prevManualMovementCount 			= 0;
volatile long moveDist 						= 500000; 	// distance for homing
volatile bool stepperDisabled 		= false;
volatile bool directionFwd 				= true;
long fwdPosition 									= 9999;
long bwdPosition 									= 9999;
// --- Stepper motor variables --- //
int stepsPerMovement 							= 1;       	// number of steps required for a given distance
int numMovements 									= 0;        // equals total distance / step multiplier
int prevNumMovements 							= 1;       	// previous numMovements value
int stepCount 										= 0;        // number of steps taken in a given movement
double distancePerMovement 				= 0; 				// distance in micrometres travelled per movement
double prevDistance 							= 0;     		// previous step distance in micrometres
int movementProgress 							= 0;       	// number of completed movements
int prevMovementProgress 					= 1;   			// used for overwriting prev movement progress
char prevProgressMovements[10] 		= "0/0"; 		// used for overwriting prev movement progress
bool stepperMoved 								= false; 		// did stepperStep actually step or not
bool shutterTriggered 						= false;		// did the shutter trigger or not

// ***** --- PROGRAM --- ***** //

void setup(void) {
  Serial.begin(250000);
	SPI.begin();
  tft.reset();

	driver.begin();
	driver.rms_current(900);
	driver.microsteps(nrMicrosteps);
	driver.interpolate(1);

	pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(DIR_PIN, LOW);
  pinMode(DIAG_PIN, INPUT);
  pinMode(ZSTICK_PIN, INPUT_PULLUP);
  pinMode(SHUTTER_PIN, OUTPUT);
  digitalWrite(SHUTTER_PIN, LOW);
  attachInterrupt(digitalPinToInterrupt(DIAG_PIN), stallDetection, RISING);

  // set stepper AccelStepper config
  stepper.setMaxSpeed(3200);
  stepper.setAcceleration(2000);
  stepper.setEnablePin(EN_PIN);
  stepper.setPinsInverted(false, false, true);
  stepper.enableOutputs();
  stepperDisabled = false;
	stallGuardConfig();

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

	// if holding down ZSTICK_PIN, don't home rail
	if (digitalRead(ZSTICK_PIN) == LOW) {
		bootFlag = true;
	}
}

void loop() {
	currentTime = millis();

  // run AutoStack sequence if enabled
  if (autoStackFlag == true && pauseAutoStack == false) {
    autoStack();
  }
  // take touch reading
  if (currentTime - subRoutine1Time >= 50) {
    touchScreen();
    subRoutine1Time = millis();
  }
  // take joystick and limit switch reading, put stepper to sleep
  if (currentTime - subRoutine2Time >= 100) {
    // check joystick for movement
    xStickPos = analogRead(XSTICK_PIN);
    // move if past threshold and not in autoStack mode
    if ((xStickPos >= 550 || xStickPos <= 450) && autoStackFlag == false) {
      joyStick();
    }
    // sleep if stepper inactive, update position on manual screen
    if (stepper.distanceToGo() == 0 && (autoStackFlag == false || pauseAutoStack == true) && digitalRead(EN_PIN) == LOW) {
      toggleStepper(0); // disable stepper
      // refresh position on manual screen after stepping completed
      if (prevStepperPosition != stepper.currentPosition() && activeScreen == 2) {
        displayPosition();
      }
    }
    // toggle output if joystick pressed
    if (digitalRead(ZSTICK_PIN) == LOW) {
			if (stepperDisabled == false) {
      	toggleStepper(0);
				stepperDisabled = true;
			}
			else if (stepperDisabled == true) {
				toggleStepper(1);
				stepperDisabled = false;
			}
    }
		// configure SilentStep if not homing rail
		if (stallGuardConfigured == true && bootFlag == true) {
			silentStepConfig();
		}

    subRoutine2Time = millis();
  }
  // reset target to currentPosition
  if (targetFlag == true) {
    stepper.move(0);
    stepper.setSpeed(0);
    targetFlag = false;
  }
  // run homing sequence if first loop
  if (bootFlag == false) {
    bootFlag = true;
    // homeRail();
		setAutoStackPositions(1, 1);
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
  }
}

void startScreen() {
  activeScreen = 1;
  tft.fillScreen(BLACK);

  tft.drawBitmap(40, 0, logo, 240, 82, WHITE);

  tft.setTextColor(BLACK);
  // Manual option
  tft.fillRoundRect(20, 100, 130, 60, 8, CUSTOM_BLUE);
  tft.setCursor(30, 140);
  tft.setFont(&Arimo_Bold_30);
  tft.println("Manual");

  // Auto option
  tft.fillRoundRect(170, 100, 130, 60, 8, CUSTOM_GREEN);
  tft.setCursor(200,140);
  tft.setFont(&Arimo_Bold_30);
  tft.println("Auto");

  // Homing option
  // tft.fillRect(0, 200, 320, 40, CUSTOM_BLUE);
  tft.drawBitmap(30, 190, house, 50, 42, WHITE);

  // Clear autoStack
  if (autoStackFlag == false) {
    tft.drawBitmap(250, 190, reset40, 40, 40, GRAY);
  } else if (autoStackFlag == true) {
    tft.drawBitmap(250, 190, reset40, 40, 40, CUSTOM_RED);
  }
}

void startScreenTouch(TSPoint &point) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());

  // manual box
  if ((xPos >= 20 && xPos <= 150) && (yPos >= 90 && yPos <= 160)) {
    manualScreen();
  }
  // auto box
  if ((xPos >= 170 && xPos <= 300) && (yPos >= 90 && yPos <= 160)) {
    autoScreen();
  }
  // homing func
  if ((xPos >= 20 && xPos <= 100) && (yPos >= 190 && yPos <= 240)) {
    tft.drawBitmap(30, 190, house, 50, 42, CUSTOM_RED);
    homeRail();
    tft.drawBitmap(30, 190, house, 50, 42, WHITE);
  }
  // clear autoStack run
  if ((xPos >= 230 && xPos <= 320) && (yPos >= 200 && yPos <= 240) && autoStackFlag == true) {
    resetAutoStack(); // reset autostack settings
  }
}

void manualScreen() {
  activeScreen = 2;
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);

  // Step Distance
  // tft.fillRoundRect(5, 5, 125, 65, 14, CUSTOM_GREY);
  tft.fillRoundRect(-10, 5, 135, 65, 15, CUSTOM_BLUE);
  tft.setCursor(5, 30);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Dist.");

  tft.setCursor(10, 60);
  tft.setFont(&Arimo_Bold_30);
  tft.println(String(distancePerMovement/1000, 4));

  // Step Number
  // tft.fillRoundRect(5, 85, 125, 65, 14, CUSTOM_GREY);
  tft.fillRoundRect(-10, 85, 135, 65, 15, CUSTOM_BLUE);
  tft.setCursor(5, 110);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Nr.");

  tft.setCursor(45, 140);
  tft.setFont(&Arimo_Bold_30);
  tft.println(manualMovementCount);

  // Rail Posoition
  // tft.fillRoundRect(5, 165, 125, 65, 14, CUSTOM_GREY);
  tft.fillRoundRect(-10, 165, 135, 65, 15, CUSTOM_BLUE);
  tft.setCursor(5, 190);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Rail Pos.");
  tft.setCursor(9, 220);
  tft.setFont(&Arimo_Bold_30);
  tft.println(String(stepper.currentPosition()*0.0025, 4));

  // auto shutter
  if (shutterState == true) {
    tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_GREEN);
  } else if (shutterState == false) {
    tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_RED);
  }

  // reset steps
  tft.drawBitmap(155, 95, reset, 50, 50, WHITE);

  // back to main screen
  tft.drawBitmap(155, 175, backArrow, 50, 50, WHITE);

  drawArrows();
}

void manualScreenTouch(TSPoint &point) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());

  // toggle shutter on/off
  if ((xPos >= 150 && xPos <= 220) && (yPos >= 10 && yPos <= 70)) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      toggleShutter();
      if (shutterState == true) {
        tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_GREEN);
      } else if (shutterState == false) {
        tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_RED);
      }

      prevGenericTime = millis();
    }
  }
  // reset manual movement count
  if ((xPos >= 145 && xPos <= 220) && (yPos >= 90 && yPos <= 150)) {
    int16_t x1, y1;
    uint16_t w, h;
    tft.setFont(&Arimo_Bold_30);
    tft.getTextBounds(String(manualMovementCount), 45, 140, &x1, &y1, &w, &h);
    tft.fillRect(x1, y1, w, h, CUSTOM_BLUE);

    manualMovementCount = 0;
    prevManualMovementCount = 0;
    tft.setTextColor(WHITE);
    tft.setCursor(45, 140);
    tft.println(manualMovementCount);
  }
  // step distance button
  if ((xPos >= 5 && xPos <= 130) && (yPos >= 5 && yPos <= 70)) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editMovementDistance = !editMovementDistance;

      if (arrowsActive == false && editMovementDistance == false) {
        tft.setTextColor(WHITE);
        tft.setCursor(5, 30);
        tft.setFont(&Arimo_Regular_24);
        tft.println("Step Dist.");
        tft.setCursor(10, 60);
        tft.setFont(&Arimo_Bold_30);
        tft.println(String(distancePerMovement/1000, 4));
      }
      if (arrowsActive == 1 && editMovementDistance == true) {
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
  if (arrowsActive == 1 && editMovementDistance == true) {
    if (prevDistance != distancePerMovement) {
      prevDistance = distancePerMovement;
    }

    stepsPerMovement = arrowsTouch(point, 0, stepsPerMovement);
    setStepDistance();
  }
  // step motor by specified distance
  if (arrowsActive == false) {
    int val = arrowsTouch(point, 1, 0);
  }
  // back button
  if ((xPos >= 150 && xPos <= 210) && (yPos >= 170 && yPos <= 225) && arrowsActive == false) {
    startScreen();
  }
}

void autoScreen() {
  activeScreen = 3;
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);

  // Step Distance
  tft.fillRoundRect(-10, 5, 135, 65, 15, CUSTOM_BLUE);
  tft.setCursor(5, 30);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Dist.");
  tft.setCursor(10, 60);
  tft.setFont(&Arimo_Bold_30);
  tft.println(String(distancePerMovement/1000, 4));

  // Estimated time remaining
  tft.fillRoundRect(-10, 85, 135, 65, 15, CUSTOM_BLUE);
  tft.setCursor(5, 110);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Est. Time");
  // show estimated time
  estimateDuration(1);

  // Step progress
  tft.fillRoundRect(-10, 165, 135, 65, 15, CUSTOM_BLUE);
  tft.setCursor(5, 190);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Progress");
  // show progress, force update due to screen change
  updateProgress(1);

  // auto shutter
  if (shutterState == true) {
    tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_GREEN);
  } else if (shutterState == false) {
    tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_RED);
  }

  // enter auto config
  tft.drawBitmap(155, 95, cogWheel, 50, 50, GRAY);
  // back to main screen
  tft.drawBitmap(155, 175, backArrow, 50, 50, WHITE);

  if (pauseAutoStack == true && autoStackFlag == true) {
    drawPlayPause(0, 1);
  } else if (pauseAutoStack == false && autoStackFlag == true) {
    drawPlayPause(1, 0);
  } else if (pauseAutoStack == false && autoStackFlag == false) {
    drawPlayPause(0, 0);
  }
}

void autoScreenTouch(TSPoint &point) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());

  // shutter toggle
  if ((xPos >= 140 && xPos <= 210) && (yPos >= 10 && yPos <= 70)) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      toggleShutter();
      if (shutterState == true) {
        tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_GREEN);
      } else if (shutterState == false) {
        tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_RED);
      }
      prevGenericTime = millis();
    }
  }
  // config button
  if ((xPos >= 140 && xPos <= 210) && (yPos >= 80 && yPos <= 150)) {
    autoConfigScreen();
  }
  // step distance button
  if ((xPos >= 5 && xPos <= 130) && (yPos >= 5 && yPos <= 70)) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editMovementDistance = !editMovementDistance;

      if (arrowsActive == false && editMovementDistance == false) {
        tft.setTextColor(WHITE);
        tft.setCursor(5, 30);
        tft.setFont(&Arimo_Regular_24);
        tft.println("Step Dist.");
        tft.setCursor(10, 60);
        tft.setFont(&Arimo_Bold_30);
        tft.println(String(distancePerMovement/1000, 4));
      }
      if (arrowsActive == 1 && editMovementDistance == true) {
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
  if (arrowsActive == 1 && editMovementDistance == true) {
    if (prevDistance != distancePerMovement) {
      prevDistance = distancePerMovement;
    }

    stepsPerMovement = arrowsTouch(point, 0, stepsPerMovement);
    setStepDistance();
  }
  // run AutoStack sequence
  if (arrowsActive == false && (xPos >= 230 && xPos <= 310) && (yPos >= 30 && yPos <= 120)) {
    autoStackFlag = true;
    pauseAutoStack = false; // re-enable autoStack + pause button
    drawPlayPause(1, 0); // grey out play, recolour pause button
  }
  // pause/resume AutoStack sequence
  if (arrowsActive == false && pauseAutoStack == false && autoStackFlag == true && (xPos >= 230 && xPos <= 310) && (yPos >= 135 && yPos <= 225)) {
    pauseAutoStack = true;
    drawPlayPause(0, 1); // recolour play, grey out pause button
  }
  // back button
  if ((xPos >= 150 && xPos <= 210) && (yPos >= 170 && yPos <= 225) && arrowsActive == false) {
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
  tft.setTextColor(CUSTOM_GREEN);
  tft.println("START");
  tft.setCursor(35, 65);
  tft.setFont(&Arimo_Regular_16);
  tft.setTextColor(WHITE);
  tft.println(startPosition);

  // Set end point
  tft.setCursor(20, 130);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(CUSTOM_RED);
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
  tft.setTextColor(CUSTOM_BLUE);
  tft.println("RUN");
  tft.setCursor(35, 225);
  tft.setFont(&Arimo_Regular_16);
  tft.setTextColor(WHITE, BLACK);
  tft.println(stepper.currentPosition());

  // set delay
  tft.drawBitmap(160, 70, delayClock, 50, 50, WHITE);
  tft.setFont(&Arimo_Regular_20);
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(155, 143);
  tft.println(shutterDelay);
  tft.setCursor(175, 143);
  tft.println(" sec");

  // back to auto screen
  tft.drawBitmap(155, 175, backArrow, 50, 50, WHITE);

  drawArrows();
}

void autoConfigScreenTouch(TSPoint &point) {
  // scale from 0->1023 to tft dimension and swap coordinates
  int xPos = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());
  int yPos = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());

  // start button
  if ((xPos >= 10 && xPos <= 140) && (yPos >= 10 && yPos <= 80) && (editShutterDelay + editEndPosition) == 0) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editStartPosition = !editStartPosition;

      if (arrowsActive == 0 && editStartPosition == false) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, BLACK);
        tft.fillRoundRect(10, 24, 5, 40, 4, BLACK);
      }
      if (arrowsActive == 1 && editStartPosition == true) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, YELLOW);
        tft.fillRoundRect(10, 24, 5, 40, 4, YELLOW);
      }

      prevGenericTime = millis();
    }
  }
  // set start position
  if (editStartPosition == true && arrowsActive == 1) {
    if (prevStartPosition != startPosition) {
      prevStartPosition = startPosition;
    }
    int val = arrowsTouch(point, 1, 0);
    setAutoStackPositions(1, 0); //set start but not end position
  }
  // end button
  if ((xPos >= 10 && xPos <= 120) && (yPos >= 90 && yPos <= 160) && (editShutterDelay + editStartPosition) == 0) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editEndPosition = !editEndPosition;

      if (arrowsActive == 0 && editEndPosition == false) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, BLACK);
        tft.fillRoundRect(10, 104, 5, 40, 4, BLACK);
      }
      if (arrowsActive == 1 && editEndPosition == true) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, YELLOW);
        tft.fillRoundRect(10, 104, 5, 40, 4, YELLOW);
      }

      prevGenericTime = millis();
    }
  }
  // set end position
  if (editEndPosition == true && arrowsActive == 1) {
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
  if ((xPos >= 150 && xPos <= 220) && (yPos >= 60 && yPos <= 130) && (editStartPosition + editEndPosition) == 0) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editShutterDelay = !editShutterDelay;

      if (arrowsActive == 0 && editShutterDelay == false) {
        tft.drawBitmap(160, 70, delayClock, 50, 50, WHITE);
      }
      if (arrowsActive == 1 && editShutterDelay == true) {
        tft.drawBitmap(160, 70, delayClock, 50, 50, YELLOW);
      }

      prevGenericTime = millis();
    }
  }
  // set shutter delay
  if (editShutterDelay == true && arrowsActive == 1) {
    if (prevDelay != shutterDelay) {
      prevDelay = shutterDelay;
    }
    shutterDelay = arrowsTouch(point, 0, shutterDelay);
    setShutterDelay();
  }
  // show current position of stepper
  displayPosition();
  // back button - tft.setCursor(150, 205);
  if ((xPos >= 150 && xPos <= 210) && (yPos >= 170 && yPos <= 225) && arrowsActive == 0) {
    autoScreen();
  }

}

void drawArrows() {
  tft.fillTriangle(230, 65, 260, 15, 290, 65, CUSTOM_GREEN);
  tft.fillRoundRect(245, 69, 30, 36, 4, CUSTOM_GREEN);

  tft.fillTriangle(230, 175, 260, 225, 290, 175, CUSTOM_RED);
  tft.fillRoundRect(245, 135, 30, 36, 4, CUSTOM_RED);
}

void drawPlayPause(bool greyPlay = 0, bool greyPause = 0) {
  if (greyPlay == 1) {
    tft.fillCircle(270, 75, 37, CUSTOM_GREY_LITE);
    tft.fillTriangle(255, 50, 295, 75, 255, 100, WHITE);
  } else {
    tft.fillCircle(270, 75, 37, CUSTOM_GREEN);
    tft.fillTriangle(255, 50, 295, 75, 255, 100, WHITE);
  }

  if (greyPause == 1) {
    tft.fillCircle(270, 180, 37, CUSTOM_GREY_LITE);
    tft.fillRoundRect(250, 155, 15, 50, 4, WHITE);
    tft.fillRoundRect(275, 155, 15, 50, 4, WHITE);
  } else {
    tft.fillCircle(270, 180, 37, CUSTOM_BLUE);
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
    if (stepMotor == 1) { // if stepping required, trigger shutter, move step
      if (shutterState == true && activeScreen == 2) {
        shutterTriggered = triggerShutter();
      }
      stepperMoved = stepperStep(1, 500); // forward
    }
  }
  if ((xPos >= 230 && xPos <= 290) && (yPos >= 135 && yPos <= 225)) {
    val--;
    if (stepMotor == 1) { // if stepping required, move motor
      stepperMoved = stepperStep(-1, 500); // reverse
    }
  }

  return val;
}

void autoStack() {
  // wake stepper from sleep
  if (digitalRead(EN_PIN) == HIGH) {
    toggleStepper(1);
  }
  // move stepper to start if not already there
  if (goToStart == true) {
    while (stepper.currentPosition() != startPosition) {
      // move to start
      stepper.setSpeed(-600);
      stepper.moveTo(startPosition);
      stepper.runSpeedToPosition();
    }
		goToStart = false;
		movementProgress = 0;
		joystickState = false;
		stepperMoved = false;
		shutterTriggered = false;
    // stepper.move(0);
    // stepper.setSpeed(0);
  }
  // take photo and increment progress for first step of each movement
  if (movementProgress <= numMovements && stepperMoved == false) {
		// if shutter enabled, take photo
		if (shutterState == true && shutterTriggered == false) {
	    shutterTriggered = triggerShutter(); // take photo
		}

		// move stepper
		stepperMoved = stepperStep(1, shutterDelay*1000); // forward direction, shutterdelay

		if (stepperMoved == true) {
			movementProgress++;
	    // make sure correct screen is displaying
	    if (activeScreen == 3) { // auto screen
	      updateProgress(0); // don't force refresh
				estimateDuration(0); // don't force refresh
	    }
			shutterTriggered = false; // reset shutter
			stepperMoved = false; // reset stepper move
		}
  }
  // stop AutoStack sequence if end reached
  if (movementProgress >= numMovements) {
		estimateDuration(1); // force refresh
    autoStackFlag = false;
    goToStart = true;
    movementProgress = 0;
    joystickState = true;
		stepperMoved = false;
		shutterTriggered = false;
    drawPlayPause(0, 0); // reset play to green
  }
}

void homeRail() {
	// set configuration for stallGuard
	if (stallGuardConfigured == false) {
		stallGuardConfig();
	}
	stepper.moveTo(moveDist);
	// if in fwd direction, reverse direction
	if (directionFwd == true) {
		changeDirection();
	}
	// while back position not set and forward position not set, run to -50000 or stall
	// after stall, change direction and run to 50000 or stall
	while (bwdPosition != 0 || fwdPosition <= 9999) {
		stepper.run();
	}
	// if back and forward position set, move to middle position
	if (bwdPosition == 0 && fwdPosition > 10000) {
		bootFlag = true;
		stepper.moveTo(fwdPosition / 2);
		while (stepper.distanceToGo() != 0) {
			stepper.run();
		}
	}
}

void dryRun() {
  // wake stepper from sleep
	if (digitalRead(EN_PIN) == HIGH) {
    toggleStepper(1);
  }

  // move to start
  while (stepper.currentPosition() > startPosition) {
    stepper.setSpeed(-600);
    stepper.moveTo(startPosition);
    stepper.runSpeedToPosition();
  }
  displayPosition();

  // step slow through procedure
  while (stepper.currentPosition() < endPosition) {
    stepper.setSpeed(100);
    stepper.moveTo(endPosition);
    stepper.runSpeedToPosition();
  }
  displayPosition();

  // return to start
  while (stepper.currentPosition() > startPosition) {
    stepper.setSpeed(-600);
    stepper.moveTo(startPosition);
    stepper.runSpeedToPosition();
  }

  // stops motor instantly
  stepper.setSpeed(0);
  targetFlag = true;
  movementProgress = 0;
  displayPosition();
}

bool stepperStep(int stepDirection, unsigned long stepperDelay) {
  currentTime = millis();

  // step after elapsed amount of time
  if (currentTime - prevStepTime > stepperDelay) {
    // wake stepper from sleep
		if (digitalRead(EN_PIN) == HIGH) {
	    toggleStepper(1);
	  }

    int stepVelocity = stepDirection * stepsPerMovement;

    stepper.move(stepVelocity);
		stepper.setSpeed(200*stepDirection);
		while (stepper.distanceToGo() != 0) {
	    stepper.runSpeed();
			// stepDue = true;
		}

    prevStepTime = currentTime;
		return true;
  } else {
			return false;
	}
}

void joyStick() {
  // wake stepper from sleep
	if (stepperDisabled == true) {
    toggleStepper(1);
  }
  // while joystick is operated
  while (xStickPos > 515 || xStickPos < 495) {
    // move either -1, 0, or 1 steps
    stepper.move(map(xStickPos, 0, 1023, 1, -1));
    joystick_speed = map((xStickPos - 508), -508, 515, 2000, -2000);

    stepper.setSpeed(joystick_speed);
    stepper.runSpeed();

    // check stick position again
    if (millis() % 50 == 0) {
       xStickPos = analogRead(XSTICK_PIN);
    }
  }

  // // check start/end position adjustment
  if (editStartPosition == true && arrowsActive == 1) {
    if (prevStartPosition != startPosition) {
      prevStartPosition = startPosition;
    }
    setAutoStackPositions(1, 0); //set start but not end position
  }
  if (editEndPosition == true && arrowsActive == 1) {
    if (prevEndPosition != endPosition) {
      prevEndPosition = endPosition;
    }
    setAutoStackPositions(0, 1); //set end but not start position
  }
  if (activeScreen == 2 || activeScreen == 4) {
    displayPosition();
  }
  // update prev position
  prevStepperPosition = stepper.currentPosition();
}

void toggleJoystick() {
  if ((currentTime - prevGenericTime) >= genericTouchDelay) {
    joystickState = !joystickState;
    prevGenericTime = millis();
  }
}

void toggleShutter() {
  if ((currentTime - prevGenericTime) >= genericTouchDelay) {
    shutterState = !shutterState;
    prevGenericTime = millis();
  }
}

void resetAutoStack() {
  if ((currentTime - prevGenericTime) >= genericTouchDelay) {
    autoStackFlag = false;
    goToStart = true;
    joystickState = true;
    pauseAutoStack = false;
    // stepDue = true;
    stepCount = 1;
    movementProgress = 0;
    numMovements = 0;
    startPosition = stepper.currentPosition();
    endPosition = 0;
    tft.drawBitmap(250, 190, reset40, 40, 40, GRAY);

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

bool triggerShutter() {
	flashReady = flashStatus();
	shutterTriggered = false;
  if (shutterState == true) {
		unsigned long triggerTime = millis();

		// wait for flash to be ready
		while (flashReady == false) {
			flashReady = flashStatus();
			delay(50);
		}

		// trigger flash
    digitalWrite(SHUTTER_PIN, HIGH);

		// wait for flash to be triggered
		while (shutterTriggered == false) {
			delay(50);
			// if signal missed or shot never taken, break
			if (millis() - triggerTime > 10000) {
				break;
			}
			// check if flash triggered
			flashReady = flashStatus();
			// if signal received, exit and reset shutter
			if (flashReady == false) {
				shutterTriggered = true;
				delay(250); // pause for 250ms
			}
		}
		recycleTime = (millis() - triggerTime);

		// reset shutter signal
    digitalWrite(SHUTTER_PIN, LOW);
	}
	return shutterTriggered;
}

void setStepDistance() {

  // constrain multiplier range
  if (stepsPerMovement < 1) {
    stepsPerMovement = 1;
  } else if (stepsPerMovement > 80) {
    stepsPerMovement = 40;
  }

  // stepper is 200 steps/rev, linear rail has 1mm pitch, 1:1 gear ratio
  // 1 step = 1/200 = 0.005mm
	// microsteps = 4
	// 0.005 / 4 = 1.25μm
  distancePerMovement = microstepDistance * stepsPerMovement;

  if (prevDistance != distancePerMovement) {
    // print new delay value
    int16_t x1, y1;
    uint16_t w, h;
    tft.setFont(&Arimo_Bold_30);
    tft.getTextBounds(String(prevDistance/1000, 4), 10, 60, &x1, &y1, &w, &h);
    tft.fillRect(x1, y1, w, h, CUSTOM_BLUE);
    tft.setCursor(10, 60);
    tft.setTextColor(YELLOW, CUSTOM_BLUE);
    tft.println(String(distancePerMovement/1000, 4));

    // calculate number of steps required to cover range
    numMovements = ceil((endPosition - startPosition)*1.00/stepsPerMovement);

    if (activeScreen == 3) {
      // update estimated time
      estimateDuration(0); // don't force refresh
      // update progress
      updateProgress(0); // don't force refresh
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
    if (prevStartPosition != startPosition && activeScreen == 4) {
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
    goToStart = true;
  }

  if (setEnd == 1) {
    // lower limit
    if (endPosition < 0) {
      endPosition = 0;
    }
    // get new value
    endPosition = stepper.currentPosition();
    // print end point if changed
    if (prevEndPosition != endPosition && activeScreen == 4) {
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
  prevNumMovements = numMovements;
}

void displayPosition() {
  // update for new values
  if (prevStepperPosition != stepper.currentPosition()) {
    int16_t x1, y1;
    uint16_t w, h;

    if (activeScreen == 2) { // 2 = manual screen
      // update rail position value
      tft.setFont(&Arimo_Bold_30);
      tft.getTextBounds(String(prevStepperPosition*0.0025, 4), 9, 220, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, CUSTOM_BLUE);
      tft.setCursor(9, 220);
      tft.setTextColor(WHITE, CUSTOM_BLUE);
      tft.println(String(stepper.currentPosition()*0.0025, 4));

      // update movement count for manual screen
      manualMovementCount++;

      tft.setFont(&Arimo_Bold_30);
      tft.getTextBounds(String(prevManualMovementCount), 45, 140, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, CUSTOM_BLUE);
      tft.setTextColor(WHITE);
      tft.setCursor(45, 140);
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

void estimateDuration(bool screenRefresh) {
  float duration = numMovements * (shutterDelay + (recycleTime/1000));
  float elapsed = movementProgress * (shutterDelay + (recycleTime/1000));
  float remaining = duration - elapsed;
  int minutes = floor(remaining / 60);
  int seconds = int(remaining) % 60;
  char timeMinutesSeconds[5];

  int16_t x, y;
  uint16_t w, h;
  tft.setFont(&Arimo_Bold_30);
  tft.setTextColor(WHITE);
  sprintf_P(timeMinutesSeconds, PSTR("%02d:%02d"), minutes, seconds);

  if ((minutes != prevMinutes || seconds != prevSeconds || screenRefresh == 1) && activeScreen == 3) {
    tft.getTextBounds(String(prevTimeMinutesSeconds), 15, 140, &x, &y, &w, &h);
    tft.fillRect(x, y, w, h, CUSTOM_BLUE);
    tft.setCursor(15, 140);
    tft.println(timeMinutesSeconds);

    prevMinutes = minutes;
    prevSeconds = seconds;
    // assign new time to prev variable
    sprintf_P(prevTimeMinutesSeconds, PSTR("%02d:%02d"), minutes, seconds);
  }
}

void updateProgress(bool screenRefresh) {
  char progressMovements[10];

  int16_t x, y;
  uint16_t w, h;
  sprintf_P(progressMovements, PSTR("%02d/%02d"), movementProgress, numMovements);

  if ((movementProgress != prevMovementProgress) || numMovements != prevNumMovements || screenRefresh == 1) {
    tft.getTextBounds(String(prevProgressMovements), 20, 220, &x, &y, &w, &h);
    tft.fillRect(x, y, w, h, CUSTOM_BLUE);
    tft.setFont(&Arimo_Bold_24);
    tft.setTextColor(WHITE);
    tft.setCursor(20, 220);
    tft.println(progressMovements);

    prevNumMovements = numMovements;
    prevMovementProgress = movementProgress;
    // assign new time to prev variable
    sprintf_P(prevProgressMovements, PSTR("%02d:%02d"), movementProgress, numMovements);
  }
}

void toggleStepper(bool enable) {
	if (enable == 1) {
		delay(10); // give time for last step to complete
		stepper.enableOutputs();
		stepperDisabled = false;
		delay(10); // breathing space
	}

	if (enable == 0) {
		stepper.setSpeed(0);
		stepper.move(0);
		stepper.disableOutputs();
		stepperDisabled = true;
	}
}

void stallGuardConfig() {
  stallGuardConfigured = true;
  //set TMC2130 config
  driver.push(); // reset registers
  driver.toff(3);
  driver.tbl(1);
  driver.hysteresis_start(8);
  driver.hysteresis_end(1);
  driver.diag1_stall(1);
  driver.diag1_active_high(1);
  driver.coolstep_min_speed(0x0009F); // 20bit max
  driver.semin(3);
  driver.semax(2);
  driver.sg_min(1); // if sg_result < sg_min*32, current increase
  driver.sg_max(3); // if sg_result >= (sg_min+sg_max+1)*32, current decrease
  driver.sedn(0b01);
  driver.sg_stall_value(0);
  driver.stealthChop(0);
}

void silentStepConfig() {
  stallGuardConfigured = false;
  driver.push(); // reset registers
  driver.stealthChop(1);
  driver.stealth_autoscale(1);
  driver.stealth_gradient(0xF); // 1 to 15
  driver.interpolate(1);
  driver.push();
  driver.diag1_stall(0);
  driver.diag1_active_high(0);
  driver.coolstep_min_speed(0x0);
  driver.stealth_freq(0x0); // 0 or 1 for 16MHz
//    driver.chopper_mode(0);
//    driver.stealth_max_speed(0x0002F);
//    driver.double_edge_step(1);
//    driver.chopper_mode(1);
//    driver.sync_phases(1);
}

bool flashStatus() {
  int flashValue = 0;

  flashValue = analogRead(FLASH_PIN);
  Serial.println(flashValue);

  if (flashValue >= 280) {
    flashReady = true;
  }
  else {
    flashReady = false;
  }

  return flashReady;
}

void changeDirection() {
  if (directionFwd == true) {
    moveDist = -500000;
    directionFwd = false;
  }
  else if (directionFwd == false) {
    moveDist = 500000;
    directionFwd = true;
  }
  stepper.moveTo(moveDist);
}

void stallDetection() {
  Serial.print("DIAG: ");
  Serial.println(digitalRead(DIAG_PIN));
  if (bootFlag == true) {
    toggleStepper(0);
    stepper.setSpeed(0);
    if (directionFwd == false) {
      stepper.setCurrentPosition(0);
      bwdPosition = stepper.currentPosition();
    }
    if (directionFwd == true) {
      fwdPosition = stepper.currentPosition();
    }
    toggleStepper(1);
    changeDirection();
  }
}
