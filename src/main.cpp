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
// #include "AccelStepper.h"											// software stepping implementation
// #include "TMC2130Stepper.h" 									// stepper driver library
// #include "TMC2130Stepper_REGDEFS.h"	  				// stepper driver registry definitions
#include "TMCStepper.h"
// #include "TimerFreeTone.h"                    // produces beep tone for piezo
#include "gfxButton.h"                        // my library for adding/controlling TFT buttons
// project definitions and functions
#include "DriverConfig.h"											// functions for configuring TMC2130 profiles
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


TMC5160Stepper                driver(CS_PIN, R_SENSE);
TouchScreen 		ts 					= TouchScreen(XP, YP, XM, YM, 300);
MCUFRIEND_kbv 	tft;
gfxButton       gfxB;
gfxTouch        gfxT;


// --- currentTimes and elapsed times --- //
unsigned long currentTime 				= 0;        	// current time in millis()
unsigned long prevButtonCheck 		= 0;    			// time subroutine1 last ran
unsigned long prevJoystickCheck 		= 0;    			// time subroutine2 last ran
unsigned long prevStepTime 				= 0;       		// previous step time reading
unsigned long recycleTime 				= 1000;    		// duration to take photo
unsigned long prevGenericTime 		= 0;    			// generic timer for toggles and such
unsigned long genericTouchDelay 	= 200; 				// 200ms touch delay for toggles
unsigned long lastReadFlash				= 0;					// previous time at which the light sensor was polled
int prevMinutes 									= 1;        	// duration of autoStack
int prevSeconds 									= 1;        	// duration of autoStack
char prevTimeMinutesSeconds[6] 		= "00:00"; 		// previous duration time, global to prevent overwrite on loop

// --- Screen flags --- //
int activeScreen 									= 1;        	// currently displayed screen
bool arrowsActive 								= false;      // take arrow touch readings
bool editShutterDelay 						= false;     	// set shutter delay time
bool editStartPosition 						= false;     	// set start point for auto mode
bool editEndPosition 							= false;      // set end point for auto mode
bool editMovementDistance 				= false;  		// set step distance in any mode
bool editFlashOnValue							= false;			// set flash on value
bool editFlashOffValue						= false;			// set flash off value
bool testFlash                    = false;      // flag for testing flash threshold
bool screenRotated                 = false;      // false means usb in lower left corner
// --- Input and Output values --- //
int xStickPos 										= 0;        	// ADC value of x-axis joystick
int xPosUpper                     = 522; 				// Upper boundary of joystick resting point, calibrated during setup
int xPosResting                   = 512;				// Resting point of joystick reading, calibrated during setup
int xPosLower                     = 502;				// Lower boundary of of joystick resting point, calibrated during setup
int xPosDiff                      = 0;          // Difference between ideal middle (512) and actual resting point
int zStickVal 										= 1;        	// increment count of z-axis button press
int prevZStickVal 								= 1;        	// only increment per button press
int shutterDelay 									= 1;        	// delay between step and shutter trigger
int prevDelay 										= 1;        	// previous delay value
int joyStickSpeed 								= 0;					// joystick speed value
int flashValue										= 10;					// reading from light sensor on flash LED
int prevFlashValue								= 0;					// previous reading of flash light sensor
int flashThreshold                = 280;        // threshold value for flash being ready to fire
int prevFlashThreshold            = 0;          // previous threshold value for flash being ready
int flashOnValue                  = 300;        // initial value for flash considered as being ready
int flashOffValue                 = 30;         // initial value for flash considered as recycling
// --- Enable/Disable functionality --- //
bool runHomingSequence 										= true;       // runs rehoming sequence
bool homedRail										= false;			// true if homeRail() run successfully
bool goToStart 										= true;       // move to start for autoStack procedure
bool joystickState 								= true;       // enabled/disabled
bool autoStackFlag 								= false;      // enables function for stack procedure
bool pauseAutoStack 							= false;      // pause stack procedure
bool shutterEnabled 							= false;      // disabled/enabled
bool targetFlag 									= false;      // resets stepper target
bool flashReady 									= false;			// flash ready for next photo
bool stallGuardConfigured 				= true;				// stallGuard config has run
bool autoStackMax                 = false;      // set endPosition to max for indetermine autoStack procedure
// --- Position values --- //
long startPosition 								= 0;        	// start position for stack procedure
long prevStartPosition 						= 0;
long endPosition 									= 0;        	// end position for stack procedure
long prevEndPosition 							= 0;
long prevStepperPosition 					= 1;    			// used for showing position of stepper if changed
int manualMovementCount 					= 0;    			// count of manual movements
int prevManualMovementCount 			= 0;
volatile long moveDist 						= 60000; 			// distance for homing to ensure it reaches end stop
volatile bool stepperDisabled 		= false;      // is stepper disabled? in ISR
volatile bool directionFwd 				= true;       // is stepper in fwd direction? in ISR
volatile long fwdPosition 				= 1;          // fwdPosition of rail, set to non-zero number initially, in ISR
volatile long bwdPosition 				= 1;          // bwdPosition of rail, set to non-zero number initially, in ISR
// --- Stepper motor variables --- //
int stepsPerMovement 							= 1;       		// number of steps required for a given distance
int movementsRequired 						= 0;        	// equals total distance / step multiplier
int prevMovementsRequired 				= 1;       		// previous movementsRequired value
int stepCount 										= 0;        	// number of steps taken in a given movement
double distancePerMovement 				= 1.25; 			// distance in micrometres travelled per movement
double prevDistance 							= 0;     			// previous step distance in micrometres
int completedMovements 						= 0;       		// number of completed movements
int prevCompletedMovements 				= 1;   				// used for overwriting prev movement progress
char prevAutoStackProgress[10]  	= "0/0";			// prev progress value, global to prevent overwriting each loop
bool stepperMoved 								= false; 			// did stepMotor actually step or not
bool shutterTriggered 						= false;			// did the shutter trigger or not
bool triggerFailed                = false;      // record state if shutter trigger has failed
int stepperMaxSpeed               = 3000;       // max speed setting for AccelStepper
int rampSteps                     = 50000;      // number of steps in ramp profile for joystick control

