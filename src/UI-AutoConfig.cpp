#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "StepperControl.h"
#include "UI-Main.h"
#include "UI-AutoConfig.h"

namespace config_screen {
  #define num_btns 11
  #define num_tchs 8
  gfxButton *btn_array[num_btns];
  gfxTouch  *tch_array[num_tchs];

 // TODO implement toggle touch reset

  gfxButton btn_Start         =   gfxB.initButton(       "fillRoundRect",    0,   20,   160,   80,   15, CUSTOM_GREEN );
  gfxButton btn_StartVal      =   gfxB.initButton(       "fillRoundRect",    0,   20,   160,   80,   15, CUSTOM_GREEN );
  gfxButton btn_End           =   gfxB.initButton(       "fillRoundRect",    0,  120,   160,   80,   15, CUSTOM_RED   );
  gfxButton btn_EndVal        =   gfxB.initButton(       "fillRoundRect",    0,  120,   160,   80,   15, CUSTOM_RED   );
  gfxButton btn_Run           =   gfxB.initButton(       "fillRoundRect",    0,  220,   160,   80,   15, CUSTOM_BLUE  );
  gfxButton btn_RunVal        =   gfxB.initButton(       "fillRoundRect",    0,  220,   160,   80,   15, CUSTOM_BLUE  );
  gfxButton btn_Delay         =   gfxB.initBitmapButton( timer,      220,   20,    80,   80,             WHITE        );
  gfxButton btn_DelayVal      =   gfxB.initBlankButton(              295,   20,    40,   30                           );
  // don't add btn_Reset to array as its colour depends on the state of autoStack
  gfxButton btn_Reset         =   gfxB.initBitmapButton( cancel,      220,  120,    80,   80, BLACK          );
  gfxButton btn_Back          =   gfxB.initBitmapButton( backArrow,   220,  220,    80,   80, WHITE         );
  gfxButton btn_ArrowUp       =   gfxB.initBitmapButton( arrowUp,     350,   20,   120,  120, CUSTOM_GREEN  );
  gfxButton btn_ArrowDown     =   gfxB.initBitmapButton( arrowDown,   350,  180,   120,  120, CUSTOM_RED    );
  gfxTouch  tch_Start         =   gfxT.addToggle(    btn_StartVal,  func_Start,     0 );
  gfxTouch  tch_End           =   gfxT.addToggle(    btn_EndVal,    func_End,       0 );
  gfxTouch  tch_Run           =   gfxT.addToggle(    btn_RunVal,    func_Run,       0 );
  gfxTouch  tch_Delay         =   gfxT.addToggle(    btn_Delay,     func_Delay,     0 );
  gfxTouch  tch_Reset         =   gfxT.addMomentary( btn_Reset,     func_Reset,     0 );
  gfxTouch  tch_Back          =   gfxT.addMomentary( btn_Back,      func_Back,      0 );
  gfxTouch  tch_ArrowUp       =   gfxT.addMomentary( btn_ArrowUp,   func_ArrowUp,   0 );
  gfxTouch  tch_ArrowDown     =   gfxT.addMomentary( btn_ArrowDown, func_ArrowDown, 0 );


  void initAutoConfigButtons() {
    btn_array[0] = &btn_Start;
    btn_array[1] = &btn_StartVal;
    btn_array[2] = &btn_End;
    btn_array[3] = &btn_EndVal;
    btn_array[4] = &btn_Run;
    btn_array[5] = &btn_RunVal;
    btn_array[6] = &btn_Delay;
    btn_array[7] = &btn_DelayVal;
    btn_array[8] = &btn_Back;
    btn_array[9] = &btn_ArrowUp;
    btn_array[10] = &btn_ArrowDown;

    tch_array[0] = &tch_Start;
    tch_array[1] = &tch_End;
    tch_array[2] = &tch_Run;
    tch_array[3] = &tch_Delay;
    tch_array[4] = &tch_Reset;
    tch_array[5] = &tch_Back;
    tch_array[6] = &tch_ArrowUp;
    tch_array[7] = &tch_ArrowDown;

    btn_array[1]->addBorder(3, WHITE);
    btn_array[3]->addBorder(3, WHITE);
    btn_array[5]->addBorder(3, WHITE);
  }


