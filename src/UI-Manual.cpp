#include "UI-Main.h"
#include "UI-Manual.h"
#include "MiscFunctions.h"
#include "ShutterControl.h"
#include "StepperControl.h"

namespace manual_screen {
  #define num_btns_Manual 11
  #define num_tchs_Manual 6
  gfxButton btn_array_Manual[num_btns_Manual];
  gfxTouch  tch_array_Manual[num_tchs_Manual];

  String stepNr   = String(manualMovementCount);
  String railPos  = String(stepper.currentPosition()*(microstepDistance/1000), 5);


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
  gfxTouch  tch_StepDistance =   gfxT.addToggle(    btn_DistanceVal,  func_StepDistance, 20 );
  gfxTouch  tch_Flash        =   gfxT.addToggle(    btn_Flash,        func_Flash,        20 );
  gfxTouch  tch_Reset        =   gfxT.addMomentary( btn_Reset,        func_Reset,        20 );
  gfxTouch  tch_Back         =   gfxT.addMomentary( btn_Back,         func_Back,         20 );
  gfxTouch  tch_ArrowUp      =   gfxT.addMomentary( btn_ArrowUp,      func_ArrowUp,      20 );
  gfxTouch  tch_ArrowDown    =   gfxT.addMomentary( btn_ArrowDown,    func_ArrowDown,    20 );


  void initManualButtons() {
    btn_array_Manual[0]   = btn_StepDistance;
    btn_array_Manual[1]   = btn_DistanceVal;
    btn_array_Manual[1].addBorder(3, WHITE); //0x43AF
    btn_array_Manual[2]   = btn_StepNr;
    btn_array_Manual[3]   = btn_StepNrVal;
    btn_array_Manual[3].addBorder(3, WHITE);
    btn_array_Manual[4]   = btn_RailPos;
    btn_array_Manual[5]   = btn_RailPosVal;
    btn_array_Manual[5].addBorder(3, WHITE);
    // btn_array_Manual[6]   = btn_Flash;
    btn_array_Manual[6]   = btn_Reset;
    btn_array_Manual[7]   = btn_Back;
    btn_array_Manual[8]   = btn_ArrowUp;
    btn_array_Manual[9]   = btn_ArrowDown;

    tch_array_Manual[0]   = tch_StepDistance;
    tch_array_Manual[1]   = tch_Flash;
    tch_array_Manual[2]   = tch_Reset;
    tch_array_Manual[3]   = tch_Back;
    tch_array_Manual[4]   = tch_ArrowUp;
    tch_array_Manual[5]   = tch_ArrowDown;
  }


