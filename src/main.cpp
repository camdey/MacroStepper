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
#include "UserInterface.h"
#include "StatusEnums.h"                    // enums containing statuses for various routines, namespace routines::
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
Joystick                xStick(stepper1, XSTICK_PIN, ZSTICK_PIN);
Joystick                rStick(stepper2, RSTICK_PIN, ZSTICK_PIN);
UserInterface           ui;


// ***** --- MAIN PROGRAM --- ***** //
void setup(void) {
    Serial.begin(250000);
	SPI.begin();
    tft.reset();

	uint16_t identifier = tft.readID();
	tft.begin(identifier);
	tft.setRotation(3);
    ui.screenRotated(false);

	stepper1.initDriver(850, NR_MICROSTEPS, 1, EN_1_PIN, CS_1_PIN); //1400
    stepper1.initDriver(850, NR_MICROSTEPS, 1, EN_2_PIN, CS_2_PIN);

    pinMode(PIEZO_PIN, OUTPUT);
	pinMode(EN_1_PIN, OUTPUT);
    pinMode(EN_2_PIN, OUTPUT);
    pinMode(CS_1_PIN, OUTPUT);
    pinMode(CS_2_PIN, OUTPUT);
    stepper1.slaveSelected(true);
    stepper1.configStealthChop();
    stepper1.enabled(true);

    stepper2.slaveSelected(false);
    stepper2.enabled(false);
    
    // declare analog pin as digital input
    pinMode(ZSTICK_PIN, INPUT_PULLUP);    // pullup needed for consistent readings
    pinMode(SHUTTER_PIN, OUTPUT);
    digitalWrite(SHUTTER_PIN, LOW);

    // find stable resting point of joystick
    // calibrateJoyStick();

    btn.begin(&tft);
    btn.setScreenSize(480, 320);
    ui.initButtons(200, 75);
    ui.populateScreen(routines::ui_Home);

    rStick.restVal(50);
    rStick.restValUpper(60);
    rStick.restValLower(40);
}

void loop() {
    // run AutoStack sequence if not paused or inactive
    // if (stack.busy()) {
    //     stack.run();
    //     // update duration if on Auto screen
    //     if (ui.activeScreen() == routines::ui_Auto) {
	// 	    auto_screen::estimateDuration();
    //     }
    // }
    // take touch reading
    if (millis() - ui.lastCheckMillis() >= 50) {
        ui.readTouchScreen(ui.activeScreen());
        ui.lastCheckMillis(millis());
        // if video360 active, keep updating target so stepper keeps moving
        // if (isVideo360Active()) {
        //     stepper2.video360(getVideo360Target());
        // }
        // if (photo360.status() != routines::inactive) {
        //     photo360.run();
        // }
    }
    // take joystick and limit switch reading, put stepper to sleep
    if (millis() - xStick.lastCheckMillis() >= 100) {
    //     // check joystick for movement if button depressed and not in autoStack or photo360/video360 mode
        if (!stack.busy() && !photo360.busy() && !isVideo360Active()) {
            int xPos = xStick.readSmoothed();
            int rPos = rStick.read();
            Serial.print("rpos: "); Serial.println(rPos);
            if ((xPos >= xStick.restValUpper() || xPos <= xStick.restValLower()) && xStick.buttonActive()) {
                xStick.motion(true);
            }
            // else if ((rPos >= rStick.restValUpper() || rPos <= rStick.restValLower()) && !rStick.buttonActive()) {
            //     rStick.motion(false);
            // }
        }
    //     // sleep if stepper inactive, update position on manual screen
    //     if (stepper1.reachedTarget() && stepper1.enabled() && stack.status() == routines::inactive) {
    //         stepper1.enabled(false); // disable stepper
    //     }
    //     // sleep if stepper inactive, update position on manual screen
    //     if (stepper2.reachedTarget() && stepper2.enabled() && photo360.status() == routines::inactive) {
    //         stepper2.enabled(false); // disable stepper
    //     }
	// 	// update flashSensorValue if on Flash screen
	// 	if (ui.activeScreen() == routines::ui_Flash && (ui.canEdit(routines::btn_flashOff) || ui.canEdit(routines::btn_flashOn))) {
	// 		flash_screen::updateFlashSensorValue();
	// 	}
        xStick.lastCheckMillis(millis());
    }
}