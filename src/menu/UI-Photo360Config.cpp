#include "GlobalVariables.h"
#include "StepperControl.h"
#include "menu/UI-Main.h"
#include "menu/UI-Photo360.h"
#include "menu/UI-Photo360Config.h"
#include "menu/UI-Global.h"

namespace photoconfig_screen {
  #define num_btns 6
  gfxButton *btn_array[num_btns];


  gfxButton btn_PhotoNr       =   btn.initButton("Nr Photos",   "fillRoundRect",  0,    20,   160,  80,   15, DARKGRAY,   true  );
  gfxButton btn_Direction     =   btn.initBitmapButton(dir_cw,    200,  20,   80,   80,   CUSTOM_GREEN, true  );
  gfxButton btn_Back          =   btn.initBitmapButton(backArrow, 200,  220,  80,   80,   WHITE,        true  );
  gfxButton btn_ArrowUp       =   btn.initBitmapButton(arrowUp,   350,  20,   120,  120,  CUSTOM_GREEN, true  );
  gfxButton btn_ArrowDown     =   btn.initBitmapButton(arrowDown, 350,  180,  120,  120,  CUSTOM_RED,   true  );


  void initPhoto360ConfigButtons() {
    btn_array[0] = &btn_PhotoNr;
    btn_array[1] = &btn_Back;
    btn_array[2] = &btn_ArrowUp;
    btn_array[3] = &btn_ArrowDown;
    btn_array[4] = &btn_Direction;
    btn_array[5] = &global::btn_Reset;

    btn_PhotoNr.addToggle(func_PhotoNr,         0 );
    btn_Direction.addToggle(func_Direction,     0 );
    btn_Back.addMomentary(func_Back,            0 );
    btn_ArrowUp.addMomentary(func_ArrowUp,      0 );
    btn_ArrowDown.addMomentary(func_ArrowDown,  0 );

    btn_PhotoNr.addBorder(3, WHITE);
  }


  void populatePhoto360ConfigScreen() {
    setCurrentScreen("Photo360Config");

    // draw buttons
    for (int i=0; i < num_btns; i++) {
      if (!btn_array[i]->isHidden()) { // if button is not hidden, draw it
        btn_array[i]->drawButton();
      }
    }

    // draw text
    btn_PhotoNr.writeTextTopCentre(Arimo_Regular_30, WHITE);
    btn_PhotoNr.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getNr360Photos()));
  }


  void checkPhoto360ConfigButtons(int touch_x, int touch_y) {
    for (int i=0; i < num_btns; i++) {
      if (btn_array[i]->isTactile()) {
        btn_array[i]->contains(touch_x, touch_y);
      }
    }
  }


  void func_PhotoNr(bool btnActive) {
    if (btnActive) {
    }
    else if (!btnActive) {
    }
  }


  void func_Back(bool btnActive) {
    if (btnActive) {
      populateScreen("Photo360");
    }
  }


  void func_Direction(bool btnActive) {
    if (btnActive) {
      btn_Direction.drawButton(BLACK); // replace existing button
      btn_Direction.updateBitmap(dir_ccw); // update bitmap image
      btn_Direction.updateColour(CUSTOM_RED); // change colour
      btn_Direction.drawButton(); // draw
      setStepperDirCW(false);
    }
    else if (!btnActive) {
      btn_Direction.drawButton(BLACK); // replace existing button
      btn_Direction.updateBitmap(dir_cw); // update bitmap image
      btn_Direction.updateColour(CUSTOM_GREEN); // change colour
      btn_Direction.drawButton(); // draw
      setStepperDirCW(true);
    }
  }


  void func_ArrowUp(bool btnActive) {
    if (btnActive) {
      // calculate number of microsteps to move for each press based on current nrPhotos setting
      int nrSteps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) / getNr360Photos();
      readyStealthChop();
      setTargetVelocity(360);
      driver.XTARGET(driver.XACTUAL()+nrSteps);
    }
  }


  void func_ArrowDown(bool btnActive) {
    if (btnActive) {
      // calculate number of microsteps to move for each press based on current nrPhotos setting
      int nrSteps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) / getNr360Photos();
      readyStealthChop();
      setTargetVelocity(360);
      driver.XTARGET(driver.XACTUAL()-nrSteps);
    }
  }
}
