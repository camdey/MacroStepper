#ifndef UIPHOTO360CONFIG_H
#define UIPHOTO360CONFIG_H

#include <Arduino.h>
#include "VariableDeclarations.h"
#include "GlobalVariables.h"


namespace photoconfig_screen {
  void initPhoto360ConfigButtons();
  void populatePhoto360ConfigScreen();
  void checkPhoto360ConfigButtons(int touch_x, int touch_y);
  void func_90(bool btnActive);
  void func_180(bool btnActive);
  void func_360(bool btnActive);
  void func_Back(bool btnActive);
  void func_Direction(bool btnActive);
  void func_ArrowUp(bool btnActive);
  void func_ArrowDown(bool btnActive);
  void resetPhoto360Config();
  void hideArrows(bool hide);
}

#endif
