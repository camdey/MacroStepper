#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "StepperControl.h"
#include "UI-Main.h"
#include "UI-AutoConfig.h"

namespace config_screen {
  #define num_btns 8
  gfxButton *btn_array[num_btns];


  gfxButton btn_Start         =   btn.initButton("START", "fillRoundRect",  0,  20,   160,  80, 15, CUSTOM_GREEN, true  );
  gfxButton btn_End           =   btn.initButton("END",   "fillRoundRect",  0,  120,  160,  80, 15, CUSTOM_RED,   true  );
  gfxButton btn_Run           =   btn.initButton("RUN",   "fillRoundRect",  0,  220,  160,  80, 15, CUSTOM_BLUE,  true  );
  gfxButton btn_Delay         =   btn.initBitmapButton(timer,     220,  20,   80,   80,   WHITE,        true  );
  gfxButton btn_DelayVal      =   btn.initTransparentButton(      295,  20,   40,   30,                 false );
  gfxButton btn_Reset         =   btn.initBitmapButton(cancel,    220,  120,  80,   80,   BLACK,        true  ); // don't add btn_Reset to array as its colour depends on the state of autoStack
  gfxButton btn_Back          =   btn.initBitmapButton(backArrow, 220,  220,  80,   80,   WHITE,        true  );
  gfxButton btn_ArrowUp       =   btn.initBitmapButton(arrowUp,   350,  20,   120,  120,  CUSTOM_GREEN, true  );
  gfxButton btn_ArrowDown     =   btn.initBitmapButton(arrowDown, 350,  180,  120,  120,  CUSTOM_RED,   true  );


  void initAutoConfigButtons() {
    btn_array[0] = &btn_Start;
    btn_array[1] = &btn_End;
    btn_array[2] = &btn_Run;
    btn_array[3] = &btn_Delay;
    btn_array[4] = &btn_DelayVal;
    btn_array[5] = &btn_Back;
    btn_array[6] = &btn_ArrowUp;
    btn_array[7] = &btn_ArrowDown;

    btn_Start.addToggle(func_Start,             0 );
    btn_End.addToggle(func_End,                 0 );
    btn_Run.addToggle(func_Run,                 0 );
    btn_Delay.addToggle(func_Delay,             0 );
    btn_Reset.addMomentary(func_Reset,          0 );
    btn_Back.addMomentary(func_Back,            0 );
    btn_ArrowUp.addMomentary(func_ArrowUp,      0 );
    btn_ArrowDown.addMomentary(func_ArrowDown,  0 );

    btn_Start.addBorder(3, WHITE);
    btn_End.addBorder(3, WHITE);
    btn_Run.addBorder(3, WHITE);
  }


  void populateAutoConfigScreen() {
    setCurrentScreen("AutoConfig");

    // draw buttons
    for (int i=0; i < num_btns; i++) {
      btn_array[i]->drawButton();
    }

    if (!autoStackInitiated) {
      btn_Reset.drawButton(BLACK);
    }
    else if (autoStackInitiated) {
      btn_Reset.drawButton(CUSTOM_RED);
    }

    int currentPosition = driver.XACTUAL();

    // draw text
    btn_Start.writeTextTopCentre(Arimo_Regular_30,  WHITE);
    btn_Start.writeTextBottomCentre(Arimo_Bold_30,  WHITE,  String(getStartPosition()));
    btn_End.writeTextTopCentre(Arimo_Regular_30,    WHITE);
    btn_End.writeTextBottomCentre(Arimo_Bold_30,    WHITE,  String(getEndPosition()));
    btn_Run.writeTextTopCentre(Arimo_Regular_30,    WHITE);
    btn_Run.writeTextBottomCentre(Arimo_Bold_30,    WHITE,  String(currentPosition));
    btn_DelayVal.writeTextCentre(Arimo_Bold_30,     WHITE,  String(getShutterDelay()));
  }


