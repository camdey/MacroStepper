#include "ShutterControl.h"


/******************************************************************
Checks if last flash read was more than 10ms ago and then takes
reading from GODOX_PIN. The flashThreshold will update if the
ON and OFF values have been changed via the flashScreen.
If the flashValue reading is >= than the Threshold, flashReady
returns TRUE else FALSE.
******************************************************************/
bool flashStatus() {
  int newValue = 0;

  if (millis() - lastReadFlash >= 10) {
    // update light reading from flash
    newValue = analogRead(GODOX_PIN);
    flashValue = newValue;
    lastReadFlash = millis();

    // update threshold, e.g. ((310-50)*0.75)+50 = 245
    flashThreshold = int(((flashOnValue - flashOffValue) * 0.75) + flashOffValue);
  }

  if (flashValue >= flashThreshold) {
    flashReady = true;
  }
  else {
    flashReady = false;
  }

  return flashReady;
}

void toggleShutter() {
  if ((millis() - prevGenericTime) >= genericTouchDelay) {
    shutterEnabled = !shutterEnabled;
    prevGenericTime = millis();
  }
}

/******************************************************************
Triggers the camera shutter and flash, if enabled.
It first checks if the flash has recycled (red LED is on) and then
sets the shutter pin to HIGH. A loop will start where the flash is
continually checked to see if it has triggered (red LED goes off)
but it must stay in this loop for a minimum of 1000ms to ensure
enough time for the camera to take the photo.
******************************************************************/
bool triggerShutter() {
	flashReady = flashStatus();
  bool flashReadyDebouncer = false;
	shutterTriggered = false;


  if (shutterEnabled) {
		unsigned long triggerTime = millis();

		// wait for flash to be ready
		while (!flashReady) {
			flashReady = flashStatus();
      flashReadyDebouncer = flashReady;
      // break if flash not turning on
      if (millis() - triggerTime >= 5000) {
        break;
      }
			delay(10);
		}

		// trigger flash
    digitalWrite(SONY_PIN, HIGH);

    // delay function for giving enough time for camera to trigger
    // also checks if flash has triggered by flashReady = false
    // second variable to debounce noisy readings

    while (millis() - triggerTime <= 3000) {
      if (flashReady) {
        flashReady = flashStatus();
        flashReadyDebouncer = flashStatus();
      }
      if (!flashReady && !flashReadyDebouncer && millis() - triggerTime > 400) {
        shutterTriggered = true;
        break;
      }
      shutterTriggered = false;
      delay(10);
    }

		recycleTime = (millis() - triggerTime);

		// reset shutter signal
    digitalWrite(SONY_PIN, LOW);
	}
	return shutterTriggered;
}