  void populateAutoConfigScreen() {
    setCurrentScreen("AutoConfig");

    // draw buttons
    for (int i=0; i < num_btns; i++) {
      btn_array[i]->drawButton(tft);
    }

    if (!autoStackRunning) {
      btn_Reset.drawButton(tft, BLACK);
    }
    else if (autoStackRunning) {
      btn_Reset.drawButton(tft, CUSTOM_RED);
    }

    int currentPosition = driver.XACTUAL();
    // char delaySeconds[6];
    // sprintf_P(delaySeconds, PSTR("%02d:%02d"), 0, shutterDelay);

    // draw text
    btn_Start.writeTextTopCentre(       tft, Arimo_Regular_30,  String("START"),         WHITE);
    btn_StartVal.writeTextBottomCentre( tft, Arimo_Bold_30,     String(startPosition),   WHITE);
    btn_End.writeTextTopCentre(         tft, Arimo_Regular_30,  String("END"),           WHITE);
    btn_EndVal.writeTextBottomCentre(   tft, Arimo_Bold_30,     String(endPosition),     WHITE);
    btn_Run.writeTextTopCentre(         tft, Arimo_Regular_30,  String("RUN"),           WHITE);
    btn_RunVal.writeTextBottomCentre(   tft, Arimo_Bold_30,     String(currentPosition), WHITE);
    btn_DelayVal.writeTextCentre(       tft, Arimo_Bold_30,     String(shutterDelay),    WHITE);
  }


  void checkAutoConfigButtons(int touch_x, int touch_y) {
    for (int i=0; i < num_tchs; i++) {
      tch_array[i]->checkButton("AutoConfig", touch_x, touch_y);
    }
  }


