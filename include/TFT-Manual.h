#ifndef TFTMANUAL_H
#define TFTMANUAL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

namespace manual_screen {
// extern gfxButton btn_test;
// extern gfxButton btn_StepDistance;
// extern gfxButton btn_StepNumber;
// extern gfxButton btn_Position;
// extern gfxButton btn_Flash;
// extern gfxButton btn_Reset;
// extern gfxButton btn_Back;
// extern gfxButton btn_ArrowUp;
// extern gfxButton btn_ArrowDown;
// extern gfxTouch  tch_StepDistance;
// extern gfxTouch  tch_Flash;
// extern gfxTouch  tch_Reset;
// extern gfxTouch  tch_Back;
// extern gfxTouch  tch_ArrowUp;
// extern gfxTouch  tch_ArrowDown;

void initManualButtons();
void populateManualScreen();
void checkManualButtons(int touch_x, int touch_y, int touch_z);
void func_StepDistance(bool btnActive);
void func_StepNumber();
void func_Position();
void func_Flash(bool btnActive);
void func_Reset(bool btnActive);
void func_Back(bool btnActive);
void func_ArrowUp(bool btnActive);
void func_ArrowDown(bool btnActive);
}
#endif
