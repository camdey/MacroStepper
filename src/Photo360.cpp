#include "Photo360.h"
#include "VariableDeclarations.h"
#include "AutoStack.h"
#include "StepperControl.h"
#include "StatusEnums.h"
#include "UserInterface.h"
#include "CameraControl.h"
#include "menu/UI-Global.h"
#include "menu/UI-Photo360.h"

void Photo360::run() {
    // make sure to disable/enable pins 
    if (status() == routines::paused) {
        // disable spi pin while paused
        if (_stepper.slaveSelected()) {
            _stepper.slaveSelected(false);
        }
    } else {
        if (!_stepper.slaveSelected() || !_stepper.enabled()) {
            _stepper.slaveSelected(true);
            _stepper.enabled(true);
        }
    }
    // 0 - if start of new photo360, set speed, take first photo, and enable stepper
    if (status() == routines::start) {
            // reset target to be safe
            _stepper.XTARGET(_stepper.XACTUAL());
            // change to StealthChop if StallGuard is configured
            _stepper.readyStealthChop();
            if (camera.shutterEnabled()) {
                status(routines::newShutter);
            } else {
                status(routines::newMovement);
            }
            global::func_Reset(false); // change reset button to red
            // calculate number of microsteps to move for each photo, will truncate down to an int
            int nr360Steps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) / requiredPhotos();
             _stepper.stepsPerMovement(nr360Steps);
            // int remainder = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) % nrPhotos();
    }
    // continue if not inactive or paused
    if (photo360.busy() && photo360.status() != routines::paused) {
        // STEP 1: trigger photo
        Serial.print("Step 1: "); Serial.println(status());
        // take photo if there's been >= STACK_DWELL_TIME since a movement was executed successfully (gives time for vibration to settle)
        // this has likely passed due to the need to wait for shutter to trigger
        if (status() == routines::newShutter && _stepper.reachedTarget()) {
            Serial.println("Step 1b");
            // wait for delay, this should be skipped on the first photo
            if (millis() - lastMovementMillis() >= STACK_DWELL_TIME) {
                Serial.println("Step 1c");
                // take photo
                camera.triggerShutter(true);
                status(routines::newMovement);
            }
        }

        // STEP 2: move to new target if time since last step has passed the delay period
        // only move if photo360 hasn't been paused by flash failure and shutter has triggered or didn't need to trigger
        if (status() == routines::newMovement && (!camera.shutterEnabled() || camera.photoTaken())) {
            Serial.println("Step 2");
            executeMovement(getDirection(), shutterDelay(), _stepper.stepsPerMovement());
            camera.photoTaken(false); // reset state
        } else if (status() == routines::delayMovement && (!camera.shutterEnabled() || camera.photoTaken())) {
            executeMovement(getDirection(), shutterDelay(), _stepper.stepsPerMovement());
            Serial.println("Step 2b");
        }

        if (status() == routines::executedMovement) {
            Serial.println("Step 3");
            // increment nr completed 360 photos
            completedPhotos(completedPhotos()+1);
            if (camera.shutterEnabled()) {
                status(routines::newShutter);
            } else {
                status(routines::newMovement);
            }
            if (ui.activeScreen() == routines::ui_Photo360) {
                Serial.println("Step 3b");
                photo_screen::printPhoto360Progress();
            }
        }

        // STEP 3: photo360 completed
        if (completedPhotos() == requiredPhotos()) {
            Serial.println("Step 4");
            // reset flags
            terminatePhoto360();
            status(routines::inactive);
            photo_screen::resetPhoto360();
            _stepper.slaveSelected(false);
            _stepper.enabled(false);
        }
    }
}


// clean up variables etc after completing AutoStack sequence
void Photo360::terminatePhoto360() {
    if (ui.activeScreen() != routines::ui_Photo360) {
        ui.populateScreen(routines::ui_Photo360);           // go back to Auto screen if not already on it
    }
    status(routines::inactive);
    camera.photoTaken(false);
    completedPhotos(0);                                     // reset completed movements count
    _stepper.executedMovement(false);
    produceTone(4, 300, 200);                               // sound 4 tones for 300ms separated by a 200ms delay
    // change max velocity back to normal
    _stepper.targetVelocity(STEALTH_CHOP_VMAX);
}


/******************************************************************
Moves the stepper one Movement in a given direction. A Movement
is the total number of steps required to travel a given distance.
If there hasn't been a specified delay since the previous Movement,
this function returns false and the calling function should loop
until it receives a response of true. Prevents stall if rail has
been homed and will exit autoStack if running.
******************************************************************/
void Photo360::executeMovement(int stepDirection, unsigned long stepperDelay, int stepsPerMovement) {
    _stepper.readyStealthChop();
    // step after elapsed amount of time
    if ((millis() - lastMovementMillis() > stepperDelay)) {
        int stepVelocity = stepDirection * stepsPerMovement;
        long targetPosition = _stepper.XACTUAL() + stepVelocity;
        // set new target position
        _stepper.XTARGET(targetPosition);
        // reduce speed
        _stepper.targetVelocity(2000);
        // wait for stepper to reach target position
        while(_stepper.XACTUAL() != _stepper.XTARGET()) {}
        _stepper.targetVelocity(STEALTH_CHOP_VMAX);
        _stepper.executedMovement(true);
        if (photo360.busy()) {
            status(routines::executedMovement);
        }
        lastMovementMillis(millis());
    }
    else {
        _stepper.executedMovement(false);
        if (photo360.busy()) {
            status(routines::delayMovement);
        }
    }
}