  void func_Start(bool btnActive) {
    if (btnActive && !canEditShutterDelay() && !canEditEndPosition()) {
      setArrowsEnabled(true);
      setEditStartPosition(true);

      btn_Start.writeTextTopCentre(tft, Arimo_Regular_30, String("START"), YELLOW);
      btn_StartVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(startPosition), YELLOW);
    }
    else if (!btnActive && canEditStartPosition()) {
      setArrowsEnabled(false);
      setEditStartPosition(false);

      btn_Start.writeTextTopCentre(tft, Arimo_Regular_30, String("START"), WHITE);
      btn_StartVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(startPosition), WHITE);
    }
  }


  void func_End(bool btnActive) {
    if (btnActive && !canEditShutterDelay() && !canEditStartPosition()) {
      setArrowsEnabled(true);
      setEditEndPosition(true);

      btn_End.writeTextTopCentre(tft, Arimo_Regular_30, String("END"), YELLOW);
      btn_EndVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(endPosition), YELLOW);
    }
    else if (!btnActive && canEditEndPosition()) {
      setArrowsEnabled(false);
      setEditEndPosition(false);

      btn_End.writeTextTopCentre(tft, Arimo_Regular_30, String("END"), WHITE);
      btn_EndVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(endPosition), WHITE);
    }
  }


  void func_Run(bool btnActive) {
    if (btnActive && (canEditShutterDelay() + canEditStartPosition() + canEditEndPosition()) == 0) {
      int currentPosition = driver.XACTUAL();

      btn_Run.writeTextTopCentre(tft, Arimo_Regular_30, String("RUN"), YELLOW);
      btn_RunVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(currentPosition), YELLOW);

      dryRun();

      btn_Run.writeTextTopCentre(tft, Arimo_Regular_30, String("RUN"), WHITE);
      btn_RunVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(currentPosition), WHITE);
      tch_Run.setState(false); // reset button state to false

      displayPosition();
    }
  }


  void func_Delay(bool btnActive) {
    if (btnActive) {
      setArrowsEnabled(true);
      setEditShutterDelay(true);

      btn_Delay.drawButton(tft, YELLOW);
      btn_DelayVal.writeTextCentre(tft, Arimo_Bold_30, String(shutterDelay), YELLOW);
    }
    else if (!btnActive && canEditShutterDelay()) {
      setArrowsEnabled(false);
      setEditShutterDelay(false);

      btn_Delay.drawButton(tft, WHITE);
      btn_DelayVal.writeTextCentre(tft, Arimo_Bold_30, String(shutterDelay), WHITE);
    }
  }


  void func_Reset(bool btnActive) {
    if (btnActive) {
      autoStackRunning = false;
      goToStart = true;
      joystickState = true;
      autoStackPaused = false;
      setNrMovementsCompleted(0);
      setNrMovementsRequired(0);
      startPosition = driver.XACTUAL();
      endPosition = driver.XACTUAL();
      btn_Reset.drawButton(tft, BLACK);
    }
  }


  void func_Back(bool btnActive) {
    if (btnActive && !isArrowsEnabled()) {
      populateScreen("Auto");
    }
  }


  void func_ArrowUp(bool btnActive) {
    if (btnActive && isArrowsEnabled()) {
      // edit start postion
      if (canEditStartPosition()) {
        if (prevStartPosition != startPosition) {
          prevStartPosition = startPosition;
        }
        executeMovement(1, 150); // forward
        setAutoStackStartPosition(); //set start but not end position
      }
      // edit end postion
      else if (canEditEndPosition()) {
        if (prevEndPosition != endPosition) {
          prevEndPosition = endPosition;
        }
         executeMovement(1, 150); // forward
        setAutoStackEndPosition(); //set end but not start position
      }
      // edit shutter delay
      else if (canEditShutterDelay()) {
        if (prevDelay != shutterDelay) {
          prevDelay = shutterDelay;
        }
        shutterDelay++;
        setShutterDelay();
      }
    }
  }


  void func_ArrowDown(bool btnActive) {
    if (btnActive && isArrowsEnabled()) {
      // edit start postion
      if (canEditStartPosition()) {
        if (prevStartPosition != startPosition) {
          prevStartPosition = startPosition;
        }
        executeMovement(-1, 150); // backwards
        setAutoStackStartPosition(); //set start position
      }
      // edit end postion
      else if (canEditEndPosition()) {
        if (prevEndPosition != endPosition) {
          prevEndPosition = endPosition;
        }
        executeMovement(-1, 150); // forward
        setAutoStackEndPosition(); //set end position
      }
      // edit shutter delay
      else if (canEditShutterDelay()) {
        if (prevDelay != shutterDelay) {
          prevDelay = shutterDelay;
        }
        shutterDelay--;
        setShutterDelay();
      }
    }
  }


  void setAutoStackStartPosition() {
    // lower limit
    if (startPosition < 0) {
      startPosition = 0;
    }
    // get new value
    startPosition = driver.XACTUAL();
    // print start point if changed
    if (prevStartPosition != startPosition) {
      btn_Start.writeTextTopCentre(tft, Arimo_Regular_30, String("START"), YELLOW);
      btn_StartVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(startPosition), YELLOW);
      prevStartPosition = startPosition;
    }
    // reset macroStepping
    goToStart = true;

    setMovementsRequired();
  }


  void setAutoStackEndPosition() {
    // set new end value
    if (autoStackMax) {
      endPosition = maxRailPosition;
    }
    else if (!autoStackMax) {
      endPosition = driver.XACTUAL();
    }
    // print end point if changed
    if (prevEndPosition != endPosition) {
      btn_End.writeTextTopCentre(tft, Arimo_Regular_30, String("END"), YELLOW);
      btn_EndVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(endPosition), YELLOW);
      prevEndPosition = endPosition;
    }

  setMovementsRequired();
}


  void setShutterDelay() {

    if (shutterDelay < 1) {
      shutterDelay = 1;
    }
    else if (shutterDelay > 59) {
      shutterDelay = 59;
    }

    if (prevDelay != shutterDelay) {
      // char delaySeconds[6];
      // sprintf_P(delaySeconds, PSTR("%02d"), 0, shutterDelay);
      btn_DelayVal.writeTextCentre(tft, Arimo_Bold_30, String(shutterDelay), YELLOW);
    }
  }


  void displayPosition() {
    int currentPosition = driver.XACTUAL();
    if (getPreviousPosition() != currentPosition) {
      btn_Run.writeTextTopCentre(tft, Arimo_Regular_30, String("RUN"), WHITE);
      btn_RunVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(currentPosition), WHITE);

      setPreviousPosition(currentPosition);
    }
  }

}
