#include "MiscFunctions.h"
#include "UserInterface.h"
#include "ShutterControl.h"

void autoConfigScreen() {
  activeScreen = 4;
  tft.fillScreen(BLACK);
  tft.setTextColor(BLACK);

  // set start point
  tft.setCursor(20, 50);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(CUSTOM_GREEN);
  tft.println("START");
  updateValueField("Start Position", WHITE);

  // Set end point
  tft.setCursor(20, 130);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(CUSTOM_RED);
  tft.println("END");
  updateValueField("End Position", WHITE);

  // Run from start to end and return
  // move back from end point to start point (with accel)
  // then run from start to end with no accel
  tft.setCursor(20, 210);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(CUSTOM_BLUE);
  tft.println("RUN");
  updateValueField("Current Position", WHITE);

  // set delay
  tft.drawBitmap(160, 70, delayClock, 50, 50, WHITE);
  tft.setFont(&Arimo_Regular_20);
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(155, 143);
  tft.println(shutterDelay);
  tft.setCursor(175, 143);
  tft.println(" sec");

  // back to auto screen
  tft.drawBitmap(155, 175, backArrow, 50, 50, WHITE);

  drawArrows();
}

void autoScreen() {
  activeScreen = 3;
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);

  // Step Distance
  tft.fillRoundRect(-10, 5, 135, 65, 15, CUSTOM_BLUE);
  tft.setCursor(5, 30);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Dist.");
  updateValueField("Step Dist", WHITE);

  // Estimated time remaining
  tft.fillRoundRect(-10, 85, 135, 65, 15, CUSTOM_BLUE);
  tft.setCursor(5, 110);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Est. Time");
  // show estimated time
  estimateDuration(1);

  // Step progress
  tft.fillRoundRect(-10, 165, 135, 65, 15, CUSTOM_BLUE);
  tft.setCursor(5, 190);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Progress");
  // show progress, force update due to screen change
  updateProgress(1);

  // auto shutter
  if (shutterEnabled == true) {
    tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_GREEN);
  } else if (shutterEnabled == false) {
    tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_RED);
  }

  // enter auto config
  tft.drawBitmap(155, 95, cogWheel, 50, 50, GRAY);
  // back to main screen
  tft.drawBitmap(155, 175, backArrow, 50, 50, WHITE);

  if (pauseAutoStack == true && autoStackFlag == true) {
    drawPlayPause(0, 1);
  } else if (pauseAutoStack == false && autoStackFlag == true) {
    drawPlayPause(1, 0);
  } else if (pauseAutoStack == false && autoStackFlag == false) {
    drawPlayPause(0, 0);
  }
}

void displayPosition() {
  // update for new values
  if (prevStepperPosition != stepper.currentPosition()) {
    int16_t x1, y1;
    uint16_t w, h;

    if (activeScreen == 2) { // 2 = manual screen
      // update rail position value
      tft.setFont(&Arimo_Bold_24);
      tft.getTextBounds(String(prevStepperPosition*(microstepDistance/1000), 5), 10, 220, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, CUSTOM_BLUE);
      updateValueField("Rail Position", WHITE);

      // update movement count for manual screen
      manualMovementCount++;

      tft.setFont(&Arimo_Bold_30);
      tft.getTextBounds(String(prevManualMovementCount), 45, 140, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, CUSTOM_BLUE);
      updateValueField("Step Nr", WHITE);

      prevManualMovementCount = manualMovementCount;
    }

    if (activeScreen == 4) { // 4 = autoConfigScreen
      tft.setFont(&Arimo_Regular_16);
      tft.getTextBounds(String(prevStepperPosition), 35, 225, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, BLACK);
      updateValueField("Current Position", WHITE);
    }

    prevStepperPosition = stepper.currentPosition();
  }
}

void drawArrows() {
  tft.fillTriangle(230, 65, 260, 15, 290, 65, CUSTOM_GREEN);
  tft.fillRoundRect(245, 69, 30, 36, 4, CUSTOM_GREEN);

  tft.fillTriangle(230, 175, 260, 225, 290, 175, CUSTOM_RED);
  tft.fillRoundRect(245, 135, 30, 36, 4, CUSTOM_RED);
}

