#include "TFT-Main.h"
#include "TFT-Home.h"
#include "TFT-Manual.h"
#include "MiscFunctions.h"
#include "ShutterControl.h"
#include "StepperControl.h"
#include "TouchControl.h"
#include "UserInterface.h"
#include "gfxButton.h"

namespace manual_screen {
  #define num_btns_Manual 11
  #define num_tchs_Manual 6
  gfxButton btn_array_Manual[num_btns_Manual];
  gfxTouch  tch_array_Manual[num_tchs_Manual];

  String stepNr   = String(manualMovementCount);
  String railPos  = String(stepper.currentPosition()*(microstepDistance/1000), 5);


  gfxButton btn_StepDistance =   gfxB.initButton(       "Manual", "fillRoundRect",    0,  5,    125,  66,   15, CUSTOM_BLUE  );
  gfxButton btn_DistanceVal  =   gfxB.initButton(       "Manual", "fillRoundRect",    0,  5,    125,  66,   15, CUSTOM_BLUE  );
  gfxButton btn_StepNr       =   gfxB.initButton(       "Manual", "fillRoundRect",    0,  85,   125,  66,   15, CUSTOM_BLUE  );
  gfxButton btn_StepNrVal    =   gfxB.initButton(       "Manual", "fillRoundRect",    0,  85,   125,  66,   15, CUSTOM_BLUE  );
  gfxButton btn_RailPos      =   gfxB.initButton(       "Manual", "fillRoundRect",    0,  165,  125,  66,   15, CUSTOM_BLUE  );
  gfxButton btn_RailPosVal   =   gfxB.initButton(       "Manual", "fillRoundRect",    0,  165,  125,  66,   15, CUSTOM_BLUE  );
  gfxButton btn_Flash        =   gfxB.initBitmapButton( "Manual", aperture,         155,  15,   50,   50,       CUSTOM_RED   );
  gfxButton btn_Reset        =   gfxB.initBitmapButton( "Manual", reset,            155,  95,   50,   50,       WHITE        );
  gfxButton btn_Back         =   gfxB.initBitmapButton( "Manual", backArrow,        155,  175,  50,   50,       WHITE        );
  gfxButton btn_ArrowUp      =   gfxB.initBitmapButton( "Manual", arrowUp,          230,  15,   60,   68,       CUSTOM_GREEN );
  gfxButton btn_ArrowDown    =   gfxB.initBitmapButton( "Manual", arrowDown,        230,  135,  60,   68,       CUSTOM_RED   );
  gfxTouch  tch_StepDistance =   gfxT.addToggle(    btn_DistanceVal,   func_StepDistance,  "btn_DistanceVal",   20 );
  gfxTouch  tch_Flash        =   gfxT.addToggle(    btn_Flash,         func_Flash,         "btn_Flash",         20 );
  gfxTouch  tch_Reset        =   gfxT.addMomentary( btn_Reset,         func_Reset,         "btn_Reset",         20 );
  gfxTouch  tch_Back         =   gfxT.addMomentary( btn_Back,          func_Back,          "btn_Back",          20 );
  gfxTouch  tch_ArrowUp      =   gfxT.addMomentary( btn_ArrowUp,       func_ArrowUp,       "btn_ArrowUp",       20 );
  gfxTouch  tch_ArrowDown    =   gfxT.addMomentary( btn_ArrowDown,     func_ArrowDown,     "btn_ArrowDown",     20 );


  void initManualButtons() {
    btn_array_Manual[0]   = btn_StepDistance;
    btn_array_Manual[1]   = btn_DistanceVal;
    btn_array_Manual[2]   = btn_StepNr;
    btn_array_Manual[3]   = btn_StepNrVal;
    btn_array_Manual[4]   = btn_RailPos;
    btn_array_Manual[5]   = btn_RailPosVal;
    btn_array_Manual[6]   = btn_Flash;
    btn_array_Manual[7]   = btn_Reset;
    btn_array_Manual[8]   = btn_Back;
    btn_array_Manual[9]   = btn_ArrowUp;
    btn_array_Manual[10]  = btn_ArrowDown;

    tch_array_Manual[0] = tch_StepDistance;
    tch_array_Manual[1] = tch_Flash;
    tch_array_Manual[2] = tch_Reset;
    tch_array_Manual[3] = tch_Back;
    tch_array_Manual[4] = tch_ArrowUp;
    tch_array_Manual[5] = tch_ArrowDown;
  }


