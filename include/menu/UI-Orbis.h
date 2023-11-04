#ifndef UIORBIS_H
#define UIORBIS_H

#include <Arduino.h>
#include "VariableDeclarations.h"


namespace orbis_screen {
    void initOrbisButtons();
    void populateOrbisScreen();
    void checkOrbisButtons(int touch_x, int touch_y);
    void func_Video(bool btnActive);
    void func_Photo(bool btnActive);
}

#endif
