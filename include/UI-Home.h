#ifndef UIHOME_H
#define UIHOME_H

#include <Arduino.h>
#include "VariableDeclarations.h"

namespace home_screen {
  void initHomeButtons();
  void populateHomeScreen();
  void checkHomeButtons(int touch_x, int touch_y, int touch_z);
  void func_Auto(bool btnActive);
  void func_Manual(bool btnActive);
  void func_Home(bool btnActive);
  void func_Flash(bool btnActive);
  void func_Rotate(bool btnActive);
}
#endif
