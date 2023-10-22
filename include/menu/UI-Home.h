#ifndef UIHOME_H
#define UIHOME_H

#include <Arduino.h>
#include "VariableDeclarations.h"

namespace home_screen {
    void initHomeButtons();
    void populateHomeScreen();
    void checkHomeButtons(int touch_x, int touch_y);
    void func_Stack(bool btnActive);
    void func_Orbis(bool btnActive);
    void func_Target(bool btnActive);
    void func_Flash(bool btnActive);
    void func_Config(bool btnActive);
}
#endif
