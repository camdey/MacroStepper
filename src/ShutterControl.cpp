#include "GlobalVariables.h"
#include "ShutterControl.h"
#include "menu/UI-Auto.h"


/******************************************************************
Checks if last flash read was more than 10ms ago and then takes
reading from GODOX_PIN. The flashThreshold will update if the
ON and OFF values have been changed via the flashScreen.
If the godoxValue reading is >= than the Threshold, flashReady
returns TRUE else FALSE.
******************************************************************/
bool isFlashReady() {
  long adjustedVal;
  int weight = 40;

  // val = (w × XSTICK_PIN + (100 – w) × prevVal)
  // multiply values by 100 to avoid floating point math, the prevVal part of the formula needs as much precesion as possible
  adjustedVal = weight * (analogRead(GODOX_PIN)*100) + (100 - weight) * getGodoxFilterValue();
  setGodoxFilterValue(adjustedVal/100);
  setGodoxValue(round(adjustedVal*1.00 / 10000));
  // update threshold, e.g. ((310-50)*0.75)+50 = 245
  flashThreshold = int(((flashOnValue - flashOffValue) * 0.75) + flashOffValue);
  // Serial.print("godoxVal: "); Serial.println(getGodoxValue());
  if (getGodoxValue() >= flashThreshold) {
    setFlashAvailable(true);
  }
  else {
    setFlashAvailable(false);
  }
  
  return (getGodoxValue() >= flashThreshold);
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
    if (digitalRead(SONY_PIN) == LOW) {
      digitalWrite(SONY_PIN, HIGH);
      setFlashTriggerTime(millis());
      Serial.println("pullShutter");
      auto_screen::stackStatus(pullShutter);
    }
    // if triggered, don't set LOW until 800ms has passed
    else if (millis() - getFlashTriggerTime() >= 800) {
      digitalWrite(SONY_PIN, LOW);
      // setShutterTriggered(true);
      Serial.println("releaseShutter");
      auto_screen::stackStatus(releaseShutter);
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
  if (getStackProcedureStage() == isFlashAvailable) {
    if (isFlashReady()) {
      auto_screen::stackStatus(pullShutter);
      Serial.println("pullShutter");
    }
  }
  else if (getStackProcedureStage() == pullShutter) {
    digitalWrite(SONY_PIN, HIGH);
    if (isFlashSensorEnabled()) {
      auto_screen::stackStatus(isFlashUnavailable);
      Serial.println("isFlashUnavailable");
    }
    else {
      triggerShutter();
    }
  }
  else if (getStackProcedureStage() == isFlashUnavailable) {
    if (!isFlashReady()) {
      auto_screen::stackStatus(releaseShutter);
      Serial.println("releaseShutter");
    }
  }
  else if (getStackProcedureStage() == releaseShutter) {
    digitalWrite(SONY_PIN, LOW);
    auto_screen::stackStatus(flashSuccessful);
    Serial.println("flashSuccessful");
    recycleTime = (millis() - getFlashTriggerTime());
    setShutterTriggered(true);
  }
  // fail over
  if (millis() - getFlashTriggerTime() >= 6000 && (getStackProcedureStage() == isFlashAvailable || getStackProcedureStage() == isFlashUnavailable)) {
    auto_screen::stackStatus(flashUnresponsive);
    digitalWrite(SONY_PIN, LOW);
    Serial.println("flashUnresponsive");
  }
}

