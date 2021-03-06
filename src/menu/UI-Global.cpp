#include "GlobalVariables.h"
#include "menu/UI-Global.h"
#include "menu/UI-Main.h"
#include "menu/UI-Auto.h"

namespace global {
  gfxButton btn_Flash = btn.initBitmapButton(flashOff,  220,  20,   80, 80, CUSTOM_RED, true  );
  gfxButton btn_Reset = btn.initBitmapButton(cancel,    220,  120,  80, 80, BLACK,      true  );


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
      // setStartPosition(driver.XACTUAL());
      // setEndPosition(driver.XACTUAL());
      btn_Reset.updateColour(BLACK);
      btn_Reset.drawButton(BLACK);
    }
    // if function called from elsewhere...
    else if (autoStackInitiated) {
      btn_Reset.updateColour(CUSTOM_RED);
    }
  }
}