#ifndef UICONFIG_H
#define UICONFIG_H

#include <Arduino.h>
#include "VariableDeclarations.h"

namespace config_screen {
        int nrButtons();
        void initConfigButtons();
        void populateConfigScreen();
        void checkConfigButtons(int touch_x, int touch_y);
        void func_Back(bool btnActive);
        void func_FlipScreen(bool btnActive);
        void func_Piezo(bool btnActive);
        void func_LED(bool btnActive);
        void func_Stepper(bool btnActive);
        void func_markStack(bool btnActive);
}
#endif
