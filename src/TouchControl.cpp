#include "MiscFunctions.h"
#include "ShutterControl.h"
#include "StepperControl.h"
#include "TouchControl.h"
#include "UserInterface.h"

int arrowsTouch(int xPos, int yPos, bool moveStepper, int val = 0) {

  if ((xPos >= 230 && xPos <= 290) && (yPos >= 15 && yPos <= 105)) {
    val++;
    // if stepping required, trigger shutter, move step
    if (moveStepper == true) {
      if (shutterEnabled == true && activeScreen == 2) {
        shutterTriggered = triggerShutter();
      }
      stepperMoved = stepMotor(1, 500); // forward
    }
  }
  if ((xPos >= 230 && xPos <= 290) && (yPos >= 135 && yPos <= 225)) {
    val--;
    if (moveStepper == true) { // if stepping required, move motor
      stepperMoved = stepMotor(-1, 500); // reverse
    }
  }

  return val;
}


void autoConfigScreenTouch(int xPos, int yPos) {

  // start button
  if ((xPos >= 10 && xPos <= 140) && (yPos >= 10 && yPos <= 80) && (editShutterDelay + editEndPosition) == 0) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editStartPosition = !editStartPosition;

      if (arrowsActive == false && editStartPosition == false) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, BLACK);
        tft.fillRoundRect(10, 24, 5, 40, 4, BLACK);
      }
      if (arrowsActive == true && editStartPosition == true) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, YELLOW);
        tft.fillRoundRect(10, 24, 5, 40, 4, YELLOW);
      }

      prevGenericTime = millis();
    }
  }
  // set start position using arrows or joystick
  if (editStartPosition == true && arrowsActive == true) {
    if (prevStartPosition != startPosition) {
      prevStartPosition = startPosition;
    }
    int val = arrowsTouch(xPos, yPos, 1, 0);
    setAutoStackPositions(true, false); //set start but not end position
  }

  // end button
  if ((xPos >= 10 && xPos <= 120) && (yPos >= 90 && yPos <= 160) && (editShutterDelay + editStartPosition) == 0) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editEndPosition = !editEndPosition;

      if (arrowsActive == false && editEndPosition == false) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, BLACK);
        tft.fillRoundRect(10, 104, 5, 40, 4, BLACK);
      }
      if (arrowsActive == true && editEndPosition == true) {
        // tft.drawRoundRect(15, 15, 125, 60, 4, YELLOW);
        tft.fillRoundRect(10, 104, 5, 40, 4, YELLOW);
      }

      prevGenericTime = millis();
    }
  }
  // set end position using arrows or joystick
  if (editEndPosition == true && arrowsActive == true) {
    if (prevEndPosition != endPosition) {
      prevEndPosition = endPosition;
    }

    int val = arrowsTouch(xPos, yPos, 1, 0);
    setAutoStackPositions(false, true); //set end but not start position
  }

  // run button
  if ((xPos >= 10 && xPos <= 120) && (yPos >= 170 && yPos <= 240) && (editShutterDelay + editStartPosition + editEndPosition) == 0) {
    tft.fillRoundRect(10, 184, 5, 35, 4, YELLOW);
    dryRun();
    tft.fillRoundRect(10, 184, 5, 35, 4, BLACK);
  }

  // delay button
  if ((xPos >= 150 && xPos <= 220) && (yPos >= 60 && yPos <= 130) && (editStartPosition + editEndPosition) == 0) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editShutterDelay = !editShutterDelay;

      if (arrowsActive == false && editShutterDelay == false) {
        tft.drawBitmap(160, 70, delayClock, 50, 50, WHITE);
      }
      if (arrowsActive == true && editShutterDelay == true) {
        tft.drawBitmap(160, 70, delayClock, 50, 50, YELLOW);
      }

      prevGenericTime = millis();
    }
  }

  // set shutter delay
  if (editShutterDelay == true && arrowsActive == true) {
    if (prevDelay != shutterDelay) {
      prevDelay = shutterDelay;
    }
    shutterDelay = arrowsTouch(xPos, yPos, 0, shutterDelay);
    setShutterDelay();
  }

  // clear autoStack run
  if ((xPos >= 150 && xPos <= 210) && (yPos >= 0 && yPos <= 50) && autoStackFlag == true) {
    resetAutoStack(); // reset autostack settings
  }

  // show current position of stepper
  displayPosition();

  // back button - tft.setCursor(150, 205);
  if ((xPos >= 150 && xPos <= 210) && (yPos >= 170 && yPos <= 225) && arrowsActive == false) {
    autoScreen();
  }
}


