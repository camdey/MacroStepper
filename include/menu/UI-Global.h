#ifndef UIGLOBAL_H
#define UIGLOBAL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

namespace global {
    void initGlobalButtons();
    void func_Flash(bool btnActive);
    void func_Reset(bool btnActive);
    void func_Home(bool btnActive);
    void func_Stack(bool btnActive);
    void func_Orbis(bool btnActive);
    extern gfxButton btn_Flash;
    extern gfxButton btn_Reset;
    extern gfxButton btn_Home;
    extern gfxButton btn_Stack;
    extern gfxButton btn_360;
    extern gfxButton btn_3D;
}
#endif
