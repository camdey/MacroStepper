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

    if (!autoStackInitiated) {
      btn_Reset.drawButton(tft, BLACK);
    }
    else if (autoStackInitiated) {
      btn_Reset.drawButton(tft, CUSTOM_RED);
    }

    int currentPosition = driver.XACTUAL();

    // draw text
    btn_Start.writeTextTopCentre(       tft, Arimo_Regular_30,  String("START"),         WHITE);
    btn_StartVal.writeTextBottomCentre( tft, Arimo_Bold_30,     String(getStartPosition()),   WHITE);
    btn_End.writeTextTopCentre(         tft, Arimo_Regular_30,  String("END"),           WHITE);
    btn_EndVal.writeTextBottomCentre(   tft, Arimo_Bold_30,     String(getEndPosition()),     WHITE);
    btn_Run.writeTextTopCentre(         tft, Arimo_Regular_30,  String("RUN"),           WHITE);
    btn_RunVal.writeTextBottomCentre(   tft, Arimo_Bold_30,     String(currentPosition), WHITE);
    btn_DelayVal.writeTextCentre(       tft, Arimo_Bold_30,     String(getShutterDelay()),    WHITE);
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
      setStartPosition(driver.XACTUAL()); // set start position to current position

      btn_Start.writeTextTopCentre(tft, Arimo_Regular_30, String("START"), YELLOW);
      btn_StartVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(getStartPosition()), YELLOW);
    }
    else if (!btnActive && canEditStartPosition()) {
      setArrowsEnabled(false);
      setEditStartPosition(false);

      btn_Start.writeTextTopCentre(tft, Arimo_Regular_30, String("START"), WHITE);
      btn_StartVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(getStartPosition()), WHITE);
    }
  }


  void func_End(bool btnActive) {
    if (btnActive && !canEditShutterDelay() && !canEditStartPosition()) {
      setArrowsEnabled(true);
      setEditEndPosition(true);
      setEndPosition(driver.XACTUAL()); // set end position to current position

      btn_End.writeTextTopCentre(tft, Arimo_Regular_30, String("END"), YELLOW);
      btn_EndVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(getEndPosition()), YELLOW);
    }
    else if (!btnActive && canEditEndPosition()) {
      setArrowsEnabled(false);
      setEditEndPosition(false);

      btn_End.writeTextTopCentre(tft, Arimo_Regular_30, String("END"), WHITE);
      btn_EndVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(getEndPosition()), WHITE);
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
    }
  }


  void func_Delay(bool btnActive) {
    if (btnActive) {
      setArrowsEnabled(true);
      setEditShutterDelay(true);

      btn_Delay.drawButton(tft, YELLOW);
      btn_DelayVal.writeTextCentre(tft, Arimo_Bold_30, String(getShutterDelay()), YELLOW);
    }
    else if (!btnActive && canEditShutterDelay()) {
      setArrowsEnabled(false);
      setEditShutterDelay(false);

      btn_Delay.drawButton(tft, WHITE);
      btn_DelayVal.writeTextCentre(tft, Arimo_Bold_30, String(getShutterDelay()), WHITE);
    }
  }


  // reset AutoStack procedure to default values
  void func_Reset(bool btnActive) {
    if (btnActive) {
      autoStackInitiated = false;
      isNewAutoStack = true;
      autoStackPaused = false;
      setNrMovementsCompleted(0);
      setNrMovementsRequired(0);
      setStartPosition(driver.XACTUAL());
      setEndPosition(driver.XACTUAL());
      btn_Reset.drawButton(tft, BLACK);
    }
  }


  // called when back arrow is pressed
  void func_Back(bool btnActive) {
    if (btnActive && !isArrowsEnabled()) {
      populateScreen("Auto");
    }
  }


  // called when up arrow is pressed, either updates start/end or shutterDelay
  void func_ArrowUp(bool btnActive) {
    if (btnActive && isArrowsEnabled()) {
      // edit start postion
      if (canEditStartPosition()) {
        executeMovement(1, 150); // forward
        updateStartPosition(); //set start but not end position
        printPosition(); // update rail positon
      }
      // edit end postion
      else if (canEditEndPosition()) {
        executeMovement(1, 150); // forward
        updateEndPosition(); //set end but not start position
        printPosition(); // update rail positon
      }
      // edit shutter delay
      else if (canEditShutterDelay()) {
        incrementShutterDelay();
        printShutterDelay();
      }
    }
  }


  // called when down arrow is pressed, either updates start/end or shutterDelay
  void func_ArrowDown(bool btnActive) {
    if (btnActive && isArrowsEnabled()) {
      // edit start postion
      if (canEditStartPosition()) {
        executeMovement(-1, 150); // backwards
        updateStartPosition(); //set start position
        printPosition(); // update rail positon
      }
      // edit end postion
      else if (canEditEndPosition()) {
        executeMovement(-1, 150); // forward
        updateEndPosition(); //set end position
        printPosition(); // update rail positon
      }
      // edit shutter delay
      else if (canEditShutterDelay()) {
        decrementShutterDelay();
        printShutterDelay();
      }
    }
  }


  // sets and prints new Start position for AutoStack
  void updateStartPosition() {
    // lower limit
    if (getStartPosition() < 0) {
      setStartPosition(0);
    }
    // get new value
    setStartPosition(driver.XACTUAL());
    btn_Start.writeTextTopCentre(tft, Arimo_Regular_30, String("START"), YELLOW);
    btn_StartVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(getStartPosition()), YELLOW);
    // reset AutoStack
    isNewAutoStack = true;
  }


  // sets and prints new End position for AutoStack
  void updateEndPosition() {
    // set new end value
    if (autoStackMax) {
      setEndPosition(maxRailPosition);
    }
    else if (!autoStackMax) {
      setEndPosition(driver.XACTUAL());
    }
    btn_End.writeTextTopCentre(tft, Arimo_Regular_30, String("END"), YELLOW);
    btn_EndVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(getEndPosition()), YELLOW);
  }


  // print the new shutterDelay value to screen
  void printShutterDelay() {
    btn_DelayVal.writeTextCentre(tft, Arimo_Bold_30, String(getShutterDelay()), YELLOW);
  }


  // print current position of stepper under RUN text
  void printPosition() {
    btn_Run.writeTextTopCentre(tft, Arimo_Regular_30, String("RUN"), WHITE);
    btn_RunVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(driver.XACTUAL()), WHITE);
  }
}