  void populateManualScreen() {
    setCurrentScreen("Manual");
    stepNr = String(manualMovementCount);
    railPos = String(stepper.currentPosition()*(microstepDistance/1000), 5);

    // draw buttons
    for (int i=0; i < num_btns_Manual; i++) {
      btn_array_Manual[i].drawButton(tft);
    }
    // draw text
    btn_StepDistance.writeTextTopCentre(tft, Arimo_Regular_24, String("Step Dist."),  WHITE);
    btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_24, stepDist,  WHITE);
    btn_StepNr.writeTextTopCentre(tft, Arimo_Regular_24, String("Step Nr."),    WHITE);
    btn_StepNrVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepNr,    WHITE);
    btn_RailPos.writeTextTopCentre(tft, Arimo_Regular_24, String("Rail Pos."),   WHITE);
    btn_RailPosVal.writeTextBottomCentre(tft, Arimo_Bold_24, railPos,   WHITE);
  }


  void checkManualButtons(int touch_x, int touch_y, int touch_z) {
    // if screen pressed
    if (touch_z >= 100 && touch_z <= 1000) {
      for (int i=0; i < num_tchs_Manual; i++) {
        tch_array_Manual[i].checkButton("Manual", touch_x, touch_y);
      }
    }

    // TODO - something better so we don't check the entire array every loop??
    // else if screen not pressed, re-enable toggle
    // else if (touch_z == 0 && resetToggle == true) {
    //   for (int i=0; i < num_btns_Manual; i++) {
    //     if (tch_array_Manual[i].touchType == "toggle") {
    //       tch_array_Manual[i].setTouchReset(true);
    //     }
    //   }
    //   resetToggle = false;
    // }
  }


  void func_StepDistance(bool btnActive) {
    if (btnActive == true) {
      arrowsActive = true;
      editMovementDistance = true;

      btn_StepDistance.writeTextTopCentre(tft, Arimo_Regular_24, String("Step Dist."), YELLOW);
      btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_24, stepDist, YELLOW);
    }
    else {
      arrowsActive = false;
      editMovementDistance = false;

      // TODO would be nice to not re-write the top line on every arrow press
      btn_StepDistance.writeTextTopCentre(tft, Arimo_Regular_24, String("Step Dist."), WHITE);
      btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_24, stepDist, WHITE);
    }
  }


  void func_StepNr() {
    stepNr = String(manualMovementCount); // get latest value
    btn_StepNr.writeTextTopCentre(tft, Arimo_Regular_24, String("Step Dist."), YELLOW);
    btn_StepNrVal.writeTextBottomCentre(tft, Arimo_Bold_24, stepNr, WHITE);
  }


  void func_RailPos() {
    railPos = String(stepper.currentPosition()*(microstepDistance/1000), 5);
    btn_RailPos.writeTextTopCentre(tft, Arimo_Regular_24, String("Step Dist."), YELLOW);
    btn_RailPosVal.writeTextBottomCentre(tft, Arimo_Bold_24, railPos, WHITE);
  }


  void func_Flash(bool btnActive) {
    if (btnActive == true) {
      toggleShutter();
      btn_Flash.drawButton(tft, CUSTOM_GREEN);
    }
    else if (btnActive == false) {
      toggleShutter();
      btn_Flash.drawButton(tft, CUSTOM_RED);
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
        btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_24, stepDist, YELLOW);
      }
      // if not setting step dist., move the stepper forward
      else if (editMovementDistance == false) {
        // take photo if shutter enabled
        if (shutterEnabled == true) {
          shutterTriggered = triggerShutter();
        }
        stepperMoved = stepMotor(1, 500); // forward
        // check void displayPosition() for how we update StepNrVal
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
        btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_24, stepDist, YELLOW);
      }
      // if not setting step dist., move the stepper forward
      else if (editMovementDistance == false) {
        // take photo if shutter enabled
        if (shutterEnabled == true) {
          shutterTriggered = triggerShutter();
        }
        stepperMoved = stepMotor(-1, 500); // reverse
        // check void displayPosition() for how we update StepNrVal
      }
    }
  }
}
