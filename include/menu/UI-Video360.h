#ifndef UIVIDEO360_H
#define UIVIDEO360_H

#include <Arduino.h>
#include "VariableDeclarations.h"


namespace video_screen {
    void initVideo360Buttons();
    void populateVideo360Screen();
    void checkVideo360Buttons(int touch_x, int touch_y);
    void func_Speed(bool btnActive);
    void func_Direction(bool btnActive);
    void func_Back(bool btnActive);
    void func_PlayPause(bool btnActive);
    void func_ArrowUp(bool btnActive);
    void func_ArrowDown(bool btnActive);
    void hideArrows(bool hide);
}

#endif
