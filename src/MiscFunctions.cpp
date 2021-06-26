#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "menu/UI-Main.h"
#include "menu/UI-Manual.h"
#include "menu/UI-Auto.h"


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


// sound a tone with a given number of repetitions each for a specified length of time (ms) separated by an interval (ms)
void produceTone(int reps, int duration, int interval) {
  for (int i = 0; i < reps; i++) {
    TimerFreeTone(PIEZO_PIN, 4000, duration, 10);
    delay(interval);
  }
}


void setMovementsRequired() {
  // calculate number of steps required to cover range
  int nrMovements = ceil((getEndPosition()*1.00 - getStartPosition()*1.00) / getStepsPerMovement());
  // prevent screen under/overflow of value
  nrMovements = valueCheck(nrMovements, 0, 999);
  setNrMovementsRequired(nrMovements);
}


// calculate and set the step size in μm that is travelled per movement
void calculateStepSize() {
  float distancePerMovement;

  // constrain multiplier range
  if (getStepsPerMovement() < 1) {
    setStepsPerMovement(1);
  } else if (getStepsPerMovement() > MAX_STEPS_PER_MOVEMENT) {
    setStepsPerMovement(MAX_STEPS_PER_MOVEMENT);
  }

  distancePerMovement = MICROSTEP_DIST * getStepsPerMovement();

  if (distancePerMovement != getStepSize()) {
    // print new delay value
    setMovementsRequired();
    setStepSize(distancePerMovement);

    if (getCurrentScreen() == "Auto") {
      // update estimated time
      auto_screen::estimateDuration();
      // update progress
      auto_screen::printAutoStackProgress();
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
