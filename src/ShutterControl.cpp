#include "GlobalVariables.h"
#include "ShutterControl.h"


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
  Serial.print("godoxVal: "); Serial.println(getGodoxValue());
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
// void triggerShutter() {
// 	setShutterTriggered(false);

//   // if flash bulb enabled
//   if (isBulbEnabled()) {
//     unsigned long startTime = millis();

//     // wait for flash to be ready first
//     while (!isFlashAvailable()) {
//       if (millis() - getLastMillis() >= 10) {
//         isFlashReady();
//         // break if flash not available after 5s
//         if (millis() - startTime >= 5000) {
//           break;
//         }
//         setLastMillis(millis());
//       }
//     }
//     // trigger flash
//     digitalWrite(SONY_PIN, HIGH);
//     // wait for flash to be fired, isFlashAvailable() will be false as godox LED will briefly turn off
//     while (millis() - startTime <= 1500 && isFlashAvailable()) {
//       if (millis() - getLastMillis() >= 10) {
//         isFlashReady();
//         setLastMillis(millis());
//       }
//     }
//     if (!isFlashAvailable()) {
//       setShutterTriggered(true);
//       setFlashTriggerTime(millis());
//     }

//     recycleTime = (millis() - startTime);
//     // reset shutter signal
//     digitalWrite(SONY_PIN, LOW);
//   }
//   else {
//     // trigger flash
//     digitalWrite(SONY_PIN, HIGH);
//     delay(500);
//     digitalWrite(SONY_PIN, LOW);
//     setShutterTriggered(true);
//   }
// }

void triggerShutter() {
  if (isBulbEnabled()) {
    runFlashProcedure(true);
  }
  else {
    // if not yet triggered, trigger
    if (digitalRead(SONY_PIN) == LOW) {
      digitalWrite(SONY_PIN, HIGH);
      setFlashTriggerTime(millis());
    }
    // if triggered, don't set LOW until 800ms has passed
    else if (millis() - getFlashTriggerTime() >= 800) {
      digitalWrite(SONY_PIN, LOW);
      setShutterTriggered(true);
    }
  }
}


void runFlashProcedure(bool restart) {
  if (restart) {
    flashStep = isFlashAvailable;
    setFlashTriggerTime(millis());
    setShutterTriggered(false);
  }
  // begin flash sequence
  if (flashStep == isFlashAvailable) {
    if (isFlashReady()) {
      flashStep = enableShutter;
    }
  }
  else if (flashStep == enableShutter) {
    digitalWrite(SONY_PIN, HIGH);
    flashStep = isFlashUnavailable;
  }
  else if (flashStep == isFlashUnavailable) {
    if (!isFlashReady()) {
      flashStep = disableShutter;
    }
  }
  else if (flashStep == disableShutter) {
    digitalWrite(SONY_PIN, LOW);
    flashStep = isSuccessful;
    recycleTime = (millis() - getFlashTriggerTime());
    setShutterTriggered(true);
  }
  else if (millis() - getFlashTriggerTime() >= 6000) {
    flashStep = isUnresponsive;
  }
}

