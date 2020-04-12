#include "UI-Main.h"
#include "UI-Auto.h"
#include "MiscFunctions.h"
#include "ShutterControl.h"

namespace auto_screen {
  #define num_btns_Auto 8
  #define num_tchs_Auto 4
  gfxButton btn_array_Auto[num_btns_Auto];
  gfxTouch  tch_array_Auto[num_tchs_Auto];


  gfxButton btn_StepDistance =   gfxB.initButton(       "fillRoundRect",    0,   20,   160,   80,   15, CUSTOM_BLUE  );
  gfxButton btn_DistanceVal  =   gfxB.initButton(       "fillRoundRect",    0,   20,   160,   80,   15, CUSTOM_BLUE  );
  gfxButton btn_EstTime      =   gfxB.initButton(       "fillRoundRect",    0,  120,   160,   80,   15, CUSTOM_BLUE  );
  gfxButton btn_EstTimeVal   =   gfxB.initButton(       "fillRoundRect",    0,  120,   160,   80,   15, CUSTOM_BLUE  );
  gfxButton btn_Progress     =   gfxB.initButton(       "fillRoundRect",    0,  220,   160,   80,   15, CUSTOM_BLUE  );
  gfxButton btn_ProgressVal  =   gfxB.initButton(       "fillRoundRect",    0,  220,   160,   80,   15, CUSTOM_BLUE  );
  gfxButton btn_Flash        =   gfxB.initBitmapButton( flashOff,         220,   20,    80,   80,       CUSTOM_RED   );
  gfxButton btn_Config       =   gfxB.initBitmapButton( cogWheel,         220,  120,    80,   80,       WHITE        );
  gfxButton btn_Back         =   gfxB.initBitmapButton( backArrow,        220,  220,    80,   80,       WHITE        );
  // don't add these buttons to an array as they depend on logic as to which symbol appears and in which colour
  gfxButton btn_Play         =   gfxB.initBitmapButton( play,             350,   20,   120,  120,       CUSTOM_GREEN );
  gfxButton btn_Pause        =   gfxB.initBitmapButton( pause,            350,  180,   120,  120,       CUSTOM_BLUE  );
  gfxButton btn_ArrowUp      =   gfxB.initBitmapButton( arrowUp,          350,   20,   120,  120,       CUSTOM_GREEN );
  gfxButton btn_ArrowDown    =   gfxB.initBitmapButton( arrowDown,        350,  180,   120,  120,       CUSTOM_RED   );
  gfxTouch  tch_StepDistance =   gfxT.addToggle(    btn_DistanceVal,  func_StepDistance,  20 );
  // don't add flash button to array as bitmap depends on state
  gfxTouch  tch_Flash        =   gfxT.addToggle(    btn_Flash,        func_Flash,         20 );
  gfxTouch  tch_Config       =   gfxT.addMomentary( btn_Config,       func_Config,        20 );
  gfxTouch  tch_Back         =   gfxT.addMomentary( btn_Back,         func_Back,          20 );
  gfxTouch  tch_Play         =   gfxT.addToggle(    btn_Play,         func_Play,          20 );
  gfxTouch  tch_Pause        =   gfxT.addToggle(    btn_Pause,        func_Pause,         20 );
  gfxTouch  tch_ArrowUp      =   gfxT.addMomentary( btn_ArrowUp,      func_ArrowUp,       20 );
  gfxTouch  tch_ArrowDown    =   gfxT.addMomentary( btn_ArrowDown,    func_ArrowDown,     20 );


  void initAutoButtons() {
    btn_array_Auto[0]   = btn_StepDistance;
    btn_array_Auto[1]   = btn_DistanceVal;
    btn_array_Auto[1].addBorder(3, WHITE);
    btn_array_Auto[2]   = btn_EstTime;
    btn_array_Auto[3]   = btn_EstTimeVal;
    btn_array_Auto[3].addBorder(3, WHITE);
    btn_array_Auto[4]   = btn_Progress;
    btn_array_Auto[5]   = btn_ProgressVal;
    btn_array_Auto[5].addBorder(3, WHITE);
    btn_array_Auto[6]   = btn_Config;
    btn_array_Auto[7]   = btn_Back;

    tch_array_Auto[0] = tch_StepDistance;
    tch_array_Auto[1] = tch_Flash;
    tch_array_Auto[2] = tch_Config;
    tch_array_Auto[3] = tch_Back;
  }


