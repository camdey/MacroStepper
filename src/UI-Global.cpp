#include "GlobalVariables.h"
#include "UI-Global.h"
#include "UI-Main.h"

namespace global {
  gfxButton btn_Flash = btn.initBitmapButton(flashOff,  220,    20, 80, 80, CUSTOM_RED, true);


  void initGlobalButtons() {
    btn_Flash.addToggle(func_Flash, 0);
  }

  void func_Flash(bool btnActive) {
    if (btnActive) {
      setShutterEnabled(true);
      btn_Flash.updateBitmap(flashOn);
      btn_Flash.updateColour(CUSTOM_GREEN);
      btn_Flash.drawNewBitmap(flashOn, CUSTOM_GREEN);
    }
    else if (!btnActive) {
      setShutterEnabled(false);
      btn_Flash.updateBitmap(flashOff);
      btn_Flash.updateColour(CUSTOM_RED);
      btn_Flash.drawNewBitmap(flashOff, CUSTOM_RED);
    }
  }
}