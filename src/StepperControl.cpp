#include <Arduino.h>
#include "VariableDeclarations.h"
#include "StepperControl.h"
#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "StepperControl.h"
#include "AutoStack.h"
#include "CameraControl.h"
#include "menu/UI-Main.h"
#include "menu/UI-Auto.h"
#include "menu/UI-AutoConfig.h"
#include "menu/UI-Global.h"
#include "menu/UI-Photo360.h"

TMC5160Stepper_Ext::TMC5160Stepper_Ext(uint16_t pinCS, float RS, int8_t link) : TMC5160Stepper(pinCS, RS, link) {};

void TMC5160Stepper_Ext::initDriver(uint16_t rmsCurrent, uint16_t nrMicrosteps, bool shaftDirection, int pinEN, int pinCS) {
    begin();
    // 23HM22-2804S: max 1900rms, stepper rated for 2.8A
    // SY42STH47-1206A: max 850rms, stepper rated for 1.2A
    rms_current(rmsCurrent); 
    microsteps(nrMicrosteps);
    shaft(shaftDirection); // inverse shaft, large target moves away from rear, small target moves towards rear

    enablePin(pinEN);
    chipSelectPin(pinCS);
}


void TMC5160Stepper_Ext::configStealthChop() {
    GCONF();
    en_pwm_mode(true);
    en_softstop(true);
    PWMCONF();
    pwm_autoscale(true);                            // Needed for stealthChop
    pwm_autograd(true);
    TZEROWAIT(5000);                                // wait time before next movement, approx 300ms, range 0 - 65,536
    TPOWERDOWN(10);
    TPWMTHRS(0);                                    // TSTEP ≥ TPWMTHRS: StealthChop enabled, DcStep disabled
    TCOOLTHRS(0);                                   // TCOOLTHRS ≥ TSTEP ≥ THIGH: CoolStep enabled, StealthChop PWM disabled.
                                                    // TCOOLTHRS ≥ TSTEP: Stop on stall enabled.
    THIGH(100);                                     // TSTEP ≤ THIGH: CoolStep disabled, StealthChop PWM mode disabled. Stall Detection uses DcStep
    VDCMIN(0);                                      // |VACT| ≥ VDCMIN ≥ 256: same as if THIGH exceeded, DcStep auto commutation
    CHOPCONF();
    toff(5);                                        // General enable for motorm driver, 0 = off, >= 1 driver enabled. Value doesn't affect stealthChop
    hstrt(0);
    hend(0);
    tbl(2);                                         // comparator blank time
    chm(0);                                         // 0 = spreadCycle chopper mode (1 = const. off time)
    vhighchm(false);
    vhighfs(false);
    IHOLD_IRUN();
    ihold(5);
    irun(16);
    iholddelay(8);
    RAMPMODE(0);                                    // Positioning mode (using all A, D and V parameters)
    a1(5000);                                       // First acceleration between VSTART and V1
    AMAX(3500);                                     // Second acceleration between V1 and VMAX
    VMAX(STEALTH_CHOP_VMAX);                        // Motion ramp target velocity (VMAX >= VSTART). Can be changed during a motion.
    v1(30000);                                      // First acceleration / deceleration phase treshold velocity
    DMAX(3500);                                     // Deceleration between VMAX and V1
    d1(5000);                                       // Deceleration between V1 and VSTOP
    VSTART(0);                                      // Motor start velocity (VSTOP >= VSTART)
    VSTOP(10);                                      // Motor stop velocity

    // STALLGUARD2 PARAMETERS
    sg_stop(false);                                 // stop by stallguard, disable to release motor after stall event

    stallGuardActive(false);
}


