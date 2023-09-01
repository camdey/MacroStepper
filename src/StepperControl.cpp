#include "VariableDeclarations.h"
#include "StepperConfig.h"
#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "StepperControl.h"
#include "CameraControl.h"
#include "menu/UI-Main.h"
#include "menu/UI-Auto.h"
#include "menu/UI-AutoConfig.h"
#include "menu/UI-Global.h"
#include "menu/UI-Photo360.h"


// print various diagnostics to serial monitor
void debugStallGuard(TMC5160Stepper_Ext &stepper) {
    if (millis() - getLastMillis() >= 100) {
        Serial.print(" | event_stop_sg: "); Serial.print(stepper.event_stop_sg());
        Serial.print(" | sg_result: "); Serial.print(stepper.sg_result());
        Serial.print(" | TSTEP: "); Serial.print(stepper.TSTEP());
        Serial.print(" | CSACTUAL: "); Serial.print(stepper.cs_actual());
        Serial.print(" | XACTUAL: "); Serial.println(stepper.XACTUAL());
        setLastMillis(millis());
    }
}


// run stepper until stall triggered
bool detectEndStop(TMC5160Stepper_Ext &stepper) {
    while (stepper.event_stop_sg() == 0) {
        debugStallGuard(stepper); // print to serial monitor
    }
    return true;
}


/******************************************************************
Moves the stepper one Movement in a given direction. A Movement
is the total number of steps required to travel a given distance.
If there hasn't been a specified delay since the previous Movement,
this function returns false and the calling function should loop
until it receives a response of true. Prevents stall if rail has
been homed and will exit autoStack if running.
******************************************************************/
void executeMovement(TMC5160Stepper_Ext &stepper, int stepDirection, unsigned long stepperDelay) {
    readyStealthChop(stepper);

    // step after elapsed amount of time
    if ((millis() - getLastStepTime() > stepperDelay)) {
        auto_screen::stackStatus(stepTaken);
        int stepVelocity = stepDirection * stepper.stepsPerMovement();
        long targetPosition = stepper.XACTUAL() + stepVelocity;
        // set new target position
        stepper.XTARGET(targetPosition);
        Serial.print("Actual before: "); Serial.println(stepper.XACTUAL());
        Serial.print("Target before: "); Serial.println(stepper.XTARGET());
        // reduce speed
        stepper.targetVelocity(2000);
        // wait for stepper to reach target position
        while(stepper.XACTUAL() != stepper.XTARGET()) {
            Serial.print("Actual after: "); Serial.println(stepper.XACTUAL());
            Serial.print("Target after: "); Serial.println(stepper.XTARGET());
        }
        stepper.targetVelocity(STEALTH_CHOP_VMAX);
        stepper.executedMovement(true);    
        setLastStepTime(millis());
    }
    else {
    stepper.executedMovement(false);
        auto_screen::stackStatus(stepDelay);
  }
}


// execute a predetermined number of steps
void executeSteps(TMC5160Stepper_Ext &stepper, long nrSteps) {
    readyStealthChop(stepper);
    long targetPosition = stepper.XACTUAL() + nrSteps;
    // set new target position
    stepper.XTARGET(targetPosition);
    // wait for stepper to reach target position
    while(stepper.XACTUAL() != stepper.XTARGET()) {}
}


// find the forward and backward end stops of the linear rail using StallGuard
// moves stepper backwards until stall detected, records position
// moves stepper forwards until stall detected, records position and sets position as maximum value (384,000)
// moves stepper to middle of the rail (192,000)
void homeRail(TMC5160Stepper_Ext &stepper) {
    bool hitRearStop, hitForwardStop = false;
    readyStallGuard(stepper);
    // reset positions
    setForwardEndStop(1);
    setBackwardEndStop(1);

    // move to rear end stop + 100,000 just to ensure it over-runs without decelerating
  stepper.XTARGET(-MAX_RAIL_POSITION - 100000);
    hitRearStop = detectEndStop(stepper);               // wait until end stop detected
    // if endstop detected... 
    if (hitRearStop) {
        delay(100);
        stepper.sg_stop(false);
        stepper.sg_stop(true);


        stepper.XACTUAL(0);                             // set rear end stop position as 0
        setBackwardEndStop(stepper.XACTUAL());          // set backward position
    }
    // move to forward end stop
    stepper.XTARGET(MAX_RAIL_POSITION + 100000);
    hitForwardStop = detectEndStop(stepper);            // wait until end stop detected
    // if endstop detected...
    if (hitForwardStop) {
        delay(100);
        stepper.sg_stop(false);
        stepper.sg_stop(true);


        stepper.XACTUAL(MAX_RAIL_POSITION);             // set front end stop position as 0
        setForwardEndStop(stepper.XACTUAL());           // set forward position
    }

  // if back and forward position set, move to middle position
  if (getBackwardEndStop() == MIN_RAIL_POSITION && getForwardEndStop() == MAX_RAIL_POSITION) {
        stepper.XTARGET(192000);
        // wait to reach position before changing driver config after homing completed
        while (stepper.XACTUAL() != stepper.XTARGET()) {
            debugStallGuard(stepper); // print to serial monitor
        }

        setStartPosition(stepper.XACTUAL());            // update autoStack positions as rail has moved
        setEndPosition(stepper.XACTUAL());              // update autoStack positions as rail has moved
    configStealthChop(stepper);                         // set config for silentStep
        runHomingSequence = false;
        stepper.homed(true);
  }
}


