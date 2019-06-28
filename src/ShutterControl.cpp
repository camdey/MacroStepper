#include "ShutterControl.h"

bool flashStatus() {
  int flashValue = 0;

  flashValue = analogRead(FLASH_PIN);
  Serial.println(flashValue);

  if (flashValue >= 280) {
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
    shutterState = !shutterState;
    prevGenericTime = millis();
  }
}

bool triggerShutter() {
	flashReady = flashStatus();
	shutterTriggered = false;
  if (shutterState == true) {
		unsigned long triggerTime = millis();

		// wait for flash to be ready
		while (flashReady == false) {
			flashReady = flashStatus();
			delay(50);
		}

		// trigger flash
    digitalWrite(SHUTTER_PIN, HIGH);

		// wait for flash to be triggered
		while (shutterTriggered == false) {
			delay(50);
			// if signal missed or shot never taken, break
			if (millis() - triggerTime > 10000) {
				break;
			}
			// check if flash triggered
			flashReady = flashStatus();
			// if signal received, exit and reset shutter
			if (flashReady == false) {
				shutterTriggered = true;
				delay(250); // pause for 250ms
			}
		}
		recycleTime = (millis() - triggerTime);

		// reset shutter signal
    digitalWrite(SHUTTER_PIN, LOW);
	}
	return shutterTriggered;
}
