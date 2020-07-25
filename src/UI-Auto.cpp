#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "ShutterControl.h"
#include "UI-Main.h"
#include "UI-Auto.h"

namespace auto_screen {
  #define num_btns 8
  #define num_tchs 4
  gfxButton *btn_array[num_btns];
  gfxTouch  *tch_array[num_tchs];


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
  gfxButton btn_PlayPause    =   gfxB.initBitmapButton( play,             350,  100,   120,  120,       CUSTOM_GREEN );
  gfxButton btn_ArrowUp      =   gfxB.initBitmapButton( arrowUp,          350,   20,   120,  120,       CUSTOM_GREEN );
  gfxButton btn_ArrowDown    =   gfxB.initBitmapButton( arrowDown,        350,  180,   120,  120,       CUSTOM_RED   );
  gfxTouch  tch_StepDistance =   gfxT.addToggle(    btn_DistanceVal,  func_StepDistance,  0 );
  // don't add flash button to array as bitmap depends on state
  gfxTouch  tch_Flash        =   gfxT.addToggle(    btn_Flash,        func_Flash,         0 );
  gfxTouch  tch_Config       =   gfxT.addMomentary( btn_Config,       func_Config,        0 );
  gfxTouch  tch_Back         =   gfxT.addMomentary( btn_Back,         func_Back,          0 );
  gfxTouch  tch_PlayPause    =   gfxT.addToggle(    btn_PlayPause,    func_PlayPause,     0 );
  gfxTouch  tch_ArrowUp      =   gfxT.addMomentary( btn_ArrowUp,      func_ArrowUp,       0 );
  gfxTouch  tch_ArrowDown    =   gfxT.addMomentary( btn_ArrowDown,    func_ArrowDown,     0 );


  void initAutoButtons() {
    btn_array[0] = &btn_StepDistance;
    btn_array[1] = &btn_DistanceVal;
    btn_array[2] = &btn_EstTime;
    btn_array[3] = &btn_EstTimeVal;
    btn_array[4] = &btn_Progress;
    btn_array[5] = &btn_ProgressVal;
    btn_array[6] = &btn_Config;
    btn_array[7] = &btn_Back;

    tch_array[0] = &tch_StepDistance;
    tch_array[1] = &tch_Flash;
    tch_array[2] = &tch_Config;
    tch_array[3] = &tch_Back;

    btn_array[1]->addBorder(3, WHITE);
    btn_array[3]->addBorder(3, WHITE);
    btn_array[5]->addBorder(3, WHITE);
  }


