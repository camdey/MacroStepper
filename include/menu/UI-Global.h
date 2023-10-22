#ifndef UIGLOBAL_H
#define UIGLOBAL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

namespace global {
    void initGlobalButtons();
    void func_Flash(bool btnActive);
    void func_Reset(bool btnActive);
    void func_Home(bool btnActive);
    void func_Settings(bool btnActive);
    void func_FlashPage(bool btnActive);
    void func_Target(bool btnActive);
    extern gfxButton btn_Flash;
    extern gfxButton btn_Reset;
    extern gfxButton btn_Home;
    extern gfxButton btn_Settings;
    extern gfxButton btn_FlashPage;
    extern gfxButton btn_Target;
}
#endif
