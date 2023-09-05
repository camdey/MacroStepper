#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "StepperControl.h"
#include "JoystickControl.h"
#include "UserInterface.h"
#include "menu/UI-Manual.h"
#include "menu/UI-AutoConfig.h"


// calculate new velocity (VMAX) value while using joystick motion
// velocity value is a log-transformation of the joystick position
long Joystick::calculateVelocity(int pos) {
    long velocity;
    int xAdj;

    if (pos < 512) {
        xAdj = pos + 1;                 // prevent taking log of 0
    }
    else if (pos >= 512) {
        xAdj = (1024 - pos);
    }
    xAdj = round(log(xAdj)*100);        // multiply by 100 as value will be truncated to integer
    // 624 = log(512)*100 rounded
    // Serial.print("joystick vel: "); Serial.println(getJoystickMaxVelocity());
    velocity = map(xAdj, 0, 624, maxVelocity(), 0);

    return velocity;
}

/******************************************************************
Take 10 readings form the joystick X axis during setup and average
the result. This becomes the resting value for the joystick. If
out of the estimated range (500, 524), set at 512 to be safe.
Add/substract 10 to this resting value to get an upper and lower
bound for detecting movement. Also record difference between the
resting value and ideal mid-point (511.5) to offset pos readings
by in other functions that use this variable.
******************************************************************/
void Joystick::calibrateRestPosition() {
    int numReadings = 10;
    int sumReadings = 0;
    restValDiff(0); // ensure reset to 0 when this function is called

    for (int xth_reading = 0; xth_reading < numReadings; xth_reading++) {
        int pos = read();
        sumReadings += pos; // add reading to total
        delay(20); // wait 20ms
    }

    restVal((sumReadings / numReadings) + 0.5);  // "round" to nearest whole number

    // if restVal() below/above these values, set at middle point to be safe
    if (restVal() < 500 || restVal() > 524) {
        restVal(512);
    }
    restValDiff(restVal() + 10);
    restValLower(restVal() - 10);
    restValDiff(512 - restVal());
}


/******************************************************************
Function for moving the stepper via joystick control on the X axis.
Re-enables stepper if previously disabled and then checks to make
sure the joystick values are still in the operation range.    If the
rail has been homed, it will prevent user from allowing rail to
bump into endstops. Every 50ms it checks for new joystick input
values and also adjusts speed based on joystick input position.
For the first 5000 loops of the while() function, a modifier is
used to reduce the initial acceleration of the stepper to control
jerk. After 5000 steps, modifier is redundant and normal speed
control resumes.
******************************************************************/
void Joystick::motion(bool btnTrigger) {
    _stepper.slaveSelected(true);
    _stepper.enabled(true);
    _stepper.readyStealthChop();
    int pos = readSmoothed();
    int dir = getDirection(pos);
    long velocity = calculateVelocity(pos);

    while ((pos >= restValUpper() || pos <= restValLower()) && (buttonActive() || !btnTrigger)) {
        // don't allow movement if within 2mm of endstops if homed
        if (_stepper.homed()) {
            if (dir == 0 && _stepper.XACTUAL() <= SAFE_ZONE_BUFFER) {
                Serial.print("WARNING: hit SAFE_ZONE_BUFFER (within 2mm of rear end stop): "); Serial.println(_stepper.XACTUAL());
                produceTone(1, 250, 0);
                break;
            }
            else if (dir == 1 && MAX_RAIL_POSITION - _stepper.XACTUAL() <= SAFE_ZONE_BUFFER) {
                Serial.print("WARNING: hit SAFE_ZONE_BUFFER (within 2mm of front end stop): "); Serial.println(_stepper.XACTUAL());
                produceTone(1, 250, 0);
                break;
            }
        }
    
        // update velocity if more than 100ms since last reading
        if (millis() - lastCheckMillis() >= 100) {
            velocity = calculateVelocity(pos);
            // Serial.print(" | pos: "); Serial.print(pos);
            // Serial.print(" | currentPos: "); Serial.print(_stepper.XACTUAL());
            // Serial.print(" | targetPos: "); Serial.print(_stepper.XTARGET());
            // Serial.print(" | VACTUAL: "); Serial.print(_stepper.VACTUAL());
            // Serial.print(" | velocity: "); Serial.println(velocity);

            // buttonActive();                      // check if button still pressed, no longer needed as the call in the while clause should update automatically
            printNewPositions();                    // print new positions on the display
            lastCheckMillis(millis());
        }
        // set target to move stepper
        if (dir == 0) {
            _stepper.XTARGET(MAX_RAIL_POSITION*-1);
        }
        else if (dir == 1) {
            _stepper.XTARGET(MAX_RAIL_POSITION);
        }
    
        _stepper.targetVelocity(velocity);
        pos = readSmoothed();
    }
    _stepper.targetVelocity(0);                     // bring stepper to a stop
    while (_stepper.VACTUAL() != 0) {}              // wait for stepper to decelerate
    _stepper.XTARGET(_stepper.XACTUAL());           // reset target to actual, call after setting targetVelocity to avoid "bounce"
    printNewPositions();                            // print final positions now that stepper has stopped
    _stepper.targetVelocity(STEALTH_CHOP_VMAX);     // reset VMAX to stealthChop default
    _stepper.slaveSelected(false);
    _stepper.enabled(false);
}


// Print new rail positions depending on what screen and buttons are active
// Called during joystick movement and after joystick movement when stepper has stopped
// otherwise the position values will be out of sync across different fields
void Joystick::printNewPositions() {
    if (ui.activeScreen() == routines::ui_Manual) {                           // if on Manual screen, print new positon
        manual_screen::printPosition();
    }
    else if (ui.activeScreen() == routines::ui_AutoConfig) {                  // else if Auto Config screen, print new position
        autoconfig_screen::printPosition();

        if (ui.canEdit(routines::btn_startPosition)) {
        autoconfig_screen::updateStartPosition();                   // if editing Start position, set start but not end position
        }
        else if (ui.canEdit(routines::btn_endPosition)) {
            autoconfig_screen::updateEndPosition(); 	            // if editing End position, set end but not start position
        }
    }
}


// read value from XSTICK_PIN and use recursive filtering to minimize noise
// the previous filtered value from the joystick is used with a weight constant
int Joystick::readSmoothed() {
    long val, adjVal;
    int weight = 40;

    // val = (w × XSTICK_PIN + (100 – w) × prevVal)
    // multiply values by 100 to avoid floating point math, the prevVal part of the formula needs as much precision as possible
    adjVal = weight * (read()*100) + (100 - weight) * recursiveFilterValue();
    recursiveFilterValue(adjVal/100);
    val = round(adjVal*1.00 / 10000);

    if (ui.screenRotated()) {
        val = map(val, 0, 1023, 1023, 0);
    }
    // offset reading by difference between resting state and ideal middle point
    // val -= xStickDiff;
    return val;
}
