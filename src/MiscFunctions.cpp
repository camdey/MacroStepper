#include "MiscFunctions.h"
#include "UserInterface.h"

void estimateDuration(bool screenRefresh) {
  float duration = numMovements * (shutterDelay + (recycleTime/1000));
  float elapsed = movementProgress * (shutterDelay + (recycleTime/1000));
  float remaining = duration - elapsed;
  int minutes = floor(remaining / 60);
  int seconds = int(remaining) % 60;
  char timeMinutesSeconds[5];

  int16_t x, y;
  uint16_t w, h;
  tft.setFont(&Arimo_Bold_30);
  tft.setTextColor(WHITE);
  sprintf_P(timeMinutesSeconds, PSTR("%02d:%02d"), minutes, seconds);

  if ((minutes != prevMinutes || seconds != prevSeconds || screenRefresh == 1) && activeScreen == 3) {
    tft.getTextBounds(String(prevTimeMinutesSeconds), 15, 140, &x, &y, &w, &h);
    tft.fillRect(x, y, w, h, CUSTOM_BLUE);
    tft.setCursor(15, 140);
    tft.println(timeMinutesSeconds);

    prevMinutes = minutes;
    prevSeconds = seconds;
    // assign new time to prev variable
    sprintf_P(prevTimeMinutesSeconds, PSTR("%02d:%02d"), minutes, seconds);
  }
}

void resetAutoStack() {
  if ((currentTime - prevGenericTime) >= genericTouchDelay) {
    autoStackFlag = false;
    goToStart = true;
    joystickState = true;
    pauseAutoStack = false;
    // stepDue = true;
    stepCount = 1;
    movementProgress = false;
    numMovements = 0;
    startPosition = stepper.currentPosition();
    endPosition = 0;
    tft.drawBitmap(250, 190, reset40, 40, 40, GRAY);

    prevGenericTime = millis();
  }
}

void setStepDistance() {

  // constrain multiplier range
  if (stepsPerMovement < 1) {
    stepsPerMovement = 1;
  } else if (stepsPerMovement > 80) {
    stepsPerMovement = 40;
  }

  // stepper is 200 steps/rev, linear rail has 1mm pitch, 1:1 gear ratio
  // 1 step = 1/200 = 0.005mm
	// microsteps = 4
	// 0.005 / 4 = 1.25μm
  distancePerMovement = microstepDistance * stepsPerMovement;

  if (prevDistance != distancePerMovement) {
    // print new delay value
    int16_t x1, y1;
    uint16_t w, h;
    tft.setFont(&Arimo_Bold_30);
    tft.getTextBounds(String(prevDistance/1000, 4), 10, 60, &x1, &y1, &w, &h);
    tft.fillRect(x1, y1, w, h, CUSTOM_BLUE);
    tft.setCursor(10, 60);
    tft.setTextColor(YELLOW, CUSTOM_BLUE);
    tft.println(String(distancePerMovement/1000, 4));

    // calculate number of steps required to cover range
    numMovements = ceil((endPosition - startPosition)*1.00/stepsPerMovement);

    if (activeScreen == 3) {
      // update estimated time
      estimateDuration(0); // don't force refresh
      // update progress
      updateProgress(0); // don't force refresh
    }

    prevNumMovements = numMovements;
  }
}

void setAutoStackPositions(bool setStart, bool setEnd) {
  if (setStart == 1) {
    // lower limit
    if (startPosition < 0) {
      startPosition = 0;
    }
    // get new value
    startPosition = stepper.currentPosition();
    // print start point if changed
    if (prevStartPosition != startPosition && activeScreen == 4) {
      int16_t x1, y1;
      uint16_t w, h;
      tft.getTextBounds(String(prevStartPosition), 35, 65, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, BLACK);
      tft.setCursor(35, 65);
      tft.setFont(&Arimo_Regular_16);
      tft.setTextColor(WHITE, BLACK);
      tft.println(startPosition);
    }
    // reset macroStepping
    goToStart = true;
  }

  if (setEnd == 1) {
    // lower limit
    if (endPosition < 0) {
      endPosition = 0;
    }
    // get new value
    endPosition = stepper.currentPosition();
    // print end point if changed
    if (prevEndPosition != endPosition && activeScreen == 4) {
      int16_t x1, y1;
      uint16_t w, h;
      tft.getTextBounds(String(prevEndPosition), 35, 145, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, BLACK);
      tft.setCursor(35, 145);
      tft.setFont(&Arimo_Regular_16);
      tft.setTextColor(WHITE, BLACK);
      tft.println(endPosition);
    }
  }

  // calculate number of steps required to cover range
  numMovements = ceil((endPosition - startPosition)/stepsPerMovement);
  prevNumMovements = numMovements;
}
