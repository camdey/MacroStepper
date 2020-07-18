#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "UI-Main.h"
#include "UI-Manual.h"
#include "UI-Auto.h"


void rotateScreen() {
  if (!isScreenRotated()) {
    tft.setRotation(3);
    setScreenRotated(true);
  }
  else if (isScreenRotated()) {
    tft.setRotation(1);
    setScreenRotated(false);
  }
}


void setMovementsRequired() {
  // calculate number of steps required to cover range
  int nrMovements = ceil((endPosition*1.00 - startPosition*1.00) / getStepsPerMovement());
  // prevent screen under/overflow of value
  nrMovements = valueCheck(nrMovements, 0, 999);
  setNrMovementsRequired(nrMovements);
  prevMovementsRequired = getNrMovementsRequired();
}


// calculate and set the step size in μm that is travelled per movement
void calculateStepSize() {
  float distancePerMovement;

  // constrain multiplier range
  if (getStepsPerMovement() < 1) {
    setStepsPerMovement(1);
  } else if (getStepsPerMovement() > 100) {
    setStepsPerMovement(100);
  }

  distancePerMovement = microstepLength * getStepsPerMovement();

  if (distancePerMovement != getStepSize()) {
    // print new delay value
    setMovementsRequired();
    setStepSize(distancePerMovement);

    if (getCurrentScreen() == "Auto") {
      // update estimated time
      auto_screen::estimateDuration();
      // update progress
      auto_screen::updateProgress();
    }
  }
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
