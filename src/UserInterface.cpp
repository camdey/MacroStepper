#include "MiscFunctions.h"
#include "UserInterface.h"

void autoConfigScreen() {
  activeScreen = 4;
  tft.fillScreen(BLACK);
  tft.setTextColor(BLACK);

  // set start point
  tft.setCursor(20, 50);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(CUSTOM_GREEN);
  tft.println("START");
  tft.setCursor(35, 65);
  tft.setFont(&Arimo_Regular_16);
  tft.setTextColor(WHITE);
  tft.println(startPosition);

  // Set end point
  tft.setCursor(20, 130);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(CUSTOM_RED);
  tft.println("END");
  tft.setCursor(35, 145);
  tft.setFont(&Arimo_Regular_16);
  tft.setTextColor(WHITE);
  tft.println(endPosition);

  // Run from start to end and return
  // move back from end point to start point (with accel)
  // then run from start to end with no accel
  tft.setCursor(20, 210);
  tft.setFont(&Lato_Black_34);
  tft.setTextColor(CUSTOM_BLUE);
  tft.println("RUN");
  tft.setCursor(35, 225);
  tft.setFont(&Arimo_Regular_16);
  tft.setTextColor(WHITE, BLACK);
  tft.println(stepper.currentPosition());

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
  tft.setCursor(10, 60);
  tft.setFont(&Arimo_Bold_30);
  tft.println(String(distancePerMovement/1000, 4));

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
  if (shutterState == true) {
    tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_GREEN);
  } else if (shutterState == false) {
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
      tft.setFont(&Arimo_Bold_30);
      tft.getTextBounds(String(prevStepperPosition*0.0025, 4), 9, 220, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, CUSTOM_BLUE);
      tft.setCursor(9, 220);
      tft.setTextColor(WHITE, CUSTOM_BLUE);
      tft.println(String(stepper.currentPosition()*0.0025, 4));

      // update movement count for manual screen
      manualMovementCount++;

      tft.setFont(&Arimo_Bold_30);
      tft.getTextBounds(String(prevManualMovementCount), 45, 140, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, CUSTOM_BLUE);
      tft.setTextColor(WHITE);
      tft.setCursor(45, 140);
      tft.println(manualMovementCount);

      prevManualMovementCount = manualMovementCount;
    }

    if (activeScreen == 4) { // 4 = autoConfigScreen
      tft.setFont(&Arimo_Regular_16);
      tft.getTextBounds(String(prevStepperPosition), 35, 225, &x1, &y1, &w, &h);
      tft.fillRect(x1, y1, w, h, BLACK);
      tft.setCursor(35, 225);
      tft.setTextColor(WHITE, BLACK);
      tft.println(stepper.currentPosition());
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

  tft.setCursor(10, 60);
  tft.setFont(&Arimo_Bold_30);
  tft.println(String(distancePerMovement/1000, 4));

  // Step Number
  // tft.fillRoundRect(5, 85, 125, 65, 14, CUSTOM_GREY);
  tft.fillRoundRect(-10, 85, 135, 65, 15, CUSTOM_BLUE);
  tft.setCursor(5, 110);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Step Nr.");

  tft.setCursor(45, 140);
  tft.setFont(&Arimo_Bold_30);
  tft.println(manualMovementCount);

  // Rail Posoition
  // tft.fillRoundRect(5, 165, 125, 65, 14, CUSTOM_GREY);
  tft.fillRoundRect(-10, 165, 135, 65, 15, CUSTOM_BLUE);
  tft.setCursor(5, 190);
  tft.setFont(&Arimo_Regular_24);
  tft.println("Rail Pos.");
  tft.setCursor(9, 220);
  tft.setFont(&Arimo_Bold_30);
  tft.println(String(stepper.currentPosition()*0.0025, 4));

  // auto shutter
  if (shutterState == true) {
    tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_GREEN);
  } else if (shutterState == false) {
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
  // tft.fillRect(0, 200, 320, 40, CUSTOM_BLUE);
  tft.drawBitmap(30, 190, house, 50, 42, WHITE);

  // Clear autoStack
  if (autoStackFlag == false) {
    tft.drawBitmap(250, 190, reset40, 40, 40, GRAY);
  } else if (autoStackFlag == true) {
    tft.drawBitmap(250, 190, reset40, 40, 40, CUSTOM_RED);
  }
}

void updateProgress(bool screenRefresh) {
  char progressMovements[10];

  int16_t x, y;
  uint16_t w, h;
  sprintf_P(progressMovements, PSTR("%02d/%02d"), movementProgress, numMovements);

  if ((movementProgress != prevMovementProgress) || numMovements != prevNumMovements || screenRefresh == 1) {
    tft.getTextBounds(String(prevProgressMovements), 20, 220, &x, &y, &w, &h);
    tft.fillRect(x, y, w, h, CUSTOM_BLUE);
    tft.setFont(&Arimo_Bold_24);
    tft.setTextColor(WHITE);
    tft.setCursor(20, 220);
    tft.println(progressMovements);

    prevNumMovements = numMovements;
    prevMovementProgress = movementProgress;
    // assign new time to prev variable
    sprintf_P(prevProgressMovements, PSTR("%02d:%02d"), movementProgress, numMovements);
  }
}
