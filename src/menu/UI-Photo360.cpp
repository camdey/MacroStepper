#include "GlobalVariables.h"
#include "StepperControl.h"
#include "menu/UI-Main.h"
#include "menu/UI-Photo360.h"
#include "menu/UI-Global.h"

namespace photo_screen {
  #define num_btns 9
  gfxButton *btn_array[num_btns];
  bool canEditPhotoNr     = false; // flag for editing photoNr with arrows
  bool canEditPhotoDelay  = false; // flag for editing photoDelay with arrows


  gfxButton btn_PhotoNr       =   btn.initButton("Nr Photos",   "fillRoundRect",  0,    20,   160,  80,   15, DARKGRAY,   true  );
  gfxButton btn_Delay         =   btn.initButton("Delay",       "fillRoundRect",  0,    120,  160,  80,   15, DARKGRAY,   true  );
  gfxButton btn_Progress      =   btn.initButton("Progress",    "fillRoundRect",  0,    220,  160,  80,   15, DARKGRAY,   false );
  gfxButton btn_Config        =   btn.initBitmapButton(cogWheel,   200,  120,  80,    80, WHITE,        true  );
  gfxButton btn_Back          =   btn.initBitmapButton(backArrow,  200,  220,  80,    80, WHITE,        true  );
  gfxButton btn_PlayPause     =   btn.initBitmapButton(play,       350,  100,  120,  120, CUSTOM_GREEN, true  );
  gfxButton btn_ArrowUp       =   btn.initBitmapButton(arrowUp,    350,  20,   120,  120, CUSTOM_GREEN, true  );
  gfxButton btn_ArrowDown     =   btn.initBitmapButton(arrowDown,  350,  180,  120,  120, CUSTOM_RED,   true  );


  void initPhoto360Buttons() {
    btn_array[0] = &btn_PhotoNr;
    btn_array[1] = &btn_Delay;
    btn_array[2] = &btn_Progress;
    btn_array[3] = &global::btn_Flash;
    btn_array[4] = &btn_Config;
    btn_array[5] = &btn_Back;
    btn_array[6] = &btn_PlayPause;
    btn_array[7] = &btn_ArrowUp;
    btn_array[8] = &btn_ArrowDown;

    btn_PhotoNr.addToggle(func_PhotoNr,         0 );
    btn_Delay.addToggle(func_Delay,             0 );
    btn_Config.addMomentary(func_Config,        0 );
    btn_Back.addMomentary(func_Back,            0 );
    btn_PlayPause.addToggle(func_PlayPause,     0 );
    btn_ArrowUp.addMomentary(func_ArrowUp,      0 );
    btn_ArrowDown.addMomentary(func_ArrowDown,  0 );

    // arrows are disabled by default, only enabled when editing step size
    btn_ArrowUp.hideButton(true);
    btn_ArrowDown.hideButton(true);

    btn_PhotoNr.addBorder(3, WHITE);
    btn_Delay.addBorder(3,  WHITE);
    btn_Progress.addBorder(3, WHITE);
  }


