#include "GlobalVariables.h"
#include "ShutterControl.h"


/******************************************************************
Checks if last flash read was more than 10ms ago and then takes
reading from GODOX_PIN. The flashThreshold will update if the
ON and OFF values have been changed via the flashScreen.
If the godoxValue reading is >= than the Threshold, flashReady
returns TRUE else FALSE.
******************************************************************/
void checkFlashStatus() {
  long adjustedVal;
  int weight = 40;

  // val = (w × XSTICK_PIN + (100 – w) × prevVal)
  // multiply values by 100 to avoid floating point math, the prevVal part of the formula needs as much precesion as possible
  adjustedVal = weight * (analogRead(GODOX_PIN)*100) + (100 - weight) * getGodoxFilterValue();
  setGodoxFilterValue(adjustedVal/100);
  setGodoxValue(round(adjustedVal*1.00 / 10000));
  // update threshold, e.g. ((310-50)*0.75)+50 = 245
  flashThreshold = int(((flashOnValue - flashOffValue) * 0.75) + flashOffValue);

  if (getGodoxValue() >= flashThreshold) {
    setFlashAvailable(true);
  }
  else {
    setFlashAvailable(false);
  }
}


/******************************************************************
Triggers the camera shutter and flash, if enabled.
It first checks if the flash has recycled (red LED is on) and then
sets the shutter pin to HIGH. A loop will start where the flash is
continually checked to see if it has triggered (red LED goes off)
******************************************************************/
void triggerShutter() {
	setShutterTriggered(false);

  if (isShutterEnabled()) {
		unsigned long startTime = millis();

		// wait for flash to be ready first
		while (!isFlashAvailable()) {
      if (millis() - getLastMillis() >= 10) {
        checkFlashStatus();
        // break if flash not available after 5s
        if (millis() - startTime >= 5000) {
          break;
        }
        setLastMillis(millis());
      }
		}

		// trigger flash
    digitalWrite(SONY_PIN, HIGH);

    // wait for flash to be fired, isFlashAvailable() will be false as godox LED will briefly turn off
    while (millis() - startTime <= 3000 && isFlashAvailable()) {
      if (millis() - getLastMillis() >= 10) {
        checkFlashStatus();
        setLastMillis(millis());
      }
    }
    if (!isFlashAvailable()) {
      setShutterTriggered(true);
    }

		recycleTime = (millis() - startTime);

		// reset shutter signal
    digitalWrite(SONY_PIN, LOW);
	}
}
