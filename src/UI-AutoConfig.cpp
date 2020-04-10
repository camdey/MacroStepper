#include "UI-Main.h"
#include "UI-AutoConfig.h"
#include "MiscFunctions.h"
#include "StepperControl.h"

namespace config_screen {
  #define num_btns_AutoConfig 11
  #define num_tchs_AutoConfig 8
  gfxButton btn_array_AutoConfig[num_btns_AutoConfig];
  gfxTouch  tch_array_AutoConfig[num_tchs_AutoConfig];


  gfxButton btn_Start         =   gfxB.initBlankButton(  "AutoConfig",    0,    5,  125,  66);
  gfxButton btn_StartVal      =   gfxB.initBlankButton(  "AutoConfig",    0,    5,  125,  66);
  gfxButton btn_End           =   gfxB.initBlankButton(  "AutoConfig",    0,   85,  125,  66);
  gfxButton btn_EndVal        =   gfxB.initBlankButton(  "AutoConfig",    0,   85,  125,  66);
  gfxButton btn_Run           =   gfxB.initBlankButton(  "AutoConfig",    0,  165,  125,  66);
  gfxButton btn_RunVal        =   gfxB.initBlankButton(  "AutoConfig",    0,  165,  125,  66);
  gfxButton btn_DelayVal      =   gfxB.initBlankButton(  "AutoConfig",  155,  125,   60,  30);
  gfxButton btn_Delay         =   gfxB.initBitmapButton( "AutoConfig", delayClock,  160,   75,   50,  50,    WHITE        );
  // don't add btn_Reset to array as its colour depends on the state of autoStack
  gfxButton btn_Reset         =   gfxB.initBitmapButton( "AutoConfig", reset40,     165,   10,   40,  40,    GRAY         );
  gfxButton btn_Back          =   gfxB.initBitmapButton( "AutoConfig", backArrow,   160,  175,   50,  50,    WHITE        );
  gfxButton btn_ArrowUp       =   gfxB.initBitmapButton( "AutoConfig", arrowUp,     240,   26,   60,  68,    CUSTOM_GREEN );
  gfxButton btn_ArrowDown     =   gfxB.initBitmapButton( "AutoConfig", arrowDown,   240,  146,   60,  68,    CUSTOM_RED   );
  gfxTouch  tch_Start         =   gfxT.addToggle(    btn_StartVal,  func_Start,     "btn_StartVal",   20 );
  gfxTouch  tch_End           =   gfxT.addToggle(    btn_EndVal,    func_End,       "btn_EndVal",     20 );
  gfxTouch  tch_Run           =   gfxT.addToggle(    btn_RunVal,    func_Run,       "btn_RunVal",     20 );
  gfxTouch  tch_Delay         =   gfxT.addToggle(    btn_Delay,     func_Delay,     "btn_Delay",      20 );
  gfxTouch  tch_Reset         =   gfxT.addMomentary( btn_Reset,     func_Reset,     "btn_Reset",      20 );
  gfxTouch  tch_Back          =   gfxT.addMomentary( btn_Back,      func_Back,      "btn_Back",       20 );
  gfxTouch  tch_ArrowUp       =   gfxT.addMomentary( btn_ArrowUp,   func_ArrowUp,   "btn_ArrowUp",    20 );
  gfxTouch  tch_ArrowDown     =   gfxT.addMomentary( btn_ArrowDown, func_ArrowDown, "btn_ArrowDown",  20 );


  void initAutoConfigButtons() {
    btn_array_AutoConfig[0]   = btn_Start;
    btn_array_AutoConfig[1]   = btn_StartVal;
    btn_array_AutoConfig[2]   = btn_End;
    btn_array_AutoConfig[3]   = btn_EndVal;
    btn_array_AutoConfig[4]   = btn_Run;
    btn_array_AutoConfig[5]   = btn_RunVal;
    btn_array_AutoConfig[6]   = btn_Delay;
    btn_array_AutoConfig[7]   = btn_DelayVal;
    btn_array_AutoConfig[8]   = btn_Back;
    btn_array_AutoConfig[9]   = btn_ArrowUp;
    btn_array_AutoConfig[10]   = btn_ArrowDown;

    tch_array_AutoConfig[0] = tch_Start;
    tch_array_AutoConfig[1] = tch_End;
    tch_array_AutoConfig[2] = tch_Run;
    tch_array_AutoConfig[3] = tch_Delay;
    tch_array_AutoConfig[4] = tch_Reset;
    tch_array_AutoConfig[5] = tch_Back;
    tch_array_AutoConfig[6] = tch_ArrowUp;
    tch_array_AutoConfig[7] = tch_ArrowDown;
  }


