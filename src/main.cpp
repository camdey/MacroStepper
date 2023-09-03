// ********* Macro Stepper v2.2 *********
// *** AUTHOR: Cam Dey
// *** DATE: 2019-01-27
// *** UPDATED: 2023-08-26
// **************************************

// Macro Stepper was written to automate the focus stacking procedure commonly used
// in macrophotography. The manual mode allows the user to input the distance to travel
// per step (here called a "Movement") and whether to trigger the camera via shutter cable
// or not. The automatic mode ("AutoStack") will make a specified number of Movements
// based on the user inputted "Start" and "End" points, as well as the distance specified
// per Movement. A shutter delay can also be specified so that there is a pause before each Movement.
// This delay can be used to allow your camera flash time to recharge before the next shot.

// ----- DEFINITIONS ----- //
// STEP             = one microstep step of the stepper stepper motor
// MOVEMENT 		= one full movement of linear rail by specified distance, consisting of multiple steps
// PROCEDURE 		= one completed stack procedure, consisting of multiple movements
// DISTANCE 		= distance travelled per movement = number of steps per movement * 0.0025mm
// MANUAL MODE      = user input required to step motor by specified distance
// AUTOSTACK        = automatic mode, stepper completes procedure without further input

#include "Arduino.h"
#include "Adafruit_GFX.h"                   // core graphics library
#include "TouchScreen.h"                    // touchscreen library
#include "MCUFRIEND_kbv.h"                  // display driver for IL9481
#include "TMCStepper.h"
#include "TimerFreeTone.h"                  // produces beep tone for piezo
#include "gfxButton.h"                      // my library for adding/controlling TFT buttons
// project definitions and functions
#include "GlobalVariables.h"
#include "JoystickControl.h"                // joystick control functions
#include "MiscFunctions.h"                  // miscellaneous functions
#include "CameraControl.h"                 // functions relating to the camera shutter and flash
#include "StepperControl.h"                 // functions for controlling the stepper motor
#include "AutoStack.h"                      // AutoStack class and methods
#include "Photo360.h"
#include "VariableDeclarations.h"           // external variable declarations
#include "StatusEnums.h"                    // enums containing statuses for various routines, namespace routines::
#include "menu/UI-Main.h"
#include "menu/UI-Home.h"
#include "menu/UI-Stack.h"
#include "menu/UI-Orbis.h"
#include "menu/UI-Video360.h"
#include "menu/UI-Photo360.h"
#include "menu/UI-Photo360Config.h"
#include "menu/UI-Manual.h"
#include "menu/UI-Target.h"
#include "menu/UI-Flash.h"
#include "menu/UI-Auto.h"
#include "menu/UI-AutoConfig.h"
#include "Wire.h"


TouchScreen             ts = TouchScreen(XP, YP, XM, YM, 300);
TMC5160Stepper_Ext      stepper1(CS_1_PIN, R_1_SENSE);
TMC5160Stepper_Ext      stepper2(CS_2_PIN, R_2_SENSE);
MCUFRIEND_kbv           tft;
gfxButton               btn;
AutoStack               stack(stepper1);
Photo360                photo360(stepper2);
CameraControl           camera(SHUTTER_PIN, FLASH_SENSOR_PIN);


// --- currentTimes and elapsed times --- //
unsigned long prevButtonCheck       = 0;
unsigned long prevJoystickCheck     = 0;
// unsigned long recycleTime           = 0;                // duration to take photo
// --- Input and Output values --- //
int xStickUpper                     = 522;              // Upper boundary of joystick resting point, calibrated during setup
int xStickResting                   = 512;              // Resting point of joystick reading, calibrated during setup
int xStickLower                     = 502;              // Lower boundary of of joystick resting point, calibrated during setup
int xStickDiff                      = 0;                // Difference between ideal middle (512) and actual resting point
bool isJoystickBtnActive            = 0;                // check if joystick button is pressed (ZSTICK_PIN)




// ***** --- MAIN PROGRAM --- ***** //
void setup(void) {
    Serial.begin(250000);
	SPI.begin();
    tft.reset();

	uint16_t identifier = tft.readID();
	tft.begin(identifier);
	tft.setRotation(3);
    setScreenRotated(false);

	stepper1.initDriver(1400, NR_MICROSTEPS, 1, EN_1_PIN, CS_1_PIN);
    stepper1.initDriver(850, NR_MICROSTEPS, 1, EN_2_PIN, CS_2_PIN);

    stepper1.configStealthChop();
    stepper2.configStealthChop();

	pinMode(EN_1_PIN, OUTPUT);
    pinMode(EN_2_PIN, OUTPUT);
    stepper1.enabled(true);
    stepper2.enabled(false);
    pinMode(CS_1_PIN, OUTPUT);
    pinMode(CS_2_PIN, OUTPUT);
    stepper1.slaveSelected(true);
    stepper2.slaveSelected(false);
    pinMode(PIEZO_PIN, OUTPUT);
    
    // declare analog pin as digital input
    pinMode(ZSTICK_PIN, INPUT_PULLUP);    // pullup needed for consistent readings
    pinMode(SHUTTER_PIN, OUTPUT);
    digitalWrite(SHUTTER_PIN, LOW);

    // find stable resting point of joystick
    // calibrateJoyStick();

    btn.begin(&tft);
    btn.setScreenSize(480, 320);
    initButtons(200, 75);
    populateScreen("Home");
}

void loop() {
    // run AutoStack sequence if not paused or inactive
    if (stack.busy()) {
        stack.run();
        // update duration if on Auto screen
        if (getCurrentScreen() == "Auto") {
		    auto_screen::estimateDuration();
        }
    }
    // take touch reading
    if (millis() - prevButtonCheck >= 50) {
        readTouchScreen(getCurrentScreen());
        prevButtonCheck = millis();

        // if video360 active, keep updating target so stepper keeps moving
        if (isVideo360Active()) {
            stepper2.video360(getVideo360Target());
        }
        if (photo360.status() != routines::inactive) {
            photo360.run();
        }
    }
    // take joystick and limit switch reading, put stepper to sleep
    if (millis() - prevJoystickCheck >= 100) {
        // check joystick for movement
        int xStickPos = readJoystick();
        isJoystickBtnActive = !digitalRead(ZSTICK_PIN); // invert reading as 1 is not active and 0 is active
    
        // move if past threshold and not in autoStack or video360 mode
        if ((xStickPos >= xStickUpper || xStickPos <= xStickLower) && !stack.busy() && !photo360.busy() && !isVideo360Active() && isJoystickBtnActive) {
            joystickMotion(stepper1, xStickPos);
        }
        // sleep if stepper inactive, update position on manual screen
        if (stepper1.reachedTarget() && stepper1.enabled() && stack.status() == routines::inactive) {
            stepper1.enabled(false); // disable stepper
        }
        // sleep if stepper inactive, update position on manual screen
        if (stepper2.reachedTarget() && stepper2.enabled() && photo360.status() == routines::inactive) {
            stepper2.enabled(false); // disable stepper
        }
		// update flashSensorValue if on Flash screen
		if (getCurrentScreen() == "Flash" && (canEditFlashOffValue() || canEditFlashOnValue())) {
			flash_screen::updateFlashSensorValue();
		}
        prevJoystickCheck = millis();
    }
}
