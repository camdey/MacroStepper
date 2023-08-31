#include "GlobalVariables.h"
#include "ShutterControl.h"
#include "menu/UI-Auto.h"


/******************************************************************
Checks if last flash read was more than 10ms ago and then takes
reading from FLASH_SENSOR_PIN. The flashThreshold will update if the
ON and OFF values have been changed via the flashScreen.
If the flashSensorValue reading is >= than the Threshold, flashReady
returns TRUE else FALSE.
******************************************************************/
bool isFlashReady() {
    long adjustedVal;
    int weight = 40;

    // val = (w × XSTICK_PIN + (100 – w) × prevVal)
    // multiply values by 100 to avoid floating point math, the prevVal part of the formula needs as much precesion as possible
    adjustedVal = weight * (analogRead(FLASH_SENSOR_PIN)*100) + (100 - weight) * getflashSensorAdjustedValue();
    setflashSensorAdjustedValue(adjustedVal/100);
    setFlashSensorValue(round(adjustedVal*1.00 / 10000));
    // update threshold, e.g. ((310-50)*0.75)+50 = 245
    flashThreshold = int(((flashOnValue - flashOffValue) * 0.75) + flashOffValue);
    // Serial.print("godoxVal: "); Serial.println(getFlashSensorValue());
    if (getFlashSensorValue() >= flashThreshold) {
        setFlashAvailable(true);
    }
    else {
        setFlashAvailable(false);
    }
    
    return (getFlashSensorValue() >= flashThreshold);
}


/******************************************************************
Triggers the camera shutter and flash, if enabled.
It first checks if the flash has recycled (red LED is on) and then
sets the shutter pin to HIGH. A loop will start where the flash is
continually checked to see if it has triggered (red LED goes off)
******************************************************************/
void triggerShutter() {
    if (isFlashSensorEnabled()) {
        runFlashProcedure(true);
    }
    else {
        // if not yet triggered, trigger
        if (digitalRead(SHUTTER_PIN) == LOW) {
            digitalWrite(SHUTTER_PIN, HIGH);
            setFlashTriggerTime(millis());
            Serial.println("pullShutter");

            if (autoStackInitiated) {
                auto_screen::stackStatus(pullShutter);
            }
        }
        // if triggered, don't set LOW until 800ms has passed
        else if (millis() - getFlashTriggerTime() >= SHUTTER_PULL_TIME) {
            digitalWrite(SHUTTER_PIN, LOW);
            // setShutterTriggered(true);
            Serial.println("releaseShutter");

            if (autoStackInitiated) {
                auto_screen::stackStatus(releaseShutter);
            }
        }
    }
}


void runFlashProcedure(bool restart) {
    if (restart) {
        auto_screen::stackStatus(isFlashAvailable);
        setFlashTriggerTime(millis());
        setShutterTriggered(false);
        Serial.println("isFlashAvailable");
    }
    // begin flash sequence
    if (getCurrentStage() == isFlashAvailable) {
        if (isFlashReady()) {
            auto_screen::stackStatus(pullShutter);
            Serial.println("pullShutter");
        }
    }
    else if (getCurrentStage() == pullShutter) {
        digitalWrite(SHUTTER_PIN, HIGH);
        if (isFlashSensorEnabled()) {
            auto_screen::stackStatus(isFlashUnavailable);
            Serial.println("isFlashUnavailable");
        }
        else {
            triggerShutter();
        }
    }
    else if (getCurrentStage() == isFlashUnavailable) {
        if (!isFlashReady()) {
            auto_screen::stackStatus(releaseShutter);
            Serial.println("releaseShutter");
        }
    }
    else if (getCurrentStage() == releaseShutter) {
        digitalWrite(SHUTTER_PIN, LOW);
        auto_screen::stackStatus(flashSuccessful);
        Serial.println("flashSuccessful");
        recycleTime = (millis() - getFlashTriggerTime());
        setShutterTriggered(true);
    }
    // fail over
    if (millis() - getFlashTriggerTime() >= 6000 && (getCurrentStage() == isFlashAvailable || getCurrentStage() == isFlashUnavailable)) {
        auto_screen::stackStatus(flashUnresponsive);
        digitalWrite(SHUTTER_PIN, LOW);
        Serial.println("flashUnresponsive");
    }
}

