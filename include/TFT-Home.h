#ifndef TFTHOME_H
#define TFTHOME_H

#include <Arduino.h>
#include "VariableDeclarations.h"

// extern gfxButton btn_test;
// extern gfxButton btn_Auto;
// extern gfxButton btn_Manual;
// extern gfxButton btn_Home;
// extern gfxButton btn_Flash;
// extern gfxButton btn_Rotate;
// extern gfxTouch  tch_Auto;
// extern gfxTouch  tch_Manual;
// extern gfxTouch  tch_Home;
// extern gfxTouch  tch_Flash;
// extern gfxTouch  tch_Rotate;

void initHomeButtons();
void populateHomeScreen();
void checkHomeButtons(int touch_x, int touch_y, int touch_z);
void func_Auto(bool btnActive);
void func_Manual(bool btnActive);
void func_Home(bool btnActive);
void func_Flash(bool btnActive);
void func_Rotate(bool btnActive);

#endif
