#include "VariableDeclarations.h"
#include "StepperConfig.h"
#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "AutoStack.h"
#include "StepperControl.h"
#include "CameraControl.h"
#include "menu/UI-Main.h"
#include "menu/UI-Auto.h"
#include "menu/UI-AutoConfig.h"
#include "menu/UI-Global.h"
#include "menu/UI-Photo360.h"


void AutoStack::init() {
    if (status() == start) {                        // move to start position if beginning AutoStack
        _stepper.readyStealthChop();
        _stepper.targetVelocity(10000);                 // reduce max velocity to minimize vibration
        goToStart();
        completedMovements(0);
        _stepper.executedMovement(false);
        setShutterTriggered(false);
        lastMovementMillis(millis());
        global::func_Reset(false);                      // change reset button to red
        status(waitShutter);
        // auto_screen::status(start);
    }
}

/***********************************************************************
Runs an AutoStack procedure comprised of multiple Movements. The
function will first check whether the stage is in the Start position,
if not it will move to the Start. If the number of completed
Movements is less than the number of Movements required, it will
attempt to take a photo and move the stage one Movement. If no
Movement was taken due to delay, this function is called again and
another Movement attempted. When all Movements required are completed,
the autoStack will be completed and ceased to be called.
***********************************************************************/
void AutoStack::run() {
    init();

    if (completedMovements() <= requiredMovements() && !_stepper.executedMovement()) {
        // take photo if there's been >= STACK_DWELL_TIME since a movement was executed successfully (gives time for vibration to settle)
        // this has likely passed due to the need to wait for shutter to trigger
        if (isShutterEnabled() && !hasShutterTriggered() && millis() - lastMovementMillis() >= STACK_DWELL_TIME) {
            // if flashProcedure idle or is successful, start new procedure or trigger flash if !isFlashSensorEnabled
            if (status() == waitShutter) {
                triggerShutter();                       // take photo and trigger flash immediately or start flash procedure
            }
            else {
                runFlashProcedure(false);               // keep trying to fire flash
            }
            if (status() == debugFlash) {
                status(paused);                         // reset flash procedure
                auto_screen::displayPauseStack();       // display pause autostack interface
                populateScreen("Flash");                // go to flashScreen if can't trigger after 10 seconds
            }
        }
        // only move if autoStack hasn't been paused by flash failure and shutter has triggered or didn't need to trigger
        if (status() == newMovement && (!isShutterEnabled() || hasShutterTriggered())) {
            unsigned long delay = getShutterDelay();
            executeMovement(1, delay);
        }

        if (status() == executedMovement) {
            incrementCompletedMovements();
            status(waitShutter);
            if (getCurrentScreen() == "Auto") {         // make sure correct screen is displaying
                auto_screen::printAutoStackProgress();
                auto_screen::estimateDuration();
            }
            setShutterTriggered(false);                 // reset shutter triggered flag
            _stepper.executedMovement(false);           // reset executed movement flag
        }
    }
    if (completedMovements() >= requiredMovements()) {
        status(completedStack);
        terminateAutoStack();                           // stop AutoStack sequence if end reached
    }
}


/******************************************************************
Moves the stepper one Movement in a given direction. A Movement
is the total number of steps required to travel a given distance.
If there hasn't been a specified delay since the previous Movement,
this function returns false and the calling function should loop
until it receives a response of true. Prevents stall if rail has
been homed and will exit autoStack if running.
******************************************************************/
void AutoStack::executeMovement(int stepDirection, unsigned long stepperDelay) {
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
        status(executedMovement);    
        lastMovementMillis(millis());
    }
    else {
        _stepper.executedMovement(false);
        status(delayMovement);
  }
}


// move stepper to start for AutoStack sequence
// if current position in front of start, overshoot to remove backlash and return
// otherwise move forward to start
void AutoStack::goToStart() {
    int distanceToStart = (_stepper.XACTUAL() - startPosition());
    if (distanceToStart != 0) {
        // if current position is in front of start position
        if (distanceToStart > 0) {
            // move backwards to start postion
            // overshoot by 3200 steps/1mm and return to start to remove backlash
            overshootPosition(startPosition(), 3200);
        }
        // if current position is behind start position
        else if (distanceToStart < 0) {
            // move forward to start postion
            _stepper.XTARGET(startPosition());
            // wait for stepper to reach position
            while (_stepper.XACTUAL() != _stepper.XTARGET()) {}
        }
    }
}


void AutoStack::overshootPosition(long startPosition, int numberOfSteps) {
    int offsetPosition = startPosition - numberOfSteps;
    // overshoot start position
    _stepper.XTARGET(offsetPosition);
    // wait for stepper to reach position
    while (_stepper.XACTUAL() != _stepper.XTARGET()) {}

    delay(50);

    // move to start
    _stepper.XTARGET(startPosition);
    // wait for stepper to reach position
    while (_stepper.XACTUAL() != _stepper.XTARGET()) {}
}


// clean up variables etc after completing AutoStack sequence
void AutoStack::terminateAutoStack() {
    if (getCurrentScreen() != "Auto") {
        populateScreen("Auto");                             // go back to Auto screen if not already on it
    }
    status(inactive);
    auto_screen::displayResetStack();                       // update button and reset button bitmap
    auto_screen::estimateDuration();                        // update estimate
    global::btn_Reset.updateColour(BLACK);                  // change reset button back to black
    setShutterTriggered(false);
    completedMovements(0);                                  // reset completed movements count
    _stepper.executedMovement(false);
    produceTone(4, 300, 200);                               // sound 4 tones for 300ms separated by a 200ms delay
    // change max velocity back to normal
    _stepper.targetVelocity(STEALTH_CHOP_VMAX);
}


// run through the specified AutoStack procedure using the current start and end values
void AutoStack::dryRun() {
    _stepper.readyStealthChop();
    // reduce stepper velocity
    _stepper.targetVelocity(5000);
    // move to start position
    _stepper.XTARGET(startPosition());
    while (_stepper.XACTUAL() != _stepper.XTARGET()) {
        if (millis() - lastMovementMillis() >= 100) {
            autoconfig_screen::printPosition();             // update position
            lastMovementMillis(millis());
        }
    }
    // step slow through procedure
    _stepper.targetVelocity(500);
    // move to end position
    _stepper.XTARGET(endPosition());
    while (_stepper.XACTUAL() != _stepper.XTARGET()) {
        if (millis() - lastMovementMillis() >= 100) {
            autoconfig_screen::printPosition();             // update position
            lastMovementMillis(millis());
        }
    }

    _stepper.targetVelocity(5000);
    // return to start
    _stepper.XTARGET(startPosition());
    while (_stepper.XACTUAL() != _stepper.XTARGET()) {
        if (millis() - lastMovementMillis() >= 100) {
            autoconfig_screen::printPosition();             // update position
            lastMovementMillis(millis());
        }
    }
    // overshoot by 3200 steps/1mm and return to start
    overshootPosition(startPosition(), 3200);
    _stepper.targetVelocity(STEALTH_CHOP_VMAX);
}


