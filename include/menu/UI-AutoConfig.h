#ifndef UIAUTOCONFIG_H
#define UIAUTOCONFIG_H

#include <Arduino.h>
#include "VariableDeclarations.h"

namespace autoconfig_screen {
    void initAutoConfigButtons();
    void populateAutoConfigScreen();
    void checkAutoConfigButtons(int touch_x, int touch_y);
    void func_Start(bool btnActive);
    void func_End(bool btnActive);
    void func_Config(bool btnActive);
    void func_Run(bool btnActive);
    void func_Delay(bool btnActive);
    void func_Back(bool btnActive);
    void func_ArrowUp(bool btnActive);
    void func_ArrowDown(bool btnActive);
    void updateStartPosition();
    void updateEndPosition();
    void printShutterDelay();
    void printPosition();
}
#endif