void TMC5160Stepper_Ext::configStallGuard() {
    GCONF();
    en_pwm_mode(true);
    en_softstop(false);
    PWMCONF();
    pwm_autoscale(true);                            // Needed for stealthChop
    pwm_autograd(true);
    TZEROWAIT(10000);                               // wait time before next movement, approx 300ms, range 0 - 65,536
    TPOWERDOWN(10000);
    TPWMTHRS(0);                                    // TSTEP ≥ TPWMTHRS: StealthChop enabled, DcStep disabled
    TCOOLTHRS(400);                                 // TCOOLTHRS ≥ TSTEP ≥ THIGH: CoolStep enabled, StealthChop PWM disabled.
                                                    // TCOOLTHRS ≥ TSTEP: Stop on stall enabled.
    THIGH(0);                                       // TSTEP ≤ THIGH: CoolStep disabled, StealthChop PWM mode disabled. Stall Detection uses DcStep
    VDCMIN(0);                                      // |VACT| ≥ VDCMIN ≥ 256: same as if THIGH exceeded, DcStep auto commutation
    CHOPCONF();
    toff(5);                                        // General enable for motorm driver, 0 = off, >= 1 driver enabled. Value doesn't affect stealthChop
    hstrt(0);
    hend(0);
    tbl(2);                                         // comparator blank time
    chm(0);                                         // 0 = spreadCycle chopper mode (1 = const. off time)
    vhighchm(false);
    vhighfs(false);
    IHOLD_IRUN();
    ihold(5);
    irun(16);
    iholddelay(8);
    RAMPMODE(0);                                    // Positioning mode (using all A, D and V parameters)
    a1(1000);                                       // First acceleration between VSTART and V1
    AMAX(800);                                      // Second acceleration between V1 and VMAX
    VMAX(50000);                                    // Motion ramp target velocity (VMAX >= VSTART). Can be changed during a motion.
    v1(30000);                                      // First acceleration / deceleration phase treshold velocity
    DMAX(1200);                                     // Deceleration between VMAX and V1
    d1(1000);                                       // Deceleration between V1 and VSTOP
    VSTART(2);                                      // Motor start velocity (VSTOP >= VSTART)
    VSTOP(12);                                      // Motor stop velocity

    // STALLGUARD2 PARAMETERS
    sg_stop(true);                                  // stop by stallguard, disable to release motor after stall event
    sgt(0);                                         // stallguard sensitivity
    semin(2);                                       // If the StallGuard2 result falls below SEMIN*32, the motor current is increased
    semax(1);                                       // If the StallGuard2 result is equal to or above (SEMIN+SEMAX+1)*32, the motor current becomes decreased to save energy.
    sedn(0b00);                                     // For each 8 StallGuard2 values decrease current down step speed by one

    stallGuardActive(true);
}


// print various diagnostics to serial monitor
void TMC5160Stepper_Ext::debugStallGuard() {
    if (millis() - lastCheckMillis() >= 100) {
        Serial.print(" | event_stop_sg: "); Serial.print(event_stop_sg());
        Serial.print(" | sg_result: "); Serial.print(sg_result());
        Serial.print(" | TSTEP: "); Serial.print(TSTEP());
        Serial.print(" | CSACTUAL: "); Serial.print(cs_actual());
        Serial.print(" | XACTUAL: "); Serial.println(XACTUAL());
        lastCheckMillis(millis());
    }
}