  void populatePhoto360Screen() {
    setCurrentScreen("Photo360");
    setJoystickMaxVelocity(5000); // lower joystick speed

    // draw buttons
    for (int i=0; i < num_btns; i++) {
      if (!btn_array[i]->isHidden()) { // if button is not hidden, draw it
        btn_array[i]->drawButton();
      }
    }

    // draw text
    btn_PhotoNr.writeTextTopCentre(Arimo_Regular_30, WHITE);
    btn_PhotoNr.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getNr360Photos()));
    btn_Delay.writeTextTopCentre(Arimo_Regular_30, WHITE);
    btn_Delay.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getPhoto360Delay()));
    btn_Progress.writeTextTopCentre(Arimo_Regular_30, WHITE);
    printPhoto360Progress();
  }


  void checkPhoto360Buttons(int touch_x, int touch_y) {
    for (int i=0; i < num_btns; i++) {
      if (btn_array[i]->isTactile()) {
        btn_array[i]->contains(touch_x, touch_y);
      }
    }
  }


  // set number of photos required for photo360 procedure
  void func_PhotoNr(bool btnActive) {
    // don't allow changing mid-procedure
    if (btnActive && !photo360Initiated && !canEditPhotoDelay) {
      setArrowsEnabled(true);
      canEditPhotoNr = true;
      hideArrows(false); // show arrows, hide play/pause

      btn_PhotoNr.writeTextTopCentre(Arimo_Regular_30, YELLOW);
      btn_PhotoNr.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getNr360Photos()));

      // clear play/pause button
      btn_PlayPause.drawButton(BLACK);
      btn_ArrowUp.drawButton(CUSTOM_GREEN);
      btn_ArrowDown.drawButton(CUSTOM_RED);
    }
    else if (!btnActive && !photo360Initiated && !canEditPhotoDelay) {
      setArrowsEnabled(false);
      canEditPhotoNr = false;
      hideArrows(true); // hide arrows, show play/pause

      // TODO would be nice to not re-write the top line on every arrow press
      btn_PhotoNr.writeTextTopCentre(Arimo_Regular_30, WHITE);
      btn_PhotoNr.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getNr360Photos()));

      btn_ArrowUp.drawButton(BLACK);
      btn_ArrowDown.drawButton(BLACK);
      btn_PlayPause.drawButton();
    }
    else {
      // set back to off if conditions above not met
      btn_PhotoNr.setToggleActive(false);
    }
  }


  // set delay between photos for photo360 procedure
  void func_Delay(bool btnActive) {
    // don't allow changing unless paused or not started
    if (btnActive && (!photo360Initiated || photo360Paused) && !canEditPhotoNr) {
      setArrowsEnabled(true);
      canEditPhotoDelay = true;
      hideArrows(false); // show arrows, hide play/pause

      btn_Delay.writeTextTopCentre(Arimo_Regular_30, YELLOW);
      btn_Delay.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getPhoto360Delay()));

      // clear play/pause button
      btn_PlayPause.drawButton(BLACK);
      btn_ArrowUp.drawButton(CUSTOM_GREEN);
      btn_ArrowDown.drawButton(CUSTOM_RED);
    }
    else if (!btnActive && (!photo360Initiated || photo360Paused) && !canEditPhotoNr) {
      setArrowsEnabled(false);
      canEditPhotoDelay = false;
      hideArrows(true); // hide arrows, show play/pause

      btn_Delay.writeTextTopCentre(Arimo_Regular_30, WHITE);
      btn_Delay.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getPhoto360Delay()));

      btn_ArrowUp.drawButton(BLACK);
      btn_ArrowDown.drawButton(BLACK);
      btn_PlayPause.drawButton();
    }
    else {
      // set back to off if conditions above not met
      btn_Delay.setToggleActive(false);
    }
  }


  void func_Config(bool btnActive) {
    if (btnActive && !areArrowsEnabled()) {
      populateScreen("Photo360Config");
    }
  }


  void func_Back(bool btnActive) {
    if (btnActive && !areArrowsEnabled()) {
      populateScreen("Orbis");
    }
  }


  void func_PlayPause(bool btnActive) {
    if (btnActive && !areArrowsEnabled()) {
      if (isNewPhoto360) {
        setNrCompleted360Photos(0);
        setCurrentStage(start);
      }
      photo360Initiated = true;   // start photo360 sequence
      photo360Paused = false;
      btn_PlayPause.drawButton(BLACK); // replace existing button
      btn_PlayPause.updateBitmap(pause); // update bitmap image
      btn_PlayPause.updateColour(CUSTOM_BLUE); // change colour
      btn_PlayPause.drawButton(); // draw
      photo360(stepper2);
    }
    else if (!btnActive && !areArrowsEnabled()) {
      photo360Paused = true;  // photo360 paused
      btn_PlayPause.drawButton(BLACK); // replace existing button
      btn_PlayPause.updateBitmap(play); // update bitmap image
      btn_PlayPause.updateColour(CUSTOM_GREEN); // change colour
      btn_PlayPause.drawButton(); // draw
    }
    else {
      btn_PlayPause.setToggleActive(!btnActive); // reset button state
    }
  }


  /***********************************************************************
  Used to pause photo360 when needing to be called from another file.
  ***********************************************************************/
  void pausePhoto360() {
    // photo360Paused = true; // pause photo360
    btn_PlayPause.drawButton(BLACK); // replace existing button
    btn_PlayPause.updateBitmap(play); // update bitmap image
    btn_PlayPause.updateColour(CUSTOM_GREEN); // update color
    btn_PlayPause.drawButton(); // draw button
    // btn_PlayPause.setToggleActive(false); // set button state to "pause" mode
  }


  /***********************************************************************
  Used to reset photo360 after completed 360.
  ***********************************************************************/
  void resetPhoto360() {
    photo360Paused = false;
    photo360Initiated = false;
    isNewPhoto360 = true;
    setNrCompleted360Photos(0);
    setCurrentStage(idle);
    // setTargetVelocity(STEALTH_CHOP_VMAX);
    btn_PlayPause.drawButton(BLACK); // replace existing button
    btn_PlayPause.updateBitmap(play); // update to show play button
    btn_PlayPause.updateColour(CUSTOM_GREEN); // update color
    btn_PlayPause.drawButton(); // draw button
    btn_PlayPause.setButtonActive(false);
  }


  void func_ArrowUp(bool btnActive) {
    if (btnActive && areArrowsEnabled()) {
      if (canEditPhotoNr) {
        incrementNr360Photos();
        setNrCompleted360Photos(0); // reset in case adjusting after previous run
        printPhoto360Progress();
        btn_PhotoNr.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getNr360Photos()));
      }
      else if (canEditPhotoDelay) {
        setPhoto360Delay(getPhoto360Delay()+100);
        btn_Delay.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getPhoto360Delay()));
      }
    }
  }


  void func_ArrowDown(bool btnActive) {
    if (btnActive && areArrowsEnabled()) {
      if (canEditPhotoNr) {
        decrementNr360Photos();
        setNrCompleted360Photos(0); // reset in case adjusting after previous run
        printPhoto360Progress();
        btn_PhotoNr.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getNr360Photos()));
      }
      else if (canEditPhotoDelay) {
        setPhoto360Delay(getPhoto360Delay()-100);
        btn_Delay.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getPhoto360Delay()));
      }
    }
  }


  /***********************************************************************
  Updates the photo360 screen with current progress of 360 sequence.
  Uses sprintf_P function to concatenate two values and format them as
  "completed / remaining" on the screen.
  ***********************************************************************/
  void printPhoto360Progress() {
    char photo360Progress[10]  = "0/0";

    // format progress in "Completed / Total" string
    sprintf_P(photo360Progress, PSTR("%02d/%02d"), getNrCompleted360Photos(), getNr360Photos());
    // print to screen
    btn_Progress.writeTextBottomCentre(Arimo_Bold_30, WHITE, photo360Progress);
  }


  void hideArrows(bool hide) {
    btn_ArrowUp.hideButton(hide);
    btn_ArrowDown.hideButton(hide);
    btn_PlayPause.hideButton(!hide); // PlayPause takes opposite state to arrow buttons
  }
}
