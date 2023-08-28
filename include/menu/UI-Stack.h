#ifndef UISTACK_H
#define UISTACK_H

#include <Arduino.h>
#include "VariableDeclarations.h"

namespace stack_screen {
    void initStackButtons();
    void populateStackScreen();
    void checkStackButtons(int touch_x, int touch_y);
    void func_Auto(bool btnActive);
    void func_Manual(bool btnActive);
    void func_HomeRail(bool btnActive);
    void func_Target(bool btnActive);
    void func_Back(bool btnActive);
}
#endif
