#include "MiscFunctions.h"
#include "UserInterface.h"
#include "TFT-Manual.h"
#include "TFT-Main.h"
#include "TFT-Auto.h"

// void estimateDuration(bool screenRefresh) {
//   float duration = movementsRequired * (shutterDelay + (recycleTime/1000));
//   float elapsed = completedMovements * (shutterDelay + (recycleTime/1000));
//   float remaining = duration - elapsed;
//   int minutes = floor(remaining / 60);
//   int seconds = int(remaining) % 60;
//   char timeMinutesSeconds[6];
//
//   // prevent screen overflow
//   minutes = valueCheck(minutes, 0, 299);
//
//   int16_t x, y;
//   uint16_t w, h;
//   tft.setFont(&Arimo_Bold_30);
//   tft.setTextColor(WHITE);
//   sprintf_P(timeMinutesSeconds, PSTR("%02d:%02d"), minutes, seconds);
//
//   if ((minutes != prevMinutes || seconds != prevSeconds || screenRefresh == 1) && getCurrentScreen() == "Auto") {
//     tft.getTextBounds(String(prevTimeMinutesSeconds), 15, 140, &x, &y, &w, &h);
//     tft.fillRect(x, y, w, h, CUSTOM_BLUE);
//     tft.setCursor(15, 140);
//     tft.println(timeMinutesSeconds);
//
//     prevMinutes = minutes;
//     prevSeconds = seconds;
//     // assign new time to prev variable
//     sprintf_P(prevTimeMinutesSeconds, PSTR("%02d:%02d"), minutes, seconds);
//   }
// }


// void resetAutoStack() {
//   if ((currentTime - prevGenericTime) >= genericTouchDelay) {
//     autoStackFlag = false;
//     goToStart = true;
//     joystickState = true;
//     pauseAutoStack = false;
//     stepCount = 1;
//     completedMovements = 0;
//     movementsRequired = 0;
//     startPosition = stepper.currentPosition();
//     endPosition = 0;
//     tft.drawBitmap(160, 10, reset40, 40, 40, GRAY);
//
//     prevGenericTime = millis();
//   }
// }


void rotateScreen() {
  if (screenRotated == false) {
    tft.setRotation(3);
    screenRotated = true;
  }
  else if (screenRotated == true) {
    tft.setRotation(1);
    screenRotated = false;
  }
}



void setMovementsRequired() {
  // calculate number of steps required to cover range
  movementsRequired = ceil((endPosition*1.00 - startPosition*1.00) / stepsPerMovement);

  // prevent screen under/overflow of value
  movementsRequired = valueCheck(movementsRequired, 0, 999);
}


void setStepDistance() {

  // constrain multiplier range
  if (stepsPerMovement < 1) {
    stepsPerMovement = 1;
  } else if (stepsPerMovement > 80) {
    stepsPerMovement = 80;
  }

  // stepper is 200 steps/rev, linear rail has 1mm pitch, 1:1 gear ratio
  // 1 step = 1/200 = 0.005mm
	// microsteps = 4
	// 0.005 / 4 = 1.25μm
  distancePerMovement = microstepDistance * stepsPerMovement;
  stepDist = String(distancePerMovement/1000, 5);

  if (prevDistance != distancePerMovement) {
    // print new delay value
    setMovementsRequired();

    if (getCurrentScreen() == "Auto") {
      // update estimated time
      auto_screen::estimateDuration(0); // don't force refresh
      // update progress
      auto_screen::updateProgress(0); // don't force refresh
    }

    prevMovementsRequired = movementsRequired;
  }
}


int valueCheck(int value, int min, int max) {
  if (value > max) {
    value = max;
  }
  if (value < min) {
    value = min;
  }
  else if (value >= min && value <= max) {
    value = value;
  }
  return value;
}
