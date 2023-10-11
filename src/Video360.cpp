#include "Video360.h"
#include "VariableDeclarations.h"
#include "StepperControl.h"
#include "StatusEnums.h"
#include "UserInterface.h"
#include "CameraControl.h"
#include "menu/UI-Video360.h"

void Video360::run() {
    // change to StealthChop if StallGuard is configured
    if(_stepper.stallGuardActive()) {
        _stepper.readyStealthChop();
    }
    if (!_stepper.slaveSelected() || !_stepper.enabled()) {
        _stepper.slaveSelected(true);
        _stepper.enabled(true);
    }
    // set position
    _stepper.XTARGET(_stepper.XACTUAL()+10000*video360.getDirection());

    if (video360.status() == routines::stopping) {
        _stepper.slaveSelected(false);
        _stepper.enabled(false);
        video360.status(routines::stopped);
        stepper2.targetVelocity(STEALTH_CHOP_VMAX); // reset vmax
    }
}


// convert desired RPM to VMAX for setting max speed
// store STEPS_PER_VMAX and RPM as integers and then divide by necessary denominator to convert back
void Video360::rpmToVmax() {
    int stepsPerRev = ORBIS_MOTOR_STEPS * NR_MICROSTEPS;        // 200 * 16 = 3200
    int rpm = video360.revsPerMinute();                         // e.g. 5.0rpm = 50
    int totalSteps = rpm * stepsPerRev*1.00 / 10;               // 50 * 3200 / 10 = 16000
    int stepsPerSecond = totalSteps*1.00 / 60;                  // 16000 / 60 = 266 SPS
    int vMax = stepsPerSecond*1.00 / STEPS_PER_VMAX * 1000;     // 266 / 727 * 1000 = 365.8
    stepper2.targetVelocity(vMax);
}