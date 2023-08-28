#ifndef UIFLASH_H
#define UIFLASH_H

#include <Arduino.h>
#include "VariableDeclarations.h"

namespace flash_screen {
    void initFlashButtons();
    void populateFlashScreen();
    void checkFlashButtons(int touch_x, int touch_y);
    void func_FlashOff(bool btnActive);
    void func_FlashOn(bool btnActive);
    void func_FlashTest(bool btnActive);
    void func_Back(bool btnActive);
    void func_FlashSensor(bool btnActive);
    void updateGodoxValue();
}
#endif