void autoScreenTouch(int xPos, int yPos) {

  // shutter toggle
  if ((xPos >= 140 && xPos <= 210) && (yPos >= 10 && yPos <= 70)) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      toggleShutter();
      if (shutterEnabled == true) {
        tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_GREEN);
      } else if (shutterEnabled == false) {
        tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_RED);
      }
      prevGenericTime = millis();
    }
  }
  // config button
  if ((xPos >= 140 && xPos <= 210) && (yPos >= 80 && yPos <= 150)) {
    autoConfigScreen();
  }
  // step distance button
  if ((xPos >= 5 && xPos <= 130) && (yPos >= 5 && yPos <= 70)) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editMovementDistance = !editMovementDistance;

      if (arrowsActive == false && editMovementDistance == false) {
        tft.setTextColor(WHITE);
        tft.setCursor(5, 30);
        tft.setFont(&Arimo_Regular_24);
        tft.println("Step Dist.");
        updateValueField("Step Dist", WHITE);
      }
      if (arrowsActive == true && editMovementDistance == true) {
        tft.setTextColor(YELLOW);
        tft.setCursor(5, 30);
        tft.setFont(&Arimo_Regular_24);
        tft.println("Step Dist.");
        updateValueField("Step Dist", YELLOW);
      }

      prevGenericTime = millis();
    }
  }
  // set step size
  if (arrowsActive == true && editMovementDistance == true) {
    if (prevDistance != distancePerMovement) {
      prevDistance = distancePerMovement;
    }

    stepsPerMovement = arrowsTouch(xPos, yPos, 0, stepsPerMovement);
    setStepDistance();
  }
  // run AutoStack sequence
  if (arrowsActive == false && (xPos >= 230 && xPos <= 310) && (yPos >= 30 && yPos <= 120)) {
    autoStackFlag = true;
    pauseAutoStack = false; // re-enable autoStack + pause button
    drawPlayPause(1, 0); // grey out play, recolour pause button
  }
  // pause/resume AutoStack sequence
  if (arrowsActive == false && pauseAutoStack == false && autoStackFlag == true && (xPos >= 230 && xPos <= 310) && (yPos >= 135 && yPos <= 225)) {
    pauseAutoStack = true;
    drawPlayPause(0, 1); // recolour play, grey out pause button
  }
  // back button
  if ((xPos >= 150 && xPos <= 210) && (yPos >= 170 && yPos <= 225) && arrowsActive == false) {
    startScreen();
  }
}


void flashScreenTouch(int xPos, int yPos) {

  // Off button
  if ((xPos >= 10 && xPos <= 120) && (yPos >= 10 && yPos <= 80) && editFlashOnValue == false) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      editFlashOffValue = !editFlashOffValue;

      if (editFlashOffValue == false) {
        tft.fillRoundRect(10, 24, 5, 50, 4, BLACK);
        // update threshold if changed
        if (prevFlashThreshold != flashThreshold) {
          updateValueField("Threshold", WHITE);
        }
      }
      if (editFlashOffValue == true) {
        tft.fillRoundRect(10, 24, 5, 50, 4, YELLOW);
      }

      prevGenericTime = millis();
    }
  }

  // On button
  if ((xPos >= 10 && xPos <= 120) && (yPos >= 90 && yPos <= 160) && editFlashOffValue == false) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      editFlashOnValue = !editFlashOnValue;

      if (editFlashOnValue == false) {
        tft.fillRoundRect(10, 104, 5, 50, 4, BLACK);
        // update threshold if changed
        if (prevFlashThreshold != flashThreshold) {
          updateValueField("Threshold", WHITE);
        }
      }
      if (editFlashOnValue == true) {
        tft.fillRoundRect(10, 104, 5, 50, 4, YELLOW);
      }

      prevGenericTime = millis();
    }
  }

  // Test button
  if ((xPos >= 190 && xPos <= 300) && (yPos >= 100 && yPos <= 150) && editFlashOffValue == false && editFlashOnValue == false) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay * 5) {
      testFlash = !testFlash;

      if (shutterEnabled == false) {
        toggleShutter();
      }
      if (testFlash == true) {
        // draw box
        updateValueField("Test Button", YELLOW);

        // trigger shutter§
        shutterTriggered = triggerShutter();

        // check result
        if (shutterTriggered == false) {
          updateValueField("Test Button", CUSTOM_RED);
        } else if (shutterTriggered == true) {
          updateValueField("Test Button", CUSTOM_GREEN);
        }

        // reset testFlash to off
        testFlash = !testFlash;

      } else if (testFlash == false) {
        updateValueField("Test Button", WHITE);
      }

      prevGenericTime = millis();
    }
  }

  // Back button
  if ((xPos >= 130 && xPos <= 190) && (yPos >= 170 && yPos <= 225) && editFlashOnValue == false && editFlashOffValue == false) {
    startScreen();
  }
}


