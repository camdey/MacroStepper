#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "ShutterControl.h"
#include "StepperControl.h"
#include "UI-Main.h"
#include "UI-Manual.h"


namespace manual_screen {
  #define num_btns 10
  #define num_tchs 6
  gfxButton *btn_array[num_btns];
  gfxTouch  *tch_array[num_tchs];

  String stepNr;
  String railPos;
  int movementCount 					= 0;    			// count of manual movements


  gfxButton btn_StepDistance =   gfxB.initButton(       "fillRoundRect",     0,   20,   160,   80,   15, CUSTOM_BLUE  );
  gfxButton btn_DistanceVal  =   gfxB.initButton(       "fillRoundRect",     0,   20,   160,   80,   15, CUSTOM_BLUE  );
  gfxButton btn_StepNr       =   gfxB.initButton(       "fillRoundRect",     0,  120,   160,   80,   15, CUSTOM_BLUE  );
  gfxButton btn_StepNrVal    =   gfxB.initButton(       "fillRoundRect",     0,  120,   160,   80,   15, CUSTOM_BLUE  );
  gfxButton btn_RailPos      =   gfxB.initButton(       "fillRoundRect",     0,  220,   160,   80,   15, CUSTOM_BLUE  );
  gfxButton btn_RailPosVal   =   gfxB.initButton(       "fillRoundRect",     0,  220,   160,   80,   15, CUSTOM_BLUE  );
  // don't add to array as bitmap depends on state
  gfxButton btn_Flash        =   gfxB.initBitmapButton( flashOff,          220,   20,    80,   80,       CUSTOM_RED   );
  gfxButton btn_Reset        =   gfxB.initBitmapButton( cancel,            220,  120,    80,   80,       WHITE        );
  gfxButton btn_Back         =   gfxB.initBitmapButton( backArrow,         220,  220,    80,   80,       WHITE        );
  gfxButton btn_ArrowUp      =   gfxB.initBitmapButton( arrowUp,           350,   20,   120,  120,       CUSTOM_GREEN );
  gfxButton btn_ArrowDown    =   gfxB.initBitmapButton( arrowDown,         350,  180,   120,  120,       CUSTOM_RED   );
  gfxTouch  tch_StepDistance =   gfxT.addToggle(    btn_DistanceVal,  func_StepDistance, 0 );
  gfxTouch  tch_Flash        =   gfxT.addToggle(    btn_Flash,        func_Flash,        0 );
  gfxTouch  tch_Reset        =   gfxT.addMomentary( btn_Reset,        func_Reset,        0 );
  gfxTouch  tch_Back         =   gfxT.addMomentary( btn_Back,         func_Back,         0 );
  gfxTouch  tch_ArrowUp      =   gfxT.addMomentary( btn_ArrowUp,      func_ArrowUp,      0 );
  gfxTouch  tch_ArrowDown    =   gfxT.addMomentary( btn_ArrowDown,    func_ArrowDown,    0 );


  void initManualButtons() {
    btn_array[0] = &btn_StepDistance;
    btn_array[1] = &btn_DistanceVal;
    btn_array[2] = &btn_StepNr;
    btn_array[3] = &btn_StepNrVal;
    btn_array[4] = &btn_RailPos;
    btn_array[5] = &btn_RailPosVal;
    btn_array[6] = &btn_Reset;
    btn_array[7] = &btn_Back;
    btn_array[8] = &btn_ArrowUp;
    btn_array[9] = &btn_ArrowDown;

    tch_array[0] = &tch_StepDistance;
    tch_array[1] = &tch_Flash;
    tch_array[2] = &tch_Reset;
    tch_array[3] = &tch_Back;
    tch_array[4] = &tch_ArrowUp;
    tch_array[5] = &tch_ArrowDown;

    btn_array[1]->addBorder(3, WHITE); //0x43AF
    btn_array[3]->addBorder(3, WHITE);
    btn_array[5]->addBorder(3, WHITE);
  }


