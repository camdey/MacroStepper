#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "ShutterControl.h"
#include "UI-Main.h"
#include "UI-Auto.h"

namespace auto_screen {
  #define num_btns 5
  gfxButton *btn_array[num_btns];


  gfxButton btn_StepSize     =   btn.initButton("Step Size",  "fillRoundRect",  0,  20,   160,  80, 15, CUSTOM_BLUE, true   );
  gfxButton btn_EstTime      =   btn.initButton("Est. Time",  "fillRoundRect",  0,  120,  160,  80, 15, CUSTOM_BLUE,  false );
  gfxButton btn_Progress     =   btn.initButton("Progress",   "fillRoundRect",  0,  220,  160,  80, 15, CUSTOM_BLUE,  false );
  gfxButton btn_Flash        =   btn.initBitmapButton(flashOff, 220,    20,   80, 80, CUSTOM_RED, true  );
  gfxButton btn_Config       =   btn.initBitmapButton(cogWheel, 220,    120,  80, 80, WHITE,      true  );
  gfxButton btn_Back         =   btn.initBitmapButton(backArrow,  220,  220,  80, 80, WHITE,      true  );
  // don't add these buttons to an array as they depend on logic as to which symbol appears and in which colour
  gfxButton btn_PlayPause    =   btn.initBitmapButton(play,       350,  100,  120,  120,  CUSTOM_GREEN, true);
  gfxButton btn_ArrowUp      =   btn.initBitmapButton(arrowUp,    350,  20,   120,  120,  CUSTOM_GREEN, true);
  gfxButton btn_ArrowDown    =   btn.initBitmapButton(arrowDown,  350,  180,  120,  120,  CUSTOM_RED,   true);


  void initAutoButtons() {
    btn_array[0] = &btn_StepSize;
    btn_array[1] = &btn_EstTime;
    btn_array[2] = &btn_Progress;
    btn_array[3] = &btn_Config;
    btn_array[4] = &btn_Back;

    btn_StepSize.addToggle(func_StepDistance,   0 );
    btn_Flash.addToggle(func_Flash,             0 );
    btn_Config.addMomentary(func_Config,        0 );
    btn_Back.addMomentary(func_Back,            0 );
    btn_PlayPause.addToggle(func_PlayPause,     0 );
    btn_ArrowUp.addMomentary(func_ArrowUp,      0 );
    btn_ArrowDown.addMomentary(func_ArrowDown,  0 );

    btn_StepSize.addBorder(3, WHITE);
    btn_EstTime.addBorder(3,  WHITE);
    btn_Progress.addBorder(3, WHITE);
  }


  void populateAutoScreen() {
    setCurrentScreen("Auto");

    // draw buttons
    for (int i=0; i < num_btns; i++) {
      btn_array[i]->drawButton();
    }
    // draw play/pause separately
    if (autoStackPaused && autoStackInitiated) {
      btn_PlayPause.drawButton();
    }
    else if (!autoStackPaused && autoStackInitiated) {
      btn_PlayPause.drawNewBitmap(pause, CUSTOM_BLUE);
    }
    else if (!autoStackPaused && !autoStackInitiated) {
      btn_PlayPause.drawButton();
    }

    if (!isShutterEnabled()) {
      btn_Flash.drawButton();
    }
    else if (isShutterEnabled()) {
      btn_Flash.drawNewBitmap(flashOn, CUSTOM_GREEN);
    }

    // draw text
    btn_StepSize.writeTextTopCentre(Arimo_Regular_30, WHITE);
    btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getStepSize(), 4));
    btn_EstTime.writeTextTopCentre(Arimo_Regular_30, WHITE);
    estimateDuration();
    btn_Progress.writeTextTopCentre(Arimo_Regular_30, WHITE);
    printAutoStackProgress();
  }


  void checkAutoButtons(int touch_x, int touch_y) {
    for (int i=0; i < num_btns; i++) {
      if (btn_array[i]->isTactile()) {
        btn_array[i]->contains(touch_x, touch_y);
      }
    }
    // check play/pause and up/down arrow separately, depending which is active
    if (!isArrowsEnabled()) {
      btn_PlayPause.contains(touch_x, touch_y);
    }
    else if (isArrowsEnabled()) {
      btn_ArrowUp.contains(touch_x, touch_y);
      btn_ArrowDown.contains(touch_x, touch_y);
    }
  }


  void func_StepDistance(bool btnActive) {
    if (btnActive && (!autoStackInitiated || autoStackPaused)) {
      setArrowsEnabled(true);
      setEditMovementDistance(true);

      // clear play/pause button
      btn_PlayPause.drawNewBitmap(play, BLACK);

      btn_ArrowUp.drawButton(CUSTOM_GREEN);
      btn_ArrowDown.drawButton(CUSTOM_RED);

      btn_StepSize.writeTextTopCentre(Arimo_Regular_30, YELLOW);
      btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getStepSize(), 4));
    }
    else if (!btnActive && (!autoStackInitiated || autoStackPaused)) {
      setArrowsEnabled(false);
      setEditMovementDistance(false);

      btn_ArrowUp.drawButton(BLACK);
      btn_ArrowDown.drawButton(BLACK);

      btn_PlayPause.drawNewBitmap(play, CUSTOM_GREEN);

      // TODO would be nice to not re-write the top line on every arrow press
      btn_StepSize.writeTextTopCentre(Arimo_Regular_30, WHITE);
      btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getStepSize(), 4));
    }
    else {
      // set back to off if conditions above not met
      btn_StepSize.setToggleActive(false);
    }
  }


  void func_Flash(bool btnActive) {
    if (!isShutterEnabled()) {
      setShutterEnabled(true);
      btn_Flash.drawNewBitmap(flashOn, CUSTOM_GREEN);
    }
    else if (isShutterEnabled()) {
      setShutterEnabled(false);
      btn_Flash.drawNewBitmap(flashOff, CUSTOM_RED);
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
      btn_PlayPause.drawNewBitmap(pause, CUSTOM_BLUE);
    }
    else if (!btnActive && !isArrowsEnabled()) {
      autoStackPaused = true;  // autoStack paused
      btn_PlayPause.drawNewBitmap(play, CUSTOM_GREEN);
    }
  }


  /***********************************************************************
  Used to pause autostack when needing to be called from another file.
  ***********************************************************************/
  void pauseStack() {
    autoStackPaused = true; // pause autostack
    btn_PlayPause.drawNewBitmap(play, CUSTOM_GREEN);
    btn_PlayPause.setToggleActive(false); // set button state to "pause" mode
  }


  void func_ArrowUp(bool btnActive) {
    if (btnActive && canEditMovementDistance() && isArrowsEnabled()) {
      incrementStepsPerMovement();
      calculateStepSize();
      btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getStepSize(), 4));
    }
  }


  void func_ArrowDown(bool btnActive) {
    if (btnActive && canEditMovementDistance() && isArrowsEnabled()) {
      decrementStepsPerMovement();
      calculateStepSize();
      btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getStepSize(), 4));
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
    btn_EstTime.writeTextBottomCentre(Arimo_Bold_30, WHITE, timeMinutesSeconds);
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
    btn_Progress.writeTextBottomCentre(Arimo_Bold_30, WHITE, autoStackProgress);
  }
}
