// ********* Macro Stepper v2.1 *********
// *** AUTHOR: Cam Dey
// *** DATE: 2019-01-27
// *** UPDATED: 2020-04-10
// **************************************

// Macro Stepper was written to automate the focus stacking procedure commonly used
// in macrophotography. The manual mode allows the user to input the distance to travel
// per step (here called a "Movement") and whether to trigger the camera via shutter cable
// or not. The automatic mode ("AutoStack") will make a specified number of Movements
// based on the user inputted "Start" and "End" points, as well as the distance specified
// per Movement. A shutter delay can also be specified so that there is a pause before each Movement.
// This delay can be used to allow your camera flash time to recharge before the next shot.

// ----- DEFINITIONS ----- //
// STEP 				= one microstep step of the stepper stepper motor
// MOVEMENT 		= one full movement of linear rail by specified distance, consisting of multiple steps
// PROCEDURE 		= one completed stack procedure, consisting of multiple movements
// DISTANCE 		= distance travelled per movement = number of steps per movement * 0.0025mm
// MANUAL MODE 	= user input required to step motor by specified distance
// AUTOSTACK 		= automatic mode, stepper completes procedure without further input

#include "Arduino.h"
#include "Adafruit_GFX.h"    									// core graphics library
#include "TouchScreen.h"											// touchscreen library
#include "MCUFRIEND_kbv.h"										// display driver for IL9481
#include "TMCStepper.h"
#include "TimerFreeTone.h"                    // produces beep tone for piezo
#include "gfxButton.h"                        // my library for adding/controlling TFT buttons
// project definitions and functions
#include "DriverConfig.h"											// functions for configuring TMC2130 profiles
#include "GlobalVariables.h"
#include "JoystickControl.h"                  // joystick control functions
#include "MiscFunctions.h"										// miscellaneous functions
#include "ShutterControl.h"										// functions relating to the camera shutter and flash
#include "StepperControl.h"										// functions for controlling the stepper motor
#include "VariableDeclarations.h"							// external variable declarations
#include "UI-Main.h"
#include "UI-Home.h"
#include "UI-Manual.h"
#include "UI-Flash.h"
#include "UI-Auto.h"
#include "UI-AutoConfig.h"
#include "Wire.h"


TouchScreen 		ts = TouchScreen(XP, YP, XM, YM, 300);
TMC5160Stepper  driver(CS_PIN, R_SENSE);
MCUFRIEND_kbv 	tft;
gfxButton       gfxB;
gfxTouch        gfxT;


// --- currentTimes and elapsed times --- //
unsigned long prevButtonCheck 		= 0;    			// time subroutine1 last ran
unsigned long prevJoystickCheck 	= 0;    			// time subroutine2 last ran
unsigned long recycleTime 				= 1000;    		// duration to take photo
unsigned long lastReadFlash				= 0;					// previous time at which the light sensor was polled
// --- Input and Output values --- //
int xStickUpper                   = 522; 				// Upper boundary of joystick resting point, calibrated during setup
int xStickResting                 = 512;				// Resting point of joystick reading, calibrated during setup
int xStickLower                   = 502;				// Lower boundary of of joystick resting point, calibrated during setup
int xStickDiff                    = 0;          // Difference between ideal middle (512) and actual resting point
bool isJoystickBtnActive          = 0;          // check if joystick button is pressed (ZSTICK_PIN)
int shutterDelay 									= 1;        	// delay between step and shutter trigger
int prevDelay 										= 1;        	// previous delay value
int flashValue										= 10;					// reading from light sensor on flash LED
int flashThreshold                = 280;        // threshold value for flash being ready to fire
int flashOnValue                  = 300;        // initial value for flash considered as being ready
int flashOffValue                 = 30;         // initial value for flash considered as recycling
// --- Enable/Disable functionality --- //
bool runHomingSequence 						= true;       // runs rehoming sequence
bool goToStart 										= true;       // move to start for autoStack procedure
bool joystickState 								= true;       // enabled/disabled
bool autoStackRunning 						= false;      // enables function for stack procedure
bool autoStackPaused 							= false;      // pause stack procedure
bool flashReady 									= false;			// flash ready for next photo
bool stallGuardConfigured 				= true;				// stallGuard config has run
bool autoStackMax                 = false;      // set getEndPosition() to max for indetermine autoStack procedure 
// --- Stepper motor variables --- //
bool shutterTriggered 						= false;			// did the shutter trigger or not
bool triggerFailed                = false;      // record state if shutter trigger has failed  

// ***** --- PROGRAM --- ***** //


void setup(void) {
  Serial.begin(250000);
	SPI.begin();
  tft.reset();

	uint16_t identifier = tft.readID();
	tft.begin(identifier);
	tft.setRotation(1);
  setScreenRotated(false);

	driver.begin();
	driver.rms_current(1400); // max 1900rms, stepper rated for 2.8A
	driver.microsteps(nrMicrosteps);
  driver.shaft(1); // inverse shaft, large target moves away from rear, small target moves towards rear

  setStepperEnabled(true);
  configStealthChop();

	pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH); // unselect SPI slave
  pinMode(PIEZO_PIN, OUTPUT);
  
  // declare analog pin as digital input
  pinMode(ZSTICK_PIN, INPUT_PULLUP);  // pullup needed for consistent readings
  pinMode(SONY_PIN, OUTPUT);
  digitalWrite(SONY_PIN, LOW);

  // find stable resting point of joystick
  // calibrateJoyStick();

	// if holding down ZSTICK_PIN, don't home rail
	// if (digitalRead(ZSTICK_PIN) == LOW) {
	// 	runHomingSequence = false;
	// }
  // don't home rail on start up
  runHomingSequence = false;

  initButtons(200, 75);
  populateScreen("Home");

}

void loop() {
  // run AutoStack sequence if enabled
  if (autoStackRunning && !autoStackPaused) {
    autoStack();
  }
  // take touch reading
  if (millis() - prevButtonCheck >= 50) {
    checkButtons(getCurrentScreen());
    prevButtonCheck = millis();
  }
  // take joystick and limit switch reading, put stepper to sleep
  if (millis() - prevJoystickCheck >= 100) {
    // check joystick for movement
    int xStickPos = readJoystick();
    isJoystickBtnActive = !digitalRead(ZSTICK_PIN); // invert reading as 1 is not active and 0 is active
  
    // move if past threshold and not in autoStack mode
    if ((xStickPos >= xStickUpper || xStickPos <= xStickLower) && !autoStackRunning && isJoystickBtnActive) {
      joystickMotion(xStickPos);
    }
    // sleep if stepper inactive, update position on manual screen
    if (hasReachedTargetPosition() && (!autoStackRunning || autoStackPaused) && isStepperEnabled()) {
      setStepperEnabled(false); // disable stepper
    }
		// update flashValue if on Flash screen
		if (getCurrentScreen() == "Flash" && (canEditFlashOffValue() || canEditFlashOnValue())) {
			flash_screen::updateFlashValue();
		}
  //   // set END as maxRailPosition if Z Stick depressed
  //   if (getCurrentScreen() == "AutoConfig" && canEditEndPosition() && digitalRead(ZSTICK_PIN) == LOW) {
  //     autoStackMax = true;
  //     config_screen::updateEndPosition();
  //     autoStackMax = false;
  //   }
    Serial.print("ACTUAL: "); Serial.print(driver.XACTUAL());
    Serial.print(" | TARGET: "); Serial.println(driver.XTARGET());
    prevJoystickCheck = millis();
  }

  // run homing sequence if first loop
  if (runHomingSequence) {
    homeRail(); // run homing routine
  }
}
