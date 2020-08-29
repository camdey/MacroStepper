#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "ShutterControl.h"
#include "UI-Main.h"
#include "UI-Auto.h"
#include "UI-Global.h"

namespace auto_screen {
  #define num_btns 9
  gfxButton *btn_array[num_btns];


  gfxButton btn_StepSize     =   btn.initButton("Step Size",  "fillRoundRect",  0,  20,   160,  80, 15, DARKGRAY, true  );
  gfxButton btn_EstTime      =   btn.initButton("Est. Time",  "fillRoundRect",  0,  120,  160,  80, 15, DARKGRAY, false );
  gfxButton btn_Progress     =   btn.initButton("Progress",   "fillRoundRect",  0,  220,  160,  80, 15, DARKGRAY, false );
  // gfxButton btn_Flash        =   btn.initBitmapButton(flashOff,   220,  20,   80, 80, CUSTOM_RED, true  ); // added to global buttons
  gfxButton btn_Config       =   btn.initBitmapButton(cogWheel,   220,  120,  80, 80, WHITE,      true  );
  gfxButton btn_Back         =   btn.initBitmapButton(backArrow,  220,  220,  80, 80, WHITE,      true  );
  gfxButton btn_PlayPause    =   btn.initBitmapButton(play,       350,  100,  120,  120,  CUSTOM_GREEN, true);
  gfxButton btn_ArrowUp      =   btn.initBitmapButton(arrowUp,    350,  20,   120,  120,  CUSTOM_GREEN, true);
  gfxButton btn_ArrowDown    =   btn.initBitmapButton(arrowDown,  350,  180,  120,  120,  CUSTOM_RED,   true);


  void initAutoButtons() {
    btn_array[0] = &btn_StepSize;
    btn_array[1] = &btn_EstTime;
    btn_array[2] = &btn_Progress;
    btn_array[3] = &global::btn_Flash;
    btn_array[4] = &btn_Config;
    btn_array[5] = &btn_Back;
    btn_array[6] = &btn_PlayPause;
    btn_array[7] = &btn_ArrowUp;
    btn_array[8] = &btn_ArrowDown;

    btn_StepSize.addToggle(func_StepDistance,   0 );
    btn_Config.addMomentary(func_Config,        0 );
    btn_Back.addMomentary(func_Back,            0 );
    btn_PlayPause.addToggle(func_PlayPause,     0 );
    btn_ArrowUp.addMomentary(func_ArrowUp,      0 );
    btn_ArrowDown.addMomentary(func_ArrowDown,  0 );

    // arrows are disabled by default, only enabled when editing step size
    btn_ArrowUp.hideButton(true);
    btn_ArrowDown.hideButton(true);

    btn_StepSize.addBorder(3, WHITE);
    btn_EstTime.addBorder(3,  WHITE);
    btn_Progress.addBorder(3, WHITE);
  }


  void populateAutoScreen() {
    setCurrentScreen("Auto");

    // draw buttons
    for (int i=0; i < num_btns; i++) {
      if (!btn_array[i]->isHidden()) { // if button is not hidden, draw it
        btn_array[i]->drawButton();
      }
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
  }


  void func_StepDistance(bool btnActive) {
    if (btnActive && (!autoStackInitiated || autoStackPaused)) {
      setArrowsEnabled(true);
      setEditMovementDistance(true);
      hideArrows(false); // show arrows, hide play/pause

      btn_StepSize.writeTextTopCentre(Arimo_Regular_30, YELLOW);
      btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getStepSize(), 4));

      // clear play/pause button
      btn_PlayPause.drawButton(BLACK);

      btn_ArrowUp.drawButton(CUSTOM_GREEN);
      btn_ArrowDown.drawButton(CUSTOM_RED);
    }
    else if (!btnActive && (!autoStackInitiated || autoStackPaused)) {
      setArrowsEnabled(false);
      setEditMovementDistance(false);
      hideArrows(true); // hide arrows, show play/pause

      // TODO would be nice to not re-write the top line on every arrow press
      btn_StepSize.writeTextTopCentre(Arimo_Regular_30, WHITE);
      btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getStepSize(), 4));

      btn_ArrowUp.drawButton(BLACK);
      btn_ArrowDown.drawButton(BLACK);

      btn_PlayPause.drawButton();
    }
    else {
      // set back to off if conditions above not met
      btn_StepSize.setToggleActive(false);
    }
  }


  void func_Config(bool btnActive) {
    if (btnActive && !areArrowsEnabled()) {
      populateScreen("AutoConfig");
    }
  }


  void func_Back(bool btnActive) {
    if (btnActive && !areArrowsEnabled()) {
      populateScreen("Home");
    }
  }


  void func_PlayPause(bool btnActive) {
    if (getNrMovementsRequired() > 0) {
      if (btnActive && !areArrowsEnabled()) {
        autoStackInitiated = true;   // start autoStack sequence
        autoStackPaused = false;
        btn_PlayPause.drawButton(BLACK); // replace existing button
        btn_PlayPause.updateBitmap(pause); // update bitmap image
        btn_PlayPause.updateColour(CUSTOM_BLUE); // change colour
        btn_PlayPause.drawButton(); // draw
      }
      else if (!btnActive && !areArrowsEnabled()) {
        autoStackPaused = true;  // autoStack paused
        btn_PlayPause.drawButton(BLACK); // replace existing button
        btn_PlayPause.updateBitmap(play); // update bitmap image
        btn_PlayPause.updateColour(CUSTOM_GREEN); // change colour
        btn_PlayPause.drawButton(); // draw
      }
    }
    else {
      btn_PlayPause.setToggleActive(!btnActive); // reset button state
    }
  }


  /***********************************************************************
  Used to pause autostack when needing to be called from another file.
  ***********************************************************************/
  void pauseStack() {
    autoStackPaused = true; // pause autostack
    btn_PlayPause.drawButton(BLACK); // replace existing button
    btn_PlayPause.updateBitmap(play); // update bitmap image
    btn_PlayPause.updateColour(CUSTOM_GREEN); // update color
    btn_PlayPause.drawButton(); // draw button
    btn_PlayPause.setToggleActive(false); // set button state to "pause" mode
  }


  void func_ArrowUp(bool btnActive) {
    if (btnActive && canEditMovementDistance() && areArrowsEnabled()) {
      incrementStepsPerMovement();
      calculateStepSize();
      btn_StepSize.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getStepSize(), 4));
    }
  }


  void func_ArrowDown(bool btnActive) {
    if (btnActive && canEditMovementDistance() && areArrowsEnabled()) {
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


  void hideArrows(bool hide) {
    btn_ArrowUp.hideButton(hide);
    btn_ArrowDown.hideButton(hide);
    btn_PlayPause.hideButton(!hide); // PlayPause takes opposite state to arrow buttons
  }
}