  void populateManualScreen() {
    setCurrentScreen("Manual");
    stepNr = String(manualMovementCount);
    railPos = String(stepper.currentPosition()*(microstepDistance/1000), 5);

    // draw buttons
    for (int i=0; i < num_btns_Manual; i++) {
      btn_array_Manual[i].drawButton(tft);
    }
    if (shutterEnabled == false) {
      btn_Flash.drawButton(tft, CUSTOM_RED);
    }
    else if (shutterEnabled == true) {
      btn_Flash.drawNewBitmap(tft, flashOn, CUSTOM_GREEN);
    }

    // tft.drawRoundRect(0, 20, 160, 80, 15, WHITE);
    // tft.drawRoundRect(1, 21, 158, 78, 14, WHITE);
    // tft.drawRoundRect(2, 22, 156, 76, 13, WHITE);

    // draw text
    btn_StepDistance.writeTextTopCentre(tft, Arimo_Regular_30, String("Step Dist."),  WHITE);
    btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepDist,  WHITE);
    btn_StepNr.writeTextTopCentre(tft, Arimo_Regular_30, String("Step Nr."),    WHITE);
    btn_StepNrVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepNr,    WHITE);
    btn_RailPos.writeTextTopCentre(tft, Arimo_Regular_30, String("Rail Pos."),   WHITE);
    btn_RailPosVal.writeTextBottomCentre(tft, Arimo_Bold_30, railPos,   WHITE);
  }


  void checkManualButtons(int touch_x, int touch_y, int touch_z) {
    // if screen pressed
    if (touch_z >= 50 && touch_z <= 1000) {
      for (int i=0; i < num_tchs_Manual; i++) {
        tch_array_Manual[i].checkButton("Manual", touch_x, touch_y);
      }
    }
  }


  void func_StepDistance(bool btnActive) {
    if (btnActive == true) {
      arrowsActive = true;
      editMovementDistance = true;

      btn_StepDistance.writeTextTopCentre(tft, Arimo_Regular_30, String("Step Dist."), YELLOW);
      btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepDist, YELLOW);
    }
    else {
      arrowsActive = false;
      editMovementDistance = false;

      // TODO would be nice to not re-write the top line on every arrow press
      btn_StepDistance.writeTextTopCentre(tft, Arimo_Regular_30, String("Step Dist."), WHITE);
      btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepDist, WHITE);
    }
  }


  void func_Flash(bool btnActive) {
    if (btnActive == true) {
      toggleShutter();
      btn_Flash.drawNewBitmap(tft, flashOn, CUSTOM_GREEN);
    }
    else if (btnActive == false) {
      toggleShutter();
      // use drawNewButton so previous bitmap is filled over
      btn_Flash.drawNewBitmap(tft, flashOff, CUSTOM_RED);
    }
  }


  void func_Reset(bool btnActive) {
    if (btnActive == true) {
      manualMovementCount = 0; // reset
      prevManualMovementCount = 0; // reset
      stepNr = String(manualMovementCount); // get latest value
      btn_StepNrVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepNr, WHITE);
    }
  }


  void func_Back(bool btnActive) {
    if (btnActive == true && arrowsActive == false) {
      populateScreen("Home");
    }
  }


  void func_ArrowUp(bool btnActive) {
    if (btnActive == true) {
      // if setting step dist.
      if (editMovementDistance == true && arrowsActive == true) {
        if (prevDistance != distancePerMovement) {
          prevDistance = distancePerMovement;
        }

        stepsPerMovement++; // increment
        setStepDistance();
        btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepDist, YELLOW);
      }
      // if not setting step dist., move the stepper forward
      else if (editMovementDistance == false) {
        // take photo if shutter enabled
        if (shutterEnabled == true) {
          shutterTriggered = triggerShutter();
        }
        stepperMoved = stepMotor(1, 200); // forward
        displayPosition();
      }
    }
  }


  void func_ArrowDown(bool btnActive) {
    if (btnActive == true) {
      // if setting step dist.
      if (editMovementDistance == true && arrowsActive == true) {
        if (prevDistance != distancePerMovement) {
          prevDistance = distancePerMovement;
        }

        stepsPerMovement--; // decrement
        setStepDistance();
        btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepDist, YELLOW);
      }
      // if not setting step dist., move the stepper forward
      else if (editMovementDistance == false) {
        // take photo if shutter enabled
        if (shutterEnabled == true) {
          shutterTriggered = triggerShutter();
        }
        stepperMoved = stepMotor(-1, 500); // reverse
        displayPosition();
      }
    }
  }


  void displayPosition() {
    // update for new values
    if (prevStepperPosition != stepper.currentPosition()) {
      int currentPosition = stepper.currentPosition();
      railPos = String(currentPosition*(microstepDistance/1000), 5);
      // update rail position value
      btn_RailPosVal.writeTextBottomCentre(tft, Arimo_Bold_30, railPos, WHITE);


      manualMovementCount++;
      stepNr = String(manualMovementCount);
      // update movement count for manual screen
      btn_StepNrVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepNr, WHITE);

      prevManualMovementCount = manualMovementCount;
      prevStepperPosition = currentPosition;
    }
  }

}
