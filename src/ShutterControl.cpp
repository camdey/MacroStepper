#include "ShutterControl.h"

bool flashStatus() {
  int newValue = 0;

  if (millis() - lastReadFlash >= 10) {
    // update light reading from flash
    newValue = analogRead(FLASH_PIN);
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

void setShutterDelay() {

  if (shutterDelay < 1) {
    shutterDelay = 1;
  } else if (shutterDelay > 15) {
    shutterDelay = 15;
  }

  if (prevDelay != shutterDelay) {
    // print new delay value
    tft.setCursor(155, 143);
    tft.setFont(&Arimo_Regular_20);
    tft.setTextColor(BLACK, BLACK);
    tft.println(prevDelay);
    tft.setCursor(155, 143);
    tft.setTextColor(WHITE, BLACK);
    tft.println(shutterDelay);
  }
}

void toggleShutter() {
  if ((currentTime - prevGenericTime) >= genericTouchDelay) {
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
	shutterTriggered = false;
  if (shutterEnabled == true) {
		unsigned long triggerTime = millis();

		// wait for flash to be ready
		while (flashReady == false) {
			flashReady = flashStatus();
      // break if flash not turning on
      if (millis() - triggerTime >= 5000) {
        break;
      }
			delay(10);
		}

		// trigger flash
    digitalWrite(SHUTTER_PIN, HIGH);

    // delay function for giving enough time for camera to trigger
    // also checks if flash has triggered by flashReady = false
    while (millis() - triggerTime <= 3000) {
      if (flashReady == true) {
        flashReady = flashStatus();
      }
      if (flashReady == false && millis() - triggerTime > 1000) {
        shutterTriggered = true;
        break;
      }
      shutterTriggered = false;
      delay(10);
    }

		recycleTime = (millis() - triggerTime);

		// reset shutter signal
    digitalWrite(SHUTTER_PIN, LOW);
	}
	return shutterTriggered;
}
