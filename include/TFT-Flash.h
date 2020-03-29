#ifndef TFTFLASH_H
#define TFTFLASH_H

#include <Arduino.h>
#include "VariableDeclarations.h"

namespace flash_screen {
  void initFlashButtons();
  void populateFlashScreen();
  void checkFlashButtons(int touch_x, int touch_y, int touch_z);
  void func_FlashOff(bool btnActive);
  void func_FlashOn(bool btnActive);
  void func_FlashTest(bool btnActive);
  void func_Back(bool btnActive);
}
#endif