void drawPlayPause(bool greyPlay = 0, bool greyPause = 0) {
  if (greyPlay == 1) {
    tft.fillCircle(270, 75, 37, CUSTOM_GREY_LITE);
    tft.fillTriangle(255, 50, 295, 75, 255, 100, WHITE);
  } else {
    tft.fillCircle(270, 75, 37, CUSTOM_GREEN);
    tft.fillTriangle(255, 50, 295, 75, 255, 100, WHITE);
  }

  if (greyPause == 1) {
    tft.fillCircle(270, 180, 37, CUSTOM_GREY_LITE);
    tft.fillRoundRect(250, 155, 15, 50, 4, WHITE);
    tft.fillRoundRect(275, 155, 15, 50, 4, WHITE);
  } else {
    tft.fillCircle(270, 180, 37, CUSTOM_BLUE);
    tft.fillRoundRect(250, 155, 15, 50, 4, WHITE);
    tft.fillRoundRect(275, 155, 15, 50, 4, WHITE);
  }
}

void flashScreen() {
  activeScreen = 5;
  // Serial.print("Flash Value1: ");
  // Serial.println(flashValue);
  // flashReady = flashStatus(); // get latest value
  // Serial.print("Flash Value4: ");
  // Serial.println(flashValue);

  tft.fillScreen(BLACK);
  tft.setTextColor(BLACK);

  // set off value
  tft.setCursor(20, 50);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(CUSTOM_GREEN);
  tft.println("OFF");
  // updateValueField("Flash Off", WHITE);

  // Set on value
  tft.setCursor(20, 130);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(CUSTOM_RED);
  tft.println("ON");
  // updateValueField("Flash On", WHITE);

  // back to auto screen
  tft.drawBitmap(155, 175, backArrow, 50, 50, WHITE);
}

void manualScreen() {
  activeScreen = 2;
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);

  // Step Distance
  // tft.fillRoundRect(5, 5, 125, 65, 14, CUSTOM_GREY);
  tft.fillRoundRect(-10, 5, 135, 65, 15, CUSTOM_BLUE);
  tft.setCursor(5, 30);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Dist.");
  updateValueField("Step Dist", WHITE);

  // Step Number
  // tft.fillRoundRect(5, 85, 125, 65, 14, CUSTOM_GREY);
  tft.fillRoundRect(-10, 85, 135, 65, 15, CUSTOM_BLUE);
  tft.setCursor(5, 110);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Nr.");
  updateValueField("Step Nr", WHITE);

  // Rail Posoition
  // tft.fillRoundRect(5, 165, 125, 65, 14, CUSTOM_GREY);
  tft.fillRoundRect(-10, 165, 135, 65, 15, CUSTOM_BLUE);
  tft.setCursor(5, 190);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Rail Pos.");
  updateValueField("Rail Position", WHITE);

  // auto shutter
  if (shutterEnabled == true) {
    tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_GREEN);
  } else if (shutterEnabled == false) {
    tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_RED);
  }

  // reset steps
  tft.drawBitmap(155, 95, reset, 50, 50, WHITE);

  // back to main screen
  tft.drawBitmap(155, 175, backArrow, 50, 50, WHITE);

  drawArrows();
}

void startScreen() {
  activeScreen = 1;
  tft.fillScreen(BLACK);

  // main logo
  tft.drawBitmap(40, 0, logo, 240, 82, WHITE);

  tft.setTextColor(BLACK);
  // Manual option
  tft.fillRoundRect(20, 100, 130, 60, 8, CUSTOM_BLUE);
  tft.setCursor(30, 140);
  tft.setFont(&Arimo_Bold_30);
  tft.println("Manual");

  // Auto option
  tft.fillRoundRect(170, 100, 130, 60, 8, CUSTOM_GREEN);
  tft.setCursor(200,140);
  tft.setFont(&Arimo_Bold_30);
  tft.println("Auto");

  // Homing option
  tft.drawBitmap(30, 190, house, 50, 42, WHITE);

  // Flash settings
  tft.drawBitmap(145, 190, flash, 40, 42, WHITE);

  // Clear autoStack
  if (autoStackFlag == false) {
    tft.drawBitmap(250, 190, reset40, 40, 40, GRAY);
  } else if (autoStackFlag == true) {
    tft.drawBitmap(250, 190, reset40, 40, 40, CUSTOM_RED);
  }
}