  void checkAutoConfigButtons(int touch_x, int touch_y) {
    for (int i=0; i < num_btns; i++) {
      if (btn_array[i]->isTactile()) {
        btn_array[i]->contains(touch_x, touch_y);
      }
    }
  }


  void func_Start(bool btnActive) {
    if (btnActive && !canEditShutterDelay() && !canEditEndPosition()) {
      setArrowsEnabled(true);
      setEditStartPosition(true);
      setStartPosition(driver.XACTUAL()); // set start position to current position

      btn_Start.writeTextTopCentre(Arimo_Regular_30, YELLOW);
      btn_Start.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getStartPosition()));
    }
    else if (!btnActive && canEditStartPosition()) {
      setArrowsEnabled(false);
      setEditStartPosition(false);

      btn_Start.writeTextTopCentre(Arimo_Regular_30, WHITE);
      btn_Start.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getStartPosition()));
    }
  }


  void func_End(bool btnActive) {
    if (btnActive && !canEditShutterDelay() && !canEditStartPosition()) {
      setArrowsEnabled(true);
      setEditEndPosition(true);
      setEndPosition(driver.XACTUAL()); // set end position to current position

      btn_End.writeTextTopCentre(Arimo_Regular_30, YELLOW);
      btn_End.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getEndPosition()));
    }
    else if (!btnActive && canEditEndPosition()) {
      setArrowsEnabled(false);
      setEditEndPosition(false);

      btn_End.writeTextTopCentre(Arimo_Regular_30, WHITE);
      btn_End.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getEndPosition()));
    }
  }


  void func_Run(bool btnActive) {
    if (btnActive && (canEditShutterDelay() + canEditStartPosition() + canEditEndPosition()) == 0) {
      int currentPosition = driver.XACTUAL();

      btn_Run.writeTextTopCentre(Arimo_Regular_30, YELLOW);
      btn_Run.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(currentPosition));

      dryRun();

      btn_Run.writeTextTopCentre(Arimo_Regular_30, WHITE);
      btn_Run.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(currentPosition));
      btn_Run.setToggleActive(false); // reset button state to false
    }
  }


  void func_Delay(bool btnActive) {
    if (btnActive) {
      setArrowsEnabled(true);
      setEditShutterDelay(true);

      btn_Delay.drawButton(YELLOW);
      btn_DelayVal.writeTextCentre(Arimo_Bold_30, YELLOW, String(getShutterDelay()));
    }
    else if (!btnActive && canEditShutterDelay()) {
      setArrowsEnabled(false);
      setEditShutterDelay(false);

      btn_Delay.drawButton(WHITE);
      btn_DelayVal.writeTextCentre(Arimo_Bold_30, WHITE, String(getShutterDelay()));
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
      btn_Reset.drawButton(BLACK);
    }
  }


  // called when back arrow is pressed
  void func_Back(bool btnActive) {
    if (btnActive && !areArrowsEnabled()) {
      populateScreen("Auto");
    }
  }


  // called when up arrow is pressed, either updates start/end or shutterDelay
  void func_ArrowUp(bool btnActive) {
    if (btnActive && areArrowsEnabled()) {
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
    if (btnActive && areArrowsEnabled()) {
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
    btn_Start.writeTextTopCentre(Arimo_Regular_30, YELLOW);
    btn_Start.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getStartPosition()));
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
    btn_End.writeTextTopCentre(Arimo_Regular_30, YELLOW);
    btn_End.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getEndPosition()));
  }


  // print the new shutterDelay value to screen
  void printShutterDelay() {
    btn_DelayVal.writeTextCentre(Arimo_Bold_30, YELLOW, String(getShutterDelay()));
  }


  // print current position of stepper under RUN text
  void printPosition() {
    btn_Run.writeTextTopCentre(Arimo_Regular_30, WHITE);
    btn_Run.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(driver.XACTUAL()));
  }
}
