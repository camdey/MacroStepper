#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "ShutterControl.h"
#include "menu/UI-Main.h"
#include "menu/UI-Photo360.h"
#include "menu/UI-Global.h"

namespace photo_screen {
  #define num_btns 9
  gfxButton *btn_array[num_btns];


  gfxButton btn_PhotoNr       =   btn.initButton("Nr Photos",   "fillRoundRect",  0,    20,   160,  80,   15, DARKGRAY,   true  );
  gfxButton btn_Delay         =   btn.initButton("Delay",       "fillRoundRect",  0,    120,  160,  80,   15, DARKGRAY,   false );
  gfxButton btn_Progress      =   btn.initButton("Progress",    "fillRoundRect",  0,    220,  160,  80,   15, DARKGRAY,   false );
  gfxButton btn_Config        =   btn.initBitmapButton(cogWheel,   220,  120,  80,    80, WHITE,        true  );
  gfxButton btn_Back          =   btn.initBitmapButton(backArrow,  220,  220,  80,    80, WHITE,        true  );
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


  void func_PhotoNr(bool btnActive) {
    if (btnActive && (!autoStackInitiated || autoStackPaused)) {
      setArrowsEnabled(true);
      // setEditPhotoNr(true);
      hideArrows(false); // show arrows, hide play/pause

      btn_PhotoNr.writeTextTopCentre(Arimo_Regular_30, YELLOW);
      btn_PhotoNr.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getNr360Photos()));

      // clear play/pause button
      btn_PlayPause.drawButton(BLACK);
      btn_ArrowUp.drawButton(CUSTOM_GREEN);
      btn_ArrowDown.drawButton(CUSTOM_RED);
    }
    else if (!btnActive && (!autoStackInitiated || autoStackPaused)) {
      setArrowsEnabled(false);
      // setEditPhotoNr(false);
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


    void func_Delay(bool btnActive) {
    if (btnActive && (!autoStackInitiated || autoStackPaused)) {
      setArrowsEnabled(true);
      // setEditPhotoNr(true);
      hideArrows(false); // show arrows, hide play/pause

      // btn_Delay.writeTextTopCentre(Arimo_Regular_30, YELLOW);
      // btn_Delay.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getNr360Photos()));

      // // clear play/pause button
      // btn_PlayPause.drawButton(BLACK);
      // btn_ArrowUp.drawButton(CUSTOM_GREEN);
      // btn_ArrowDown.drawButton(CUSTOM_RED);
    }
    else if (!btnActive && (!autoStackInitiated || autoStackPaused)) {
      setArrowsEnabled(false);
      // setEditPhotoNr(false);
      hideArrows(true); // hide arrows, show play/pause

      // TODO would be nice to not re-write the top line on every arrow press
      // btn_Delay.writeTextTopCentre(Arimo_Regular_30, WHITE);
      // btn_Delay.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getNr360Photos()));

      // btn_ArrowUp.drawButton(BLACK);
      // btn_ArrowDown.drawButton(BLACK);
      // btn_PlayPause.drawButton();
    }
    else {
      // set back to off if conditions above not met
      btn_Delay.setToggleActive(false);
    }
  }


  void func_Config(bool btnActive) {
    if (btnActive && !areArrowsEnabled()) {
      populateScreen("PhotoConfig");
    }
  }


  void func_Back(bool btnActive) {
    if (btnActive && !areArrowsEnabled()) {
      populateScreen("Orbis");
    }
  }


  void func_PlayPause(bool btnActive) {
    // if (getNrMovementsRequired() > 0) {
    //   if (btnActive && !areArrowsEnabled()) {
    //     autoStackInitiated = true;   // start autoStack sequence
    //     autoStackPaused = false;
    //     btn_PlayPause.drawButton(BLACK); // replace existing button
    //     btn_PlayPause.updateBitmap(pause); // update bitmap image
    //     btn_PlayPause.updateColour(CUSTOM_BLUE); // change colour
    //     btn_PlayPause.drawButton(); // draw
    //     // btn_StackStatus.writeTextCentre(Arimo_Bold_20, WHITE);
    //   }
    //   else if (!btnActive && !areArrowsEnabled()) {
    //     autoStackPaused = true;  // autoStack paused
    //     btn_PlayPause.drawButton(BLACK); // replace existing button
    //     btn_PlayPause.updateBitmap(play); // update bitmap image
    //     btn_PlayPause.updateColour(CUSTOM_GREEN); // change colour
    //     btn_PlayPause.drawButton(); // draw
    //     // btn_StackStatus.writeTextCentre(Arimo_Bold_20, WHITE);
    //   }
    // }
    // else {
    //   btn_PlayPause.setToggleActive(!btnActive); // reset button state
    // }
  }


  /***********************************************************************
  Used to pause autostack when needing to be called from another file.
  ***********************************************************************/
  void pausePhoto360() {
    // autoStackPaused = true; // pause autostack
    btn_PlayPause.drawButton(BLACK); // replace existing button
    btn_PlayPause.updateBitmap(play); // update bitmap image
    btn_PlayPause.updateColour(CUSTOM_GREEN); // update color
    btn_PlayPause.drawButton(); // draw button
    btn_PlayPause.setToggleActive(false); // set button state to "pause" mode
  }


  /***********************************************************************
  Used to reset autostack after completed stack.
  ***********************************************************************/
  void resetPhoto360() {
    // autoStackPaused = false;
    // autoStackInitiated = false;
    // isNewAutoStack = true;
    btn_PlayPause.drawButton(BLACK); // replace existing button
    btn_PlayPause.updateBitmap(play); // update to show play button
    btn_PlayPause.updateColour(CUSTOM_GREEN); // update color
    btn_PlayPause.drawButton(); // draw button
  }


  void func_ArrowUp(bool btnActive) {
    // if (btnActive && canEditPhotoNr() && areArrowsEnabled()) {
    //   // incrementStepsPerMovement();
    //   // calculateStepSize();
    //   btn_PhotoNr.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getNr360Photos()));
    // }
  }


  void func_ArrowDown(bool btnActive) {
    // if (btnActive && canEditPhotoNr() && areArrowsEnabled()) {
    //   // decrementStepsPerMovement();
    //   // calculateStepSize();
    //   btn_PhotoNr.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getNr360Photos()));
    // }
  }


  /***********************************************************************
  Updates the autoStack screen with current progress of stack sequence.
  Uses sprintf_P function to concatenate two values and format them as
  "completed / remaining" on the screen.
  ***********************************************************************/
  void printPhoto360Progress() {
    // char photo360Progress[10]  = "0/0";

    // // format progress in "Completed / Total" string
    // sprintf_P(photo360Progress, PSTR("%02d/%02d"), getNrMovementsCompleted(), getNrMovementsRequired());
    // // print to screen
    // btn_Progress.writeTextBottomCentre(Arimo_Bold_30, WHITE, photo360Progress);
  }


  void hideArrows(bool hide) {
    btn_ArrowUp.hideButton(hide);
    btn_ArrowDown.hideButton(hide);
    btn_PlayPause.hideButton(!hide); // PlayPause takes opposite state to arrow buttons
  }
}