void manualScreenTouch(int xPos, int yPos) {

  // toggle shutter on/off
  if ((xPos >= 150 && xPos <= 220) && (yPos >= 10 && yPos <= 70)) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      toggleShutter();
      if (shutterEnabled == true) {
        tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_GREEN);
      } else if (shutterEnabled == false) {
        tft.drawBitmap(155, 15, aperture, 50, 50, CUSTOM_RED);
      }

      prevGenericTime = millis();
    }
  }
  // reset manual movement count
  if ((xPos >= 145 && xPos <= 220) && (yPos >= 90 && yPos <= 150)) {
    int16_t x1, y1;
    uint16_t w, h;
    tft.setFont(&Arimo_Bold_30);
    tft.getTextBounds(String(manualMovementCount), 45, 140, &x1, &y1, &w, &h);
    tft.fillRect(x1, y1, w, h, CUSTOM_BLUE);

    manualMovementCount = 0;
    prevManualMovementCount = 0;
    updateValueField("Step Nr", WHITE);
  }
  // step distance button
  if ((xPos >= 5 && xPos <= 130) && (yPos >= 5 && yPos <= 70)) {
    if ((currentTime - prevGenericTime) >= genericTouchDelay) {
      arrowsActive = !arrowsActive;
      editMovementDistance = !editMovementDistance;

      if (arrowsActive == false && editMovementDistance == false) {
        tft.setTextColor(WHITE);
        tft.setCursor(5, 30);
        tft.setFont(&Arimo_Regular_24);
        tft.println("Step Dist.");
        updateValueField("Step Dist", WHITE);
      }
      if (arrowsActive == true && editMovementDistance == true) {
        tft.setTextColor(YELLOW);
        tft.setCursor(5, 30);
        tft.setFont(&Arimo_Regular_24);
        tft.println("Step Dist.");
        updateValueField("Step Dist", YELLOW);
      }

      prevGenericTime = millis();
    }
  }
  // set step size
  if (arrowsActive == true && editMovementDistance == true) {
    if (prevDistance != distancePerMovement) {
      prevDistance = distancePerMovement;
    }

    stepsPerMovement = arrowsTouch(xPos, yPos, 0, stepsPerMovement);
    setStepDistance();
  }
  // step motor
  if (arrowsActive == false) {
    int val = arrowsTouch(xPos, yPos, 1, 0);
  }
  // back button
  if ((xPos >= 150 && xPos <= 210) && (yPos >= 170 && yPos <= 225) && arrowsActive == false) {
    startScreen();
  }
}


void startScreenTouch(int xPos, int yPos) {

  // manual box
  if ((xPos >= 20 && xPos <= 150) && (yPos >= 90 && yPos <= 160)) {
    manualScreen();
  }
  // auto box
  if ((xPos >= 170 && xPos <= 300) && (yPos >= 90 && yPos <= 160)) {
    autoScreen();
  }
  // homing func
  if ((xPos >= 20 && xPos <= 100) && (yPos >= 190 && yPos <= 240)) {
    tft.drawBitmap(30, 190, house, 50, 42, CUSTOM_RED);
    runHomingSequence = true;
    homeRail();
    tft.drawBitmap(30, 190, house, 50, 42, WHITE);
  }
  // change flash value settings
  if ((xPos >= 130 && xPos <= 200) && (yPos >= 200 && yPos <= 240)) {
    flashScreen();
  }
  // rotate screen
  if ((xPos >= 230 && xPos <= 320) && (yPos >= 200 && yPos <= 240)) {
    rotateScreen(); // rotate screen, touch controls, and joystick
    startScreen();
  }
}


void touchScreen() {
  int xPos, yPos;
  currentTime = millis(); // call frequently just in case

  TSPoint point = ts.getPoint();

  if (screenRotated == false) {
    xPos = map(point.y, TS_MINY, TS_MAXY, 0, 320);    // rotate & scale to TFT boundaries
    yPos = map(point.x, TS_MINX, TS_MAXX, 0, 240);   //   ... USB port at upper left
  }
  else if (screenRotated == true) {
    xPos = map(point.y, TS_MINY, TS_MAXY, tft.width(), 0);    // rotate & scale to TFT boundaries
    yPos = map(point.x, TS_MINX, TS_MAXX, tft.height(), 0);   //   ... USB port at lower right
  }

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (point.z > minPressure && point.z < maxPressure) {
    switch (activeScreen) {
      // start screen
      case 1:
        startScreenTouch(xPos, yPos);
        break;
      // manual screen
      case 2:
        manualScreenTouch(xPos, yPos);
        break;
      // autoStack screen
      case 3:
        autoScreenTouch(xPos, yPos);
        break;
      // autoStack config screen
      case 4:
        autoConfigScreenTouch(xPos, yPos);
        break;
      case 5:
        flashScreenTouch(xPos, yPos);
        break;
    }
  }
}
