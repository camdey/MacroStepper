#ifndef UIPHOTO360_H
#define UIPHOTO360_H

#include <Arduino.h>
#include "VariableDeclarations.h"
#include "GlobalVariables.h"


namespace photo_screen {
    void initPhoto360Buttons();
    void populatePhoto360Screen();
    void checkPhoto360Buttons(int touch_x, int touch_y);
    void func_PhotoNr(bool btnActive);
    void func_Delay(bool btnActive);
    void func_Config(bool btnActive);
    void func_Back(bool btnActive);
    void func_PlayPause(bool btnActive);
    void func_ArrowUp(bool btnActive);
    void func_ArrowDown(bool btnActive);
    void pausePhoto360();
    void resetPhoto360();
    void printPhoto360Progress();
    void hideArrows(bool hide);
}

#endif
