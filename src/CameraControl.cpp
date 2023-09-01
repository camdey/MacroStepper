#include "VariableDeclarations.h"
#include "StepperConfig.h"
#include "GlobalVariables.h"
#include "AutoStack.h"
#include "StepperControl.h"
#include "CameraControl.h"
#include "menu/UI-Auto.h"


/******************************************************************
Checks if last flash read was more than 10ms ago and then takes
reading from FLASH_SENSOR_PIN. The flashThreshold will update if the
ON and OFF values have been changed via the flashScreen.
If the flashSensorValue reading is >= than the Threshold, flashReady
returns TRUE else FALSE.
******************************************************************/
void CameraControl::checkFlashAvailability() {
    long adjustedVal;
    int weight = 40;

    // val = (w × XSTICK_PIN + (100 – w) × prevVal)
    // multiply values by 100 to avoid floating point math, the prevVal part of the formula needs as much precesion as possible
    adjustedVal = weight * (analogRead(FLASH_SENSOR_PIN)*100) + (100 - weight) * sensorAdjValue();
    sensorAdjValue(adjustedVal/100);
    sensorValue(round(adjustedVal*1.00 / 10000));
    // update threshold, e.g. ((310-50)*0.75)+50 = 245
    int threshold = int(((flashOnValue() - flashOffValue()) * 0.75) + flashOffValue());
    flashThreshold(threshold);
    // Serial.print("godoxVal: "); Serial.println(getFlashSensorValue());
    if (sensorValue() >= flashThreshold()) {
        // if we are mid-flash procedure, status will be shutterHigh so keep this status unless flash becomes unavailable
        if (status() == shutterHigh) {
            status(shutterHigh);
        } else {
            status(flashAvailable);
        }
    }
    else {
        // if we are mid-flash proecdure, status will be shutterHigh so if flash becomes unavailable it likely means we triggered
        // the flash successfully
        if (status() == shutterHigh) {
            status(flashRecycling);
        } else {
            status(flashUnavailable);
        }
    }
}


/******************************************************************
Triggers the camera shutter and flash, if enabled.
It first checks if the flash has recycled (red LED is on) and then
sets the shutter pin to HIGH. A loop will start where the flash is
continually checked to see if it has triggered (red LED goes off)
******************************************************************/
void CameraControl::triggerShutter() {
    if (flashSensorEnabled()) {
        runFlashProcedure(true);
    }
    else {
        // if not yet triggered, trigger
        if (digitalRead(shutterPin() == LOW)) {
            digitalWrite(shutterPin(), HIGH);
            status(shutterHigh);
            lastFlashMillis(millis());
        }
        // if triggered, don't set LOW until 800ms has passed
        else if (millis() - lastFlashMillis() >= SHUTTER_PULL_TIME) {
            digitalWrite(shutterPin(), LOW);
            status(shutterCompleted);

            if (stack.status() == waitShutter) {
                stack.status(newMovement);
            }
        }
    }
}


/******************************************************************
Flash Procedure is used when the flash sensor is enabled and we need
to check that the flash is (a) available, (b) becomes unavailable
during recycling, and (c) is available again
******************************************************************/
void CameraControl::runFlashProcedure(bool restart) {
    long millisSinceLastFlash = millis() - lastFlashMillis();

    if (restart) {
        checkFlashAvailability();
        // begin flash sequence, make sure shutter pin is pulled low
        if (status() == flashAvailable) {
            photoTaken(false);
            lastFlashMillis(millis());
            digitalWrite(shutterPin(), LOW);
            status(shutterLow);
        } else if (status() == flashUnavailable) {
            status(flashUnresponsive);
        }
    }
    // if shutter low, pull it high to begin shutter triggering
    if (status() == shutterLow) {
        digitalWrite(shutterPin(), HIGH);
        status(shutterHigh);
    }
    // continually check flash availability while shutter is pulled high
    if (status() == shutterHigh) {
        checkFlashAvailability();
    }
    // if checkFlashAvailability() detected a successful trigger and pin is currently high
    // it means we are mid-procedure and can sent the shutterPin low
    if (status() == flashRecycling && digitalRead(shutterPin() == HIGH)) {
        digitalWrite(shutterPin(), LOW);
        status(shutterCompleted);
    }
    // continually check flash availability after successful recycle detection
    // we can then know when the flash is available again
    if (status() == shutterCompleted) {
        photoTaken(true);
        checkFlashAvailability();
    }
    if (status() == flashAvailable && photoTaken()) {
        recycleTime(millis() - lastFlashMillis());
    }
    // if status is still shutterHigh after 1000ms, it may be that we didn't detect the flash recycling
    // or the flash failed to trigger, keep trying for 5000ms longer
    if (status() == shutterHigh && millisSinceLastFlash > 1000 && millisSinceLastFlash < 6000) {
        checkFlashAvailability();
    }
    // fail over if we still don't detect a successful flash after 6000ms
    if (status() == shutterHigh && millisSinceLastFlash >= 6000 ) {
        status(flashUnresponsive);
        digitalWrite(shutterPin(), LOW);
    }
}
// check that once flash triggers and sensor detects recycling, that we then wait for it to become available again

// -- begin procedure
// -- -- if flash available
// -- -- -- reset flags, pull pin low, reset last millis, set status shutterLow
// -- -- if flash unavailable
// -- -- -- set as unresponsive
// -- -- if shutter is low
// -- -- -- pull pin high, set status to shutterHigh
// -- -- if shutter is high
// -- -- -- check flash availability, status should change to flashRecycling
// -- -- if flash recucling and pin is high
// -- -- -- set pin low, set status completed
// -- -- if shutter completed
// -- -- -- photo is taken, check availability for change back to flash available
// -- -- if flash available and photo taken
// -- -- -- get recycle time
// -- -- if shutter still high and less than 6000ms has passed
// -- -- -- keep checking flash sensor for recycle signal
// -- -- if shutter high and 6000ms has passed
// -- -- -- set flash as unresponsive