// run stepper until stall triggered
bool TMC5160Stepper_Ext::detectEndStop() {
    while (event_stop_sg() == 0) {
        debugStallGuard(); // print to serial monitor
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
void TMC5160Stepper_Ext::executeMovement(int stepDirection, unsigned long stepperDelay) {
    readyStealthChop();

    // step after elapsed amount of time
    if ((millis() - lastCheckMillis() > stepperDelay)) {
        int stepVelocity = stepDirection * stepsPerMovement();
        long targetPosition = XACTUAL() + stepVelocity;
        // set new target position
        XTARGET(targetPosition);
        // Serial.print("Actual before: "); Serial.println(XACTUAL());
        // Serial.print("Target before: "); Serial.println(XTARGET());
        // reduce speed
        targetVelocity(2000);
        // wait for stepper to reach target position
        while(XACTUAL() != XTARGET()) {
            // Serial.print("Actual after: "); Serial.println(XACTUAL());
            // Serial.print("Target after: "); Serial.println(XTARGET());
        }
        targetVelocity(STEALTH_CHOP_VMAX);
        executedMovement(true);    
        lastCheckMillis(millis());
    }
    else {
        executedMovement(false);
  }
}


// execute a predetermined number of steps
void TMC5160Stepper_Ext::executeSteps(long nrSteps) {
    readyStealthChop();
    long targetPosition = XACTUAL() + nrSteps;
    // set new target position
    XTARGET(targetPosition);
    // wait for stepper to reach target position
    while(XACTUAL() != XTARGET()) {}
}


// find the forward and backward end stops of the linear rail using StallGuard
// moves stepper backwards until stall detected, records position
// moves stepper forwards until stall detected, records position and sets position as maximum value (384,000)
// moves stepper to middle of the rail (192,000)
void TMC5160Stepper_Ext::homeRail() {
    bool hitRearStop, hitForwardStop = false;
    readyStallGuard();
    // reset positions
    forwardEndStop(1);
    backwardEndStop(1);

    // move to rear end stop + 100,000 just to ensure it over-runs without decelerating
    XTARGET(-MAX_RAIL_POSITION - 100000);
    hitRearStop = detectEndStop();              // wait until end stop detected
    // if endstop detected... 
    if (hitRearStop) {
        delay(100);
        sg_stop(false);
        sg_stop(true);


        XACTUAL(0);                             // set rear end stop position as 0
        backwardEndStop(XACTUAL());             // set backward position
    }
    // move to forward end stop
    XTARGET(MAX_RAIL_POSITION + 100000);
    hitForwardStop = detectEndStop();           // wait until end stop detected
    // if endstop detected...
    if (hitForwardStop) {
        delay(100);
        sg_stop(false);
        sg_stop(true);


        XACTUAL(MAX_RAIL_POSITION);             // set front end stop position as 0
        forwardEndStop(XACTUAL());              // set forward position
    }

    // if back and forward position set, move to middle position
    if (backwardEndStop() == MIN_RAIL_POSITION && forwardEndStop() == MAX_RAIL_POSITION) {
        XTARGET(192000);
        // wait to reach position before changing driver config after homing completed
        while (XACTUAL() != XTARGET()) {
            debugStallGuard();                  // print to serial monitor
        }

        stack.startPosition(XACTUAL());         // update autoStack positions as rail has moved
        stack.endPosition(XACTUAL());           // update autoStack positions as rail has moved
        configStealthChop();                    // set config for silentStep
        homed(true);
    }
}


void TMC5160Stepper_Ext::video360(long nrSteps) {
    // change to StealthChop if StallGuard is configured
    readyStealthChop();
    // update target velocity as calling configStealthChop resets this on the driver register but not the function
    targetVelocity(targetVelocity());
    // set position
    XTARGET(XACTUAL()+nrSteps);
}


// calculate and set the step size in μm that is travelled per movement
void TMC5160Stepper_Ext::calculateStepSize() {
    float distancePerMovement;

    // constrain multiplier range
    if (stepsPerMovement() < 1) {
        stepsPerMovement(1);
    } else if (stepsPerMovement() > MAX_STEPS_PER_MOVEMENT) {
        stepsPerMovement(MAX_STEPS_PER_MOVEMENT);
    }

    distancePerMovement = MICROSTEP_DIST * stepsPerMovement();

    if (distancePerMovement != stepSize()) {
        // print new delay value
        stack.updateRequiredMovements();
        stepSize(distancePerMovement);

        if (getCurrentScreen() == "Auto") {
            // update estimated time
            auto_screen::estimateDuration();
            // update progress
            auto_screen::printAutoStackProgress();
        }
    }
}