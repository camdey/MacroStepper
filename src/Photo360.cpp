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
    _stepper.slaveSelected(true);
    _stepper.enabled(true);
    _stepper.targetVelocity(360); // approx. 5rpm
    // 0 - if start of new photo360, set speed, take first photo, and enable stepper
    if (status() == routines::start) {
            // reset target to be safe
            _stepper.XTARGET(_stepper.XACTUAL());
            // change to StealthChop if StallGuard is configured
            _stepper.readyStealthChop();
            status(routines::newShutter);
            global::func_Reset(false); // change reset button to red
            // calculate number of microsteps to move for each photo, will truncate down to an int
            int nr360Steps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) / requiredPhotos();
             _stepper.stepsPerMovement(nr360Steps);
            // int remainder = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) % nrPhotos();
    }
    // continue if not inactive or paused
    if (photo360.busy()) {
        // STEP 1: trigger photo
        // take photo if there's been >= STACK_DWELL_TIME since a movement was executed successfully (gives time for vibration to settle)
        // this has likely passed due to the need to wait for shutter to trigger
        if (status() == routines::newShutter && _stepper.reachedTarget()) {
            // wait for delay, this should be skipped on the first photo
            if (millis() - lastMovementMillis() >= STACK_DWELL_TIME) {
                // take photo
                camera.triggerShutter(true);
                status(routines::waitShutter);
            }
        }

        // STEP 2: move to new target if time since last step has passed the delay period
        // only move if photo360 hasn't been paused by flash failure and shutter has triggered or didn't need to trigger
        if (status() == routines::newMovement && (!camera.shutterEnabled() || camera.photoTaken())) {
            executeMovement(1, shutterDelay());
        }

        if (status() == routines::executedMovement) {
            // increment nr completed 360 photos
            completedPhotos(completedPhotos()+1);
            if (ui.activeScreen() == routines::ui_Photo360) {
                photo_screen::printPhoto360Progress();
            }
        }

        // STEP 3: photo360 completed
        if (completedPhotos() == requiredPhotos()) {
            // reset flags
            status(routines::inactive);
            photo_screen::resetPhoto360();
        }
    }
    _stepper.slaveSelected(false);
    _stepper.enabled(false);
}


/******************************************************************
Moves the stepper one Movement in a given direction. A Movement
is the total number of steps required to travel a given distance.
If there hasn't been a specified delay since the previous Movement,
this function returns false and the calling function should loop
until it receives a response of true. Prevents stall if rail has
been homed and will exit autoStack if running.
******************************************************************/
void Photo360::executeMovement(int stepDirection, unsigned long stepperDelay) {
    _stepper.readyStealthChop();
    // step after elapsed amount of time
    if ((millis() - lastMovementMillis() > stepperDelay)) {
        int stepVelocity = stepDirection * _stepper.stepsPerMovement();
        long targetPosition = _stepper.XACTUAL() + stepVelocity;
        // set new target position
        _stepper.XTARGET(targetPosition);
        // reduce speed
        _stepper.targetVelocity(2000);
        // wait for stepper to reach target position
        while(_stepper.XACTUAL() != _stepper.XTARGET()) {}
        _stepper.targetVelocity(STEALTH_CHOP_VMAX);
        _stepper.executedMovement(true);
        status(routines::executedMovement);    
        lastMovementMillis(millis());
    }
    else {
        _stepper.executedMovement(false);
        status(routines::delayMovement);
    }
}