void updateFlashValue() {
  // get latest flashValue reading
  flashReady = flashStatus();

  // if difference from previous reading > 2, updates value on screen
  if (abs(flashValue - prevFlashValue) > 1) {
    if (editFlashOffValue == true) {
      updateValueField("Flash Off", WHITE);
    }
    if (editFlashOnValue == true) {
      updateValueField("Flash On", WHITE);
    }
    // prevFlashValue = flashValue;
  }
}

void updateProgress(bool screenRefresh) {
  char autoStackProgress[10]      = "0/0";

  int16_t x, y;
  uint16_t w, h;
  // displays progress in "Completed / Total" format
  sprintf_P(autoStackProgress, PSTR("%02d/%02d"), completedMovements, movementsRequired);

  if ((completedMovements != prevCompletedMovements) || movementsRequired != prevMovementsRequired || screenRefresh == 1) {
    tft.getTextBounds(String(prevAutoStackProgress), 20, 220, &x, &y, &w, &h);
    tft.fillRect(x, y, w, h, CUSTOM_BLUE);
    tft.setFont(&Arimo_Bold_24);
    tft.setTextColor(WHITE);
    tft.setCursor(20, 220);
    tft.println(autoStackProgress);

    prevMovementsRequired = movementsRequired;
    prevCompletedMovements = completedMovements;
    // assign new time to prev variable
    sprintf_P(prevAutoStackProgress, PSTR("%02d:%02d"), completedMovements, movementsRequired);
  }
}

/***********************************************************************
Generic function for updating various values in different fields.
Easier to update them all via one function so the code is contained in
one place. Some fields are referenced from several parts of the code.
***********************************************************************/
void updateValueField(String valueField, int textColour) {
  // rail position field in Manual screen
  if (valueField == "Rail Position") {
    tft.setTextColor(textColour, CUSTOM_BLUE);
    tft.setFont(&Arimo_Bold_24);
    tft.setCursor(10, 220);
    tft.println(String(stepper.currentPosition()*(microstepDistance/1000), 5));
  }
  // step nr field in Manual screen
  else if (valueField == "Step Nr") {
    tft.setTextColor(textColour, CUSTOM_BLUE);
    tft.setCursor(45, 140);
    tft.setFont(&Arimo_Bold_30);
    tft.println(manualMovementCount);
  }
  // step dist field in Manual and Auto screens
  else if (valueField == "Step Dist") {
    tft.setTextColor(textColour, CUSTOM_BLUE);
    tft.setCursor(15, 60);
    tft.setFont(&Arimo_Bold_24);
    tft.println(String(distancePerMovement/1000, 5));
  }
  // start position in AutoConfig screen
  else if (valueField == "Start Position") {
    tft.setTextColor(textColour, BLACK);
    tft.setCursor(35, 65);
    tft.setFont(&Arimo_Regular_16);
    tft.println(startPosition);
  }
  // end position in AutoConfig screen
  else if (valueField == "End Position") {
    tft.setTextColor(textColour, BLACK);
    tft.setCursor(35, 145);
    tft.setFont(&Arimo_Regular_16);
    tft.println(endPosition);
  }
  // current position in AutoConfig screen
  else if (valueField == "Current Position") {
    tft.setTextColor(textColour, BLACK);
    tft.setCursor(35, 225);
    tft.setFont(&Arimo_Regular_16);
    tft.println(stepper.currentPosition());
  }
  // flashValue from light sensor
  else if (valueField == "Flash Off") {
    int16_t x, y;
    uint16_t w, h;
    tft.setFont(&Arimo_Regular_30);
    tft.setTextColor(textColour, BLACK);
    tft.getTextBounds(String(prevFlashValue), 30, 75, &x, &y, &w, &h);
    tft.fillRect(x, y, w+4, h, BLACK);
    tft.setCursor(30, 75);
    tft.println(flashValue);
  }
  else if (valueField == "Flash On") {
    int16_t x, y;
    uint16_t w, h;
    tft.setFont(&Arimo_Regular_30);
    tft.setTextColor(textColour, BLACK);
    tft.getTextBounds(String(prevFlashValue), 30, 155, &x, &y, &w, &h);
    tft.fillRect(x, y, w+4, h, BLACK);
    tft.setCursor(30, 155);
    tft.println(flashValue);
  }
}