  void populateAutoScreen() {
    setCurrentScreen("Auto");

    // draw buttons
    for (int i=0; i < num_btns_Auto; i++) {
      btn_array_Auto[i].drawButton(tft);
    }
    // draw play/pause separately
    if (pauseAutoStack == true && autoStackFlag == true) {
      drawPlayPause(false, true);
    } else if (pauseAutoStack == false && autoStackFlag == true) {
      drawPlayPause(true, false);
    } else if (pauseAutoStack == false && autoStackFlag == false) {
      drawPlayPause(false, false);
    }

    if (shutterEnabled == false) {
      btn_Flash.drawButton(tft, CUSTOM_RED);
    }
    else if (shutterEnabled == true) {
      btn_Flash.drawNewBitmap(tft, flashOn, CUSTOM_GREEN);
    }

    // draw text
    btn_StepDistance.writeTextTopCentre(tft, Arimo_Regular_30, String("Step Dist."), WHITE);
    btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepDist, WHITE);
    btn_EstTime.writeTextTopCentre(tft, Arimo_Regular_30, String("Est. Time"), WHITE);
    estimateDuration(true);
    btn_Progress.writeTextTopCentre(tft, Arimo_Regular_30, String("Progress"), WHITE);
    updateProgress(true);
  }


  void checkAutoButtons(int touch_x, int touch_y, int touch_z) {
    // if screen pressed
    if (touch_z >= 50 && touch_z <= 1000) {
      for (int i=0; i < num_tchs_Auto; i++) {
        tch_array_Auto[i].checkButton("Auto", touch_x, touch_y);
      }
      // check play/pause and up/down arrow separately, depending which is active
      if (arrowsActive == false) {
        tch_Play.checkButton("Auto", touch_x, touch_y);
        tch_Pause.checkButton("Auto", touch_x, touch_y);
      } else if (arrowsActive == true) {
        tch_ArrowUp.checkButton("Auto", touch_x, touch_y);
        tch_ArrowDown.checkButton("Auto", touch_x, touch_y);
      }
    }
  }


  void func_StepDistance(bool btnActive) {
    if (btnActive == true) {
      arrowsActive = true;
      editMovementDistance = true;
      // clear play/pause buttons
      btn_Play.drawButton(tft, BLACK);
      btn_Pause.drawButton(tft, BLACK);

      btn_ArrowUp.drawButton(tft, CUSTOM_GREEN);
      btn_ArrowDown.drawButton(tft, CUSTOM_RED);

      // btn_StepDistance.writeTextTopCentre(tft, Arimo_Regular_30, String("Step Dist."), YELLOW);
      // btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepDist, YELLOW);
    }
    else {
      arrowsActive = false;
      editMovementDistance = false;

      btn_ArrowUp.drawButton(tft, BLACK);
      btn_ArrowDown.drawButton(tft, BLACK);

      btn_Play.drawButton(tft, CUSTOM_GREEN);
      btn_Pause.drawButton(tft, CUSTOM_BLUE);

      // TODO would be nice to not re-write the top line on every arrow press
      // btn_StepDistance.writeTextTopCentre(tft, Arimo_Regular_30, String("Step Dist."), WHITE);
      // btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepDist, WHITE);
    }
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


  void func_Config(bool btnActive) {
    if (btnActive == true && arrowsActive == false) {
      populateScreen("AutoConfig");
    }
  }


  void func_Back(bool btnActive) {
    if (btnActive == true && arrowsActive == false) {
      populateScreen("Home");
    }
  }


  void func_Play(bool btnActive) {
    if (btnActive == true) {
      btn_Play.drawButton(tft, CUSTOM_GREY);
    }
    else {
      btn_Play.drawButton(tft, CUSTOM_GREEN);
    }
  }


  void func_Pause(bool btnActive) {
    if (btnActive == true) {
      btn_Pause.drawButton(tft, CUSTOM_GREY);
    }
    else {
      btn_Pause.drawButton(tft, CUSTOM_BLUE);
    }
  }


  void func_ArrowUp(bool btnActive) {
    if (btnActive == true && editMovementDistance == true && arrowsActive == true) {
      if (prevDistance != distancePerMovement) {
        prevDistance = distancePerMovement;
      }

      stepsPerMovement++; // increment
      setStepDistance();
      btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepDist, YELLOW);
    }
  }


  void func_ArrowDown(bool btnActive) {
    if (btnActive == true && editMovementDistance == true && arrowsActive == true) {
      if (prevDistance != distancePerMovement) {
        prevDistance = distancePerMovement;
      }

      stepsPerMovement--; // decrement
      setStepDistance();
      btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, stepDist, YELLOW);
    }
  }


  /***********************************************************************
  Draws the Play/Pause buttons in the AutoStack screen of the controller
  ***********************************************************************/
  void drawPlayPause(bool greyPlay = false, bool greyPause = false) {
    if (greyPlay == true) {
      btn_Play.drawButton(tft, CUSTOM_GREY);
    } else {
      btn_Play.drawButton(tft, CUSTOM_GREEN);
    }

    if (greyPause == true) {
      btn_Pause.drawButton(tft, CUSTOM_GREY);
    } else {
      btn_Pause.drawButton(tft, CUSTOM_BLUE);
    }
  }


  /***********************************************************************
  Updates the autoStack screen with est. remaining time of stack sequence.
  Uses sprintf_P function to concatenate minutes and seconds and format
  them as "mm:ss" on the screen.
  ***********************************************************************/
  void estimateDuration(bool screenRefresh) {
    float duration = movementsRequired * (shutterDelay + (recycleTime/1000));
    float elapsed = completedMovements * (shutterDelay + (recycleTime/1000));
    float remaining = duration - elapsed;
    int minutes = floor(remaining / 60);
    int seconds = int(remaining) % 60;
    char timeMinutesSeconds[6];

    // prevent screen overflow
    minutes = valueCheck(minutes, 0, 299);

    sprintf_P(timeMinutesSeconds, PSTR("%02d:%02d"), minutes, seconds);

    if ((minutes != prevMinutes || seconds != prevSeconds || screenRefresh == 1) && getCurrentScreen() == "Auto") {
      btn_EstTimeVal.writeTextBottomCentre(tft, Arimo_Bold_30, timeMinutesSeconds, WHITE);

      prevMinutes = minutes;
      prevSeconds = seconds;
      // assign new time to prev variable
      // sprintf_P(prevTimeMinutesSeconds, PSTR("%02d:%02d"), minutes, seconds);
    }
  }


  /***********************************************************************
  Updates the autoStack screen with current progress of stack sequence.
  Uses sprintf_P function to concatenate two values and format them as
  "completed / remaining" on the screen.
  ***********************************************************************/
  void updateProgress(bool screenRefresh) {
    char autoStackProgress[10]  = "0/0";

    // displays progress in "Completed / Total" format
    sprintf_P(autoStackProgress, PSTR("%02d/%02d"), completedMovements, movementsRequired);

    if ((completedMovements != prevCompletedMovements) || movementsRequired != prevMovementsRequired || screenRefresh == 1) {
      btn_ProgressVal.writeTextBottomCentre(tft, Arimo_Bold_30, autoStackProgress, WHITE);

      prevMovementsRequired = movementsRequired;
      prevCompletedMovements = completedMovements;
      // assign new time to prev variable
      // sprintf_P(prevAutoStackProgress, PSTR("%02d/%02d"), completedMovements, movementsRequired);
    }
  }

}