  void populateAutoConfigScreen() {
    setCurrentScreen("AutoConfig");

    // draw buttons
    for (int i=0; i < num_btns_AutoConfig; i++) {
      btn_array_AutoConfig[i].drawButton(tft);
    }

    if (autoStackFlag == false) {
      btn_Reset.drawButton(tft, GRAY);
    }
    else if (autoStackFlag == true) {
      btn_Reset.drawButton(tft, CUSTOM_RED);
    }

    int currentPosition = stepper.currentPosition();
    char delaySeconds[6];
    sprintf_P(delaySeconds, PSTR("%02d:%02d"), 0, shutterDelay);

    // draw text
    btn_Start.writeTextTopCentre(       tft, Lato_Black_34,     String("START"),         CUSTOM_GREEN);
    btn_StartVal.writeTextBottomCentre( tft, Arimo_Bold_20,     String(startPosition),   WHITE);
    btn_End.writeTextTopCentre(         tft, Lato_Black_34,     String("END"),           CUSTOM_RED);
    btn_EndVal.writeTextBottomCentre(   tft, Arimo_Bold_20,     String(endPosition),     WHITE);
    btn_Run.writeTextTopCentre(         tft, Lato_Black_34,     String("RUN"),           CUSTOM_BLUE);
    btn_RunVal.writeTextBottomCentre(   tft, Arimo_Bold_20,     String(currentPosition), WHITE);
    btn_DelayVal.writeTextCentre(       tft, Arimo_Regular_20,  String(delaySeconds),    WHITE);
  }


  void checkAutoConfigButtons(int touch_x, int touch_y, int touch_z) {
    // if screen pressed
    if (touch_z >= 50 && touch_z <= 1000) {
      for (int i=0; i < num_tchs_AutoConfig; i++) {
        tch_array_AutoConfig[i].checkButton("AutoConfig", touch_x, touch_y);
      }
    }
  }


  void func_Start(bool btnActive) {
    if (btnActive == true && editShutterDelay == false && editEndPosition == false) {
      arrowsActive = true;
      editStartPosition = true;

      btn_StartVal.writeTextBottomCentre(tft, Arimo_Bold_20, String(startPosition), YELLOW);
    }
    else if (btnActive == false && editStartPosition == true) {
      arrowsActive = false;
      editStartPosition = false;

      btn_StartVal.writeTextBottomCentre(tft, Arimo_Bold_20, String(startPosition), WHITE);
    }
  }


  void func_End(bool btnActive) {
    if (btnActive == true && editShutterDelay == false && editStartPosition == false) {
      arrowsActive = true;
      editEndPosition = true;

      btn_EndVal.writeTextBottomCentre(tft, Arimo_Bold_20, String(endPosition), YELLOW);
    }
    else if (btnActive == false && editEndPosition == true) {
      arrowsActive = false;
      editEndPosition = false;

      btn_EndVal.writeTextBottomCentre(tft, Arimo_Bold_20, String(endPosition), WHITE);
    }
  }


  void func_Run(bool btnActive) {
    if (btnActive == true && (editShutterDelay + editStartPosition + editEndPosition) == 0) {
      btn_Run.writeTextTopCentre(tft, Lato_Black_34, String("RUN"), YELLOW);
      dryRun();
      btn_Run.writeTextTopCentre(tft, Lato_Black_34, String("RUN"), CUSTOM_BLUE);
      tch_Run.setState(false); // reset button state to false

      displayPosition();
    }
  }


  void func_Delay(bool btnActive) {
    if (btnActive == true) {
      arrowsActive = true;
      editShutterDelay = true;

      btn_Delay.drawButton(tft, YELLOW);
    }
    else if (btnActive == false && editShutterDelay == true) {
      arrowsActive = false;
      editShutterDelay = false;

      btn_Delay.drawButton(tft, WHITE);
    }
  }


