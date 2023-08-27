#include "GlobalVariables.h"
#include "menu/UI-Global.h"
#include "menu/UI-Main.h"
#include "menu/UI-Auto.h"
#include "menu/UI-Photo360.h"

namespace global {
  gfxButton btn_Flash = btn.initBitmapButton(flashOff,  200,  20,   80, 80, CUSTOM_RED, true  );
  gfxButton btn_Reset = btn.initBitmapButton(cancel,    200,  120,  80, 80, BLACK,      true  );


  void initGlobalButtons() {
    btn_Flash.addToggle(func_Flash, 0);
    btn_Reset.addMomentary(func_Reset, 0);
  }

  void func_Flash(bool btnActive) {
    if (btnActive) {
      setShutterEnabled(true);
      btn_Flash.updateBitmap(flashOn);
      btn_Flash.updateColour(CUSTOM_GREEN);
      btn_Flash.drawNewBitmap(flashOn, CUSTOM_GREEN);
      auto_screen::stackStatus(newStep);
    }
    else if (!btnActive) {
      setShutterEnabled(false);
      btn_Flash.updateBitmap(flashOff);
      btn_Flash.updateColour(CUSTOM_RED);
      btn_Flash.drawNewBitmap(flashOff, CUSTOM_RED);
      auto_screen::stackStatus(newStep);
    }
  }

  // reset AutoStack procedure to default values
  void func_Reset(bool btnActive) {
    // if button pressed and autostack active...
    if (btnActive && autoStackInitiated) {
      autoStackInitiated = false;
      isNewAutoStack = true;
      autoStackPaused = false;
      setNrMovementsCompleted(0);
      // setNrMovementsRequired(0);
      // setStartPosition(stepper1.XACTUAL());
      // setEndPosition(stepper1.XACTUAL());
      btn_Reset.updateColour(BLACK);
      btn_Reset.drawButton(BLACK);
      setCurrentStage(idle);
    }
    // if button pressed and photo360 active...
    else if (btnActive && photo360Initiated) {
      photo360Paused = false;
      photo360Initiated = false;
      isNewPhoto360 = true;
      setNrCompleted360Photos(0);
      btn_Reset.updateColour(BLACK);
      btn_Reset.drawButton(BLACK);
      // setTargetVelocity(STEALTH_CHOP_VMAX);
      setCurrentStage(idle);
    }
    // if function called from elsewhere...
    else if ((!btnActive && autoStackInitiated) || photo360Initiated) {
      btn_Reset.updateColour(CUSTOM_RED);
    }
  }
}