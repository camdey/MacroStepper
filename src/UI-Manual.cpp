#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "ShutterControl.h"
#include "StepperControl.h"
#include "UI-Main.h"
#include "UI-Manual.h"


namespace manual_screen {
  #define num_btns 8
  gfxButton *btn_array[num_btns];
  String stepNr;
  String railPos;
  int movementCount = 0;  // count of manual movements


  gfxButton btn_StepSize    =   btn.initButton("Step Size", "fillRoundRect", 0,   20,  160,  80,  15,  DARKGRAY, true  );
  gfxButton btn_StepNr      =   btn.initButton("Step Nr.",  "fillRoundRect", 0,  120,  160,  80,  15,  DARKGRAY, false );
  gfxButton btn_RailPos     =   btn.initButton("Rail Pos.", "fillRoundRect", 0,  220,  160,  80,  15,  DARKGRAY, false );
  gfxButton btn_Flash       =   btn.initBitmapButton(flashOff,   220,  20,   80,   80,   CUSTOM_RED,   true  );
  gfxButton btn_Reset       =   btn.initBitmapButton(cancel,     220,  120,  80,   80,   WHITE,        true  );
  gfxButton btn_Back        =   btn.initBitmapButton(backArrow,  220,  220,  80,   80,   WHITE,        true  );
  gfxButton btn_ArrowUp     =   btn.initBitmapButton(arrowUp,    350,  20,   120,  120,  CUSTOM_GREEN, true  );
  gfxButton btn_ArrowDown   =   btn.initBitmapButton(arrowDown,  350,  180,  120,  120,  CUSTOM_RED,   true  );


  void initManualButtons() {
    btn_array[0] = &btn_StepSize;
    btn_array[1] = &btn_Flash;
    btn_array[2] = &btn_ArrowUp;
    btn_array[3] = &btn_StepNr;
    btn_array[4] = &btn_Reset;
    btn_array[5] = &btn_RailPos;
    btn_array[6] = &btn_Back;
    btn_array[7] = &btn_ArrowDown;

    btn_StepSize.addToggle(func_StepDistance, 0);
    btn_Flash.addToggle(func_Flash, 0);
    btn_Reset.addMomentary(func_Reset, 0);
    btn_Back.addMomentary(func_Back, 0);
    btn_ArrowUp.addMomentary(func_ArrowUp, 0);
    btn_ArrowDown.addMomentary(func_ArrowDown, 0);

    btn_StepSize.addBorder(3, WHITE);
    btn_StepNr.addBorder(3, WHITE);
    btn_RailPos.addBorder(3, WHITE);
  }


  void populateManualScreen() {
    setCurrentScreen("Manual");
    stepNr  = String(movementCount);
    railPos = String(driver.XACTUAL()*(microstepLength/1000), 5);

    // draw buttons
    for (int i=0; i < num_btns; i++) {
      btn_array[i]->drawButton();
    }
    if (!isShutterEnabled()) {
      btn_Flash.drawButton(CUSTOM_RED);
    }
    else if (isShutterEnabled()) {
      btn_Flash.drawNewBitmap(flashOn, CUSTOM_GREEN);
    }
    // draw text
    btn_StepSize.writeTextTopCentre(Arimo_Regular_30, WHITE);
    btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getStepSize(), 4));
    btn_StepNr.writeTextTopCentre(Arimo_Regular_30, WHITE);
    btn_StepNr.writeTextBottomCentre(Arimo_Bold_30, WHITE, stepNr);
    btn_RailPos.writeTextTopCentre(Arimo_Regular_30, WHITE);
    btn_RailPos.writeTextBottomCentre(Arimo_Bold_30, WHITE, railPos);
  }


  void checkManualButtons(int touch_x, int touch_y) {
    for (int i=0; i < num_btns; i++) {
      if (btn_array[i]->isTactile()) {
        btn_array[i]->contains(touch_x, touch_y);
      }
    }
  }


  void func_StepDistance(bool btnActive) {
    if (btnActive) {
      setArrowsEnabled(true);
      setEditMovementDistance(true);

      btn_StepSize.writeTextTopCentre(Arimo_Regular_30, YELLOW);
      btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getStepSize(), 4));
    }
    else {
      setArrowsEnabled(false);
      setEditMovementDistance(false);

      // TODO would be nice to not re-write the top line on every arrow press
      btn_StepSize.writeTextTopCentre(Arimo_Regular_30, WHITE);
      btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getStepSize(), 4));
    }
  }


  void func_Flash(bool btnActive) {
    if (!isShutterEnabled()) {
      setShutterEnabled(true);
      btn_Flash.drawNewBitmap(flashOn, CUSTOM_GREEN);
    }
    else if (isShutterEnabled()) {
      setShutterEnabled(false);
      // use drawNewButton so previous bitmap is filled over
      btn_Flash.drawNewBitmap(flashOff, CUSTOM_RED);
    }
  }


  void func_Reset(bool btnActive) {
    if (btnActive) {
      movementCount = 0; // reset
      stepNr = String(movementCount); // get latest value
      btn_StepNr.writeTextBottomCentre(Arimo_Bold_30, WHITE, stepNr);
    }
  }


  void func_Back(bool btnActive) {
    if (btnActive && !isArrowsEnabled()) {
      populateScreen("Home");
    }
  }


  void func_ArrowUp(bool btnActive) {
    if (btnActive) {
      // if setting step size
      if (canEditMovementDistance() && isArrowsEnabled()) {
        incrementStepsPerMovement();
        calculateStepSize();
        btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getStepSize(), 4));
      }
      // if not setting step size, move the stepper forward
      else if (!canEditMovementDistance()) {
        // take photo if shutter enabled
        if (isShutterEnabled()) {
          triggerShutter();
        }
        executeMovement(1, 400); // forward
        if (hasExecutedMovement()) {
          printPosition();
          updateMovementCount();
        }
      }
    }
  }


  void func_ArrowDown(bool btnActive) {
    if (btnActive) {
      // if setting step size
      if (canEditMovementDistance() && isArrowsEnabled()) {
        decrementStepsPerMovement();
        calculateStepSize();
        btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getStepSize(), 4));
      }
      // if not setting step size, move the stepper forward
      else if (!canEditMovementDistance()) {
        // take photo if shutter enabled
        if (isShutterEnabled()) {
          triggerShutter();
        }
        executeMovement(-1, 400); // reverse
        if (hasExecutedMovement()) {
          printPosition();
          updateMovementCount();
        }
      }
    }
  }


  // print new position of rail
  void printPosition() {
    // print new position of rail
    railPos = String(driver.XACTUAL()*(microstepLength/1000), 5);
    btn_RailPos.writeTextBottomCentre(Arimo_Bold_30, WHITE, railPos);
  }


  // print new movementCount
  void updateMovementCount() {
    // increment movementCount and print to screen
    movementCount++;
    stepNr = String(movementCount);
    btn_StepNr.writeTextBottomCentre(Arimo_Bold_30, WHITE, stepNr);
  }

}