String currentScreen;
String stepDist = "0.00125";
int joystickMaxVelocity = 100000;

// ***** --- PROGRAM --- ***** //

// TODO
// - add reduced speed option for joystick (z button press?)
// - remove serialTuple and functions for sending new speed/accel settings
// - fix bug with rampSteps where hovering at low speed will use up all rampSteps and
//    moving to a much higher speed will cause the motor to stall


void setup(void) {
  Serial.begin(250000);
	SPI.begin();
  tft.reset();

	uint16_t identifier = tft.readID();
	if (identifier == 0x9341) {
		Serial.println(F("Found ILI9341 LCD driver"));
	} else {
		Serial.print(F("No driver found"));
	}

	tft.begin(identifier);
	tft.setRotation(1);
  screenRotated = false;

	// driver.begin();
	// driver.rms_current(900);
	// driver.microsteps(nrMicrosteps);
  // driver.shaft(1); // inverse shaft, large target moves away from rear, small target moves towards rear
  stepperDisabled = false;
  // driverConfig("joystick");

	pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH); // unselect SPI slave
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(DIR_PIN, LOW);
  pinMode(DIAG1_PIN, INPUT);
  pinMode(PIEZO_PIN, OUTPUT);

  // TimerFreeTone(PIEZO_PIN, 4000, 500, 10);

  // declare analog pin as digital input
  pinMode(ZSTICK_PIN, INPUT_PULLUP);
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
	currentTime = millis();

  // run AutoStack sequence if enabled
  // if (autoStackFlag == true && pauseAutoStack == false) {
  //   autoStack();
  // }
  // // take touch reading
  if (millis() - prevButtonCheck >= 50) {
    checkButtons(getCurrentScreen());
    prevButtonCheck = millis();
  }

  // take joystick and limit switch reading, put stepper to sleep
  // if (millis() - prevJoystickCheck >= 250) {
  //   // Serial.print(millis()); Serial.print(" | free ram: "); Serial.println(FreeRam());
  //   // check joystick for movement
  //   readJoystick();
  //
  //   // move if past threshold and not in autoStack mode
  //   if ((xStickPos >= xPosUpper || xStickPos <= xPosLower) && autoStackFlag == false) {
  //     joystickControl();
  //   }
  //   // sleep if stepper inactive, update position on manual screen
  //   if (stepper.distanceToGo() == 0 && (autoStackFlag == false || pauseAutoStack == true) && digitalRead(EN_PIN) == LOW) {
  //     toggleStepper(false); // disable stepper
  //     // refresh position on manual screen after stepping completed
  //     if (prevStepperPosition != driver.XACTUAL() && getCurrentScreen() == "Manual") {
  //       manual_screen::displayPosition();
  //     }
  //   }
	// 	// configure SilentStep if not homing rail
	// 	if (stallGuardConfigured == true && runHomingSequence == false) {
	// 		driverConfig("joystick");
  //   }
	// 	// update flashValue if on right screen
	// 	if (getCurrentScreen() == "Flash" && (editFlashOffValue == true || editFlashOnValue == true)) {
	// 		flash_screen::updateFlashValue();
	// 	}
  //   // set END as maxPosition if Z Stick depressed
  //   if (getCurrentScreen() == "AutoConfig" && editEndPosition == true && digitalRead(ZSTICK_PIN) == LOW) {
  //     autoStackMax = true;
  //     config_screen::setAutoStackPositions(false, true);
  //     autoStackMax = false;
  //   }
  //
  //   prevJoystickCheck = millis();
  // }

  // reset target to currentPosition
  // if (targetFlag == true) {
  //   stepper.move(0);
  //   stepper.setSpeed(0);
  //   targetFlag = false;
  // }
  // run homing sequence if first loop
  // if (runHomingSequence == true) {
  //   homeRail(); // run homing routine
	// 	config_screen::setAutoStackPositions(true, true); // set both start and end points
	// 	driverConfig("joystick"); // set config for silentStep
  //   runHomingSequence = false;
  // }
}
