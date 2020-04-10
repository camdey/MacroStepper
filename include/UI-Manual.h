#ifndef UIMANUAL_H
#define UIMANUAL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

namespace manual_screen {
  void initManualButtons();
  void populateManualScreen();
  void checkManualButtons(int touch_x, int touch_y, int touch_z);
  void func_StepDistance(bool btnActive);
  void func_Flash(bool btnActive);
  void func_Reset(bool btnActive);
  void func_Back(bool btnActive);
  void func_ArrowUp(bool btnActive);
  void func_ArrowDown(bool btnActive);
  void displayPosition();
}
#endif