  void populateAutoScreen() {
    setCurrentScreen("Auto");

    // draw buttons
    for (int i=0; i < num_btns; i++) {
      btn_array[i]->drawButton(tft);
    }
    // draw play/pause separately
    if (autoStackPaused && autoStackInitiated) {
      btn_PlayPause.drawButton(tft);
    }
    else if (!autoStackPaused && autoStackInitiated) {
      btn_PlayPause.drawNewBitmap(tft, pause, CUSTOM_BLUE);
    }
    else if (!autoStackPaused && !autoStackInitiated) {
      btn_PlayPause.drawButton(tft);
    }

    if (!isShutterEnabled()) {
      btn_Flash.drawButton(tft);
    }
    else if (isShutterEnabled()) {
      btn_Flash.drawNewBitmap(tft, flashOn, CUSTOM_GREEN);
    }

    // draw text
    btn_StepDistance.writeTextTopCentre(tft, Arimo_Regular_30, String("Step Size"), WHITE);
    btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(getStepSize(), 4), WHITE);
    btn_EstTime.writeTextTopCentre(tft, Arimo_Regular_30, String("Est. Time"), WHITE);
    estimateDuration();
    btn_Progress.writeTextTopCentre(tft, Arimo_Regular_30, String("Progress"), WHITE);
    printAutoStackProgress();
  }


  void checkAutoButtons(int touch_x, int touch_y) {
    for (int i=0; i < num_tchs; i++) {
      tch_array[i]->checkButton("Auto", touch_x, touch_y);
    }
    // check play/pause and up/down arrow separately, depending which is active
    if (!isArrowsEnabled()) {
      tch_PlayPause.checkButton("Auto", touch_x, touch_y);
    }
    else if (isArrowsEnabled()) {
      tch_ArrowUp.checkButton("Auto", touch_x, touch_y);
      tch_ArrowDown.checkButton("Auto", touch_x, touch_y);
    }
  }


  void func_StepDistance(bool btnActive) {
    if (btnActive && (!autoStackInitiated || autoStackPaused)) {
      setArrowsEnabled(true);
      setEditMovementDistance(true);

      // clear play/pause button
      btn_PlayPause.drawNewBitmap(tft, play, BLACK);

      btn_ArrowUp.drawButton(tft, CUSTOM_GREEN);
      btn_ArrowDown.drawButton(tft, CUSTOM_RED);

      btn_StepDistance.writeTextTopCentre(tft, Arimo_Regular_30, String("Step Size"), YELLOW);
      btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(getStepSize(), 4), YELLOW);
    }
    else if (!btnActive && (!autoStackInitiated || autoStackPaused)) {
      setArrowsEnabled(false);
      setEditMovementDistance(false);

      btn_ArrowUp.drawButton(tft, BLACK);
      btn_ArrowDown.drawButton(tft, BLACK);

      btn_PlayPause.drawNewBitmap(tft, play, CUSTOM_GREEN);

      // TODO would be nice to not re-write the top line on every arrow press
      btn_StepDistance.writeTextTopCentre(tft, Arimo_Regular_30, String("Step Size"), WHITE);
      btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(getStepSize(), 4), WHITE);
    }
    else {
      // set back to off if conditions above not met
      tch_StepDistance.setState(false);
    }
  }


  void func_Flash(bool btnActive) {
    if (btnActive) {
      setShutterEnabled(true);
      btn_Flash.drawNewBitmap(tft, flashOn, CUSTOM_GREEN);
    }
    else if (!btnActive) {
      setShutterEnabled(false);
      btn_Flash.drawNewBitmap(tft, flashOff, CUSTOM_RED);
    }
  }


  void func_Config(bool btnActive) {
    if (btnActive && !isArrowsEnabled()) {
      populateScreen("AutoConfig");
    }
  }


  void func_Back(bool btnActive) {
    if (btnActive && !isArrowsEnabled()) {
      populateScreen("Home");
    }
  }


  void func_PlayPause(bool btnActive) {
    if (btnActive && !isArrowsEnabled()) {
      autoStackInitiated = true;   // start autoStack sequence
      autoStackPaused = false;
      btn_PlayPause.drawNewBitmap(tft, pause, CUSTOM_BLUE);
    }
    else if (!btnActive && !isArrowsEnabled()) {
      autoStackPaused = true;  // autoStack paused
      btn_PlayPause.drawNewBitmap(tft, play, CUSTOM_GREEN);
    }
  }


  /***********************************************************************
  Used to pause autostack when needing to be called from another file.
  ***********************************************************************/
  void pauseStack() {
    autoStackPaused = true; // pause autostack
    btn_PlayPause.drawNewBitmap(tft, play, CUSTOM_GREEN);
    tch_PlayPause.setState(false); // set button state to "pause" mode
  }


  void func_ArrowUp(bool btnActive) {
    if (btnActive && canEditMovementDistance() && isArrowsEnabled()) {
      incrementStepsPerMovement();
      calculateStepSize();
      btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(getStepSize(), 4), YELLOW);
    }
  }


  void func_ArrowDown(bool btnActive) {
    if (btnActive && canEditMovementDistance() && isArrowsEnabled()) {
      decrementStepsPerMovement();
      calculateStepSize();
      btn_DistanceVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(getStepSize(), 4), YELLOW);
    }
  }


  /***********************************************************************
  Updates the autoStack screen with est. remaining time of stack sequence.
  Uses sprintf_P function to concatenate minutes and seconds and format
  them as "mm:ss" on the screen.
  ***********************************************************************/
  void estimateDuration() {
    float duration = getNrMovementsRequired() * (getShutterDelay() + (recycleTime/1000));
    float elapsed = getNrMovementsCompleted() * (getShutterDelay() + (recycleTime/1000));
    float remaining = duration - elapsed;
    int minutes = floor(remaining / 60);
    int seconds = int(remaining) % 60;
    char timeMinutesSeconds[6];

    // prevent screen overflow
    minutes = valueCheck(minutes, 0, 299);

    // format time as "mm:ss" string
    sprintf_P(timeMinutesSeconds, PSTR("%02d:%02d"), minutes, seconds);
    // print to screen
    btn_EstTimeVal.writeTextBottomCentre(tft, Arimo_Bold_30, timeMinutesSeconds, WHITE);
  }


  /***********************************************************************
  Updates the autoStack screen with current progress of stack sequence.
  Uses sprintf_P function to concatenate two values and format them as
  "completed / remaining" on the screen.
  ***********************************************************************/
  void printAutoStackProgress() {
    char autoStackProgress[10]  = "0/0";

    // format progress in "Completed / Total" string
    sprintf_P(autoStackProgress, PSTR("%02d/%02d"), getNrMovementsCompleted(), getNrMovementsRequired());
    // print to screen
    btn_ProgressVal.writeTextBottomCentre(tft, Arimo_Bold_30, autoStackProgress, WHITE);
  }
}
