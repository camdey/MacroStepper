#include "MiscFunctions.h"
#include "UI-Main.h"
#include "UI-Manual.h"
#include "UI-Auto.h"


void rotateScreen() {
  if (!screenRotated) {
    tft.setRotation(3);
    screenRotated = true;
  }
  else if (screenRotated) {
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


// calculate and set the step size in μm that is travelled per movement
void calculateStepSize() {
  float distancePerMovement;

  // constrain multiplier range
  if (stepsPerMovement < 1) {
    stepsPerMovement = 1;
  } else if (stepsPerMovement > 80) {
    stepsPerMovement = 80;
  }

  // stepper is 400 steps/rev, linear rail has 2mm pitch
  // 1 step = 2/400 = 0.005mm
	// microsteps = 16
	// 0.005 / 16 = 0.3125μm
  distancePerMovement = microstepLength * stepsPerMovement;

  if (distancePerMovement != getStepSize()) {
    // print new delay value
    setMovementsRequired();
    setStepSize(distancePerMovement);

    if (getCurrentScreen() == "Auto") {
      // update estimated time
      auto_screen::estimateDuration(0); // don't force refresh
      // update progress
      auto_screen::updateProgress(0); // don't force refresh
    }

    prevMovementsRequired = movementsRequired;
  }
}

// set float value of distance travelled per movement in μm, call from calculateStepSize()
void setStepSize(float size) {
  stepSize = size;
}


// return float value of distance travelled per movement in μm
float getStepSize() {
  return stepSize;
}


// check whether a given value is in a specified range
// if not change value to min or max, whichever is closest
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


// Set the last millis() reading, useful for doing things within loops every X ms 
void setLastMillis(long millis) {
  lastMillis = millis;
}


// Get the last millis() reading, useful for doing things within loops every X ms
long getLastMillis() {
  return lastMillis;
}


// Set the state of the GUI arrows to on/off 
void setArrowState(bool state) {
  arrowsActive = state;
}


// Get the state of the GUI arrows
bool getArrowState() {
  return arrowsActive;
}