  void populateManualScreen() {
    setCurrentScreen("Manual");
    stepNr = String(movementCount);
    railPos = String(driver.XACTUAL()*(microstepLength/1000), 5);

    // draw buttons
    for (int i=0; i < num_btns; i++) {
      btn_array[i]->drawButton(tft);
    }
    if (!isCameraEnabled()) {
      btn_Flash.drawButton(tft, CUSTOM_RED);
    }
    else if (isCameraEnabled()) {
      btn_Flash.drawNewBitmap(tft, flashOn, CUSTOM_GREEN);
    }

    // draw text
    btn_StepDistance.writeTextTopCentre(tft, Arimo_Regular_30, String("Step Size"),  WHITE);
    btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(getStepSize(), 4),  WHITE);
    btn_StepNr.writeTextTopCentre(tft, Arimo_Regular_30, String("Step Nr."),    WHITE);
    btn_StepNrVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepNr,    WHITE);
    btn_RailPos.writeTextTopCentre(tft, Arimo_Regular_30, String("Rail Pos."),   WHITE);
    btn_RailPosVal.writeTextBottomCentre(tft, Arimo_Bold_30, railPos,   WHITE);
  }


  void checkManualButtons(int touch_x, int touch_y) {
    for (int i=0; i < num_tchs; i++) {
      tch_array[i]->checkButton("Manual", touch_x, touch_y);
    }
  }


  void func_StepDistance(bool btnActive) {
    if (btnActive) {
      setArrowsEnabled(true);
      setEditMovementDistance(true);

      btn_StepDistance.writeTextTopCentre(tft, Arimo_Regular_30, String("Step Size"), YELLOW);
      btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(getStepSize(), 4), YELLOW);
    }
    else {
      setArrowsEnabled(false);
      setEditMovementDistance(false);

      // TODO would be nice to not re-write the top line on every arrow press
      btn_StepDistance.writeTextTopCentre(tft, Arimo_Regular_30, String("Step Size"), WHITE);
      btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(getStepSize(), 4), WHITE);
    }
  }


  void func_Flash(bool btnActive) {
    if (btnActive) {
      setCameraEnabled(true);
      btn_Flash.drawNewBitmap(tft, flashOn, CUSTOM_GREEN);
    }
    else if (!btnActive) {
      setCameraEnabled(false);
      // use drawNewButton so previous bitmap is filled over
      btn_Flash.drawNewBitmap(tft, flashOff, CUSTOM_RED);
    }
  }


  void func_Reset(bool btnActive) {
    if (btnActive) {
      movementCount = 0; // reset
      stepNr = String(movementCount); // get latest value
      btn_StepNrVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepNr, WHITE);
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
        btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(getStepSize(), 4), YELLOW);
      }
      // if not setting step size, move the stepper forward
      else if (!canEditMovementDistance()) {
        // take photo if shutter enabled
        if (isCameraEnabled()) {
          shutterTriggered = triggerShutter();
        }
        executeMovement(1, 400); // forward
        if (hasExecutedMovement()) {
          displayPosition();
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
        btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(getStepSize(), 4), YELLOW);
      }
      // if not setting step size, move the stepper forward
      else if (!canEditMovementDistance()) {
        // take photo if shutter enabled
        if (isCameraEnabled()) {
          shutterTriggered = triggerShutter();
        }
        executeMovement(-1, 400); // reverse
        if (hasExecutedMovement()) {
          displayPosition();
        }
      }
    }
  }


  // print new position of rail and movementCount
  // TODO ignore changes to movementCount from joystick motion???
  void displayPosition() {
    // print new position of rail
    railPos = String(driver.XACTUAL()*(microstepLength/1000), 5);
    btn_RailPosVal.writeTextBottomCentre(tft, Arimo_Bold_30, railPos, WHITE);
    // increment movementCount and print to screen
    movementCount++;
    stepNr = String(movementCount);
    btn_StepNrVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepNr, WHITE);
  }

}
