#include "MiscFunctions.h"
#include "UI-Main.h"
#include "UI-Manual.h"
#include "UI-Auto.h"


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


void setLastMillis(long millis) {
  lastMillis = millis;
}


long getLastMillis() {
  return lastMillis;
}