void video360(TMC5160Stepper_Ext &stepper, long nrSteps) {
    // change to StealthChop if StallGuard is configured
    readyStealthChop(stepper);
    // update target velocity as calling configStealthChop resets this on the driver register but not the function
    stepper.targetVelocity(stepper.targetVelocity());
    // set position
    stepper.XTARGET(stepper.XACTUAL()+nrSteps);
}


void photo360(TMC5160Stepper_Ext &stepper) {
        stepper.targetVelocity(360); // approx. 5rpm
        // 0 - if start of new photo360, set speed, take first photo, and enable stepper
        if (getCurrentStage() == start && isNewPhoto360 && photo360Initiated) {
                // Serial.print("STARTED: "); Serial.println(millis());
                // reset target to be safe
                stepper.XTARGET(stepper.XACTUAL());
                // change to StealthChop if StallGuard is configured
                readyStealthChop(stepper);
                setCurrentStage(pullShutter);
                global::func_Reset(false); // change reset button to red
        }

        if (photo360Initiated && !photo360Paused) {
            // if not waiting for shutter and we are at the target, trigger flash
            // STEP 1: trigger photo
            if (getCurrentStage() == pullShutter && stepper.reachedTarget()) {
                // wait for delay, this should be skipped on the first photo
                if (millis() - getLastPhoto360Step() >= getPhoto360Delay()) {
                    // take photo
                    // Serial.print("PULL SHUTTER: "); Serial.println(millis());
                    digitalWrite(SHUTTER_PIN, HIGH);
                    lastFlashMillis(millis());
                    // set this at the start of the "loop" (pull, release, move)
                    setLastPhoto360Step();
                    setCurrentStage(releaseShutter);
                }
            }

            // if shutter hasn't triggered yet, keep retrying until 800ms has passed
            if (getCurrentStage() == releaseShutter) {
                if (millis() - lastFlashMillis() >= 800) {
                    // Serial.print("RELEASE SHUTTER: "); Serial.println(millis());
                    digitalWrite(SHUTTER_PIN, LOW);
                    // increment nr completed 360 photos
                    setNrCompleted360Photos(getNrCompleted360Photos()+1);
                    if (getCurrentScreen() == "Photo360") {
                        photo_screen::printPhoto360Progress();
                    }
                    setCurrentStage(newStep);
                }
            }

            // STEP 2: move to new target if time since last step has passed the delay period
            if (getCurrentStage() == newStep) {
                // wait for delay
                if (millis() - getLastPhoto360Step() >= getPhoto360Delay()/2) {
                    // Serial.print("STEP TO TARGET: "); Serial.println(millis());
                    // calculate number of microsteps to move for each photo, will truncate down to an int
                    int nr360Steps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) / getNr360Photos();
                    int remainder = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) % getNr360Photos();
                    // if there is a remainder, add a 1 step to the same number of photos as the size of the remainder
                    // e.g. if remainder is 80 steps, add 1 step to the last 80 photos in the photo360 procedure
                    if (getNrCompleted360Photos() > getNr360Photos()-remainder) {
                        nr360Steps++;
                    }
                    // if not clockwise, reverse direction of travel
                    if (!stepper.rotateClockwise()) {
                        nr360Steps = nr360Steps*-1;
                    }
                    stepper.XTARGET(stepper.XACTUAL() + nr360Steps);
                    setCurrentStage(stepTaken);
                }
            }

            // STEP 3: if arrived at target, take photo and loop back through steps
            if (getCurrentStage() == stepTaken && stepper.reachedTarget()) {
                // Serial.print("AT TARGET: "); Serial.println(millis());
                setCurrentStage(pullShutter);
            }

            // photo360 completed, previous `pullShutter` state will be overwritten by the reset to idle
            if (getNrCompleted360Photos() == getNr360Photos()) {
                // reset flags
                photo_screen::resetPhoto360();
                // Serial.print("COMPLETED: "); Serial.println(millis());
            }
        }
}