  void func_Reset(bool btnActive) {
    if (btnActive == true) {
      autoStackFlag = false;
      goToStart = true;
      joystickState = true;
      pauseAutoStack = false;
      stepCount = 1;
      completedMovements = 0;
      movementsRequired = 0;
      startPosition = stepper.currentPosition();
      endPosition = 0;
      btn_Reset.drawButton(tft, GRAY);
    }
  }


  void func_Back(bool btnActive) {
    if (btnActive == true && arrowsActive == false) {
      populateScreen("Auto");
    }
  }


  void func_ArrowUp(bool btnActive) {
    if (btnActive == true && arrowsActive == true) {
      // edit start postion
      if (editStartPosition == true) {
        if (prevStartPosition != startPosition) {
          prevStartPosition = startPosition;
        }
        stepperMoved = stepMotor(1, 150); // forward
        setAutoStackPositions(true, false); //set start but not end position
      }
      // edit end postion
      else if (editEndPosition == true) {
        if (prevEndPosition != endPosition) {
          prevEndPosition = endPosition;
        }
        stepperMoved = stepMotor(1, 150); // forward
        setAutoStackPositions(false, true); //set end but not start position
      }
      // edit shutter delay
      else if (editShutterDelay == true) {
        if (prevDelay != shutterDelay) {
          prevDelay = shutterDelay;
        }
        shutterDelay++;
        setShutterDelay();
      }
    }
  }


  void func_ArrowDown(bool btnActive) {
    if (btnActive == true && arrowsActive == true) {
      // edit start postion
      if (editStartPosition == true) {
        if (prevStartPosition != startPosition) {
          prevStartPosition = startPosition;
        }
        stepperMoved = stepMotor(-1, 150); // backwards
        setAutoStackPositions(true, false); //set start but not end position
      }
      // edit end postion
      else if (editEndPosition == true) {
        if (prevEndPosition != endPosition) {
          prevEndPosition = endPosition;
        }
        stepperMoved = stepMotor(-1, 150); // forward
        setAutoStackPositions(false, true); //set end but not start position
      }
      // edit shutter delay
      else if (editShutterDelay == true) {
        if (prevDelay != shutterDelay) {
          prevDelay = shutterDelay;
        }
        shutterDelay--;
        setShutterDelay();
      }
    }
  }


  void setAutoStackPositions(bool setStart, bool setEnd) {
    if (setStart == true) {
      // lower limit
      if (startPosition < 0) {
        startPosition = 0;
      }
      // get new value
      startPosition = stepper.currentPosition();
      // print start point if changed
      if (prevStartPosition != startPosition && getCurrentScreen() == "AutoConfig") {
        btn_StartVal.writeTextBottomCentre(tft, Arimo_Regular_20, String(startPosition), YELLOW);
        prevStartPosition = startPosition;
      }
      // reset macroStepping
      goToStart = true;
    }

    if (setEnd == true) {
      // set new end value
      if (autoStackMax == true) {
        endPosition = maxPosition;
      }
      else if (autoStackMax == false) {
        endPosition = stepper.currentPosition();
      }
      // print end point if changed
      if (prevEndPosition != endPosition && getCurrentScreen() == "AutoConfig") {
        btn_EndVal.writeTextBottomCentre(tft, Arimo_Regular_20, String(endPosition), YELLOW);
        prevEndPosition = endPosition;
      }
    }

    setMovementsRequired();
    prevMovementsRequired = movementsRequired;
  }


  void setShutterDelay() {

    if (shutterDelay < 1) {
      shutterDelay = 1;
    }
    else if (shutterDelay > 59) {
      shutterDelay = 59;
    }

    if (prevDelay != shutterDelay) {
      char delaySeconds[6];
      sprintf_P(delaySeconds, PSTR("%02d:%02d"), 0, shutterDelay);
      btn_DelayVal.writeTextCentre(tft, Arimo_Regular_20, String(delaySeconds), WHITE);
    }
  }


  void displayPosition() {
    if (prevStepperPosition != stepper.currentPosition()) {
      int currentPosition = stepper.currentPosition();
      btn_RunVal.writeTextBottomCentre(tft, Arimo_Bold_20, String(currentPosition), WHITE);

      prevStepperPosition = currentPosition;
    }